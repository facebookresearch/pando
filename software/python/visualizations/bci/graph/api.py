#!/usr/bin/env python3
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

import functools
from collections.abc import Iterable
from enum import Enum
from typing import Callable, Dict, List, Optional, Set, Union

from .common import (
    DependencyTableT,
    GraphSpecT,
    TypedTransformFuncT,
    TransformTableT,
)
from .transformer import Transformer

# Note: different graphs will be delineated by different enums, which will
# materialize as one added layer on top of these tables, specifically
# graph enum -> DependencyTableT.  This will come in a couple diffs
# after enabling one global graph via decorators.

ALL_TRANSFORMS: Dict[GraphSpecT, List[TypedTransformFuncT]] = {}


def inputs(*args: Enum) -> Callable[[TypedTransformFuncT], TypedTransformFuncT]:
    """
    Function decorator used for declaring the input type(s) for
    a function.  The arguments here should be some enum that is used
    for structuring the different data types used between transform
    functions in the graph.
    """

    def decorator_inputs(func: TypedTransformFuncT) -> TypedTransformFuncT:
        func.input_types = args
        return func

    return decorator_inputs


def returns(*args: Enum) -> Callable[[TypedTransformFuncT], TypedTransformFuncT]:
    """
    Function decorator used for declaring the output type(s) for
    a function.  The arguments here should be some enum that is used
    for structuring the different data types used between transform
    functions in the graph.
    """
    if len(args) == 0:
        raise TypeError("Must specify at least one type")

    def decorator_returns(func: TypedTransformFuncT) -> TypedTransformFuncT:
        func.return_types = args
        _append_to_node_list(func)
        return func

    return decorator_returns


def register_transform(
    func: TypedTransformFuncT,
    inputs: Optional[List[Enum]],
    returns: Union[Enum, List[Enum]],
) -> None:
    """
    Registers func with the given inputs and returns.  This is
    a convenience function for re-using the same function on different
    data / types.
    """

    # we need this wrapper so if we register the same function more
    # than once, we don't overwrite the properties used for
    # defining the function inputs/outputs (graph edges)
    @functools.wraps(func)
    def wrapper(*args, **kwargs) -> object:
        return func(*args, **kwargs)

    if inputs is not None:
        wrapper.input_types = inputs
    return_types = returns if isinstance(returns, Iterable) else [returns]
    wrapper.return_types = return_types
    _append_to_node_list(wrapper)


def register_source(
    func: TypedTransformFuncT, returns: Union[Enum, List[Enum]]
) -> None:
    return register_transform(func, None, returns)


# this will soon take a Enum class as an argument
# for selecting what graph to instantiate.
def create_graph(
    graph_spec: GraphSpecT, available_sources: Optional[Set[Enum]] = None
) -> Transformer:
    """
    Constructs the Transformer for the given graph specification.
    Note that the graph specification is the Enum class used for
    defining the @inputs and @returns for the functions in the graph.
    """
    if graph_spec not in ALL_TRANSFORMS:
        raise RuntimeError(f"No functions registered for {graph_spec}")

    all_transforms = ALL_TRANSFORMS[graph_spec]
    valid_transforms = _find_nodes_from_roots(all_transforms, available_sources)

    dependencies: DependencyTableT = {}
    transforms: TransformTableT = {}
    sources: TransformTableT = {}

    for func in valid_transforms:
        if hasattr(func, "input_types"):
            for return_type in func.return_types:
                dependencies[return_type] = func.input_types
                transforms[return_type] = func
        else:
            for return_type in func.return_types:
                sources[return_type] = func

    return Transformer(dependencies, transforms, sources)


def _find_nodes_from_roots(
    all_transforms: List[TypedTransformFuncT],
    available_sources: Optional[Set[Enum]] = None,
) -> List[TypedTransformFuncT]:
    """
    Given a list of graph nodes and the available sources,
    we compute all of the valid graph nodes which can be computed
    from the given available_sources.  This involves first building
    a table from node to all nodes that depend on the given node,
    which we then use to do a breadth-first traversal of the graph
    for finding all of the dependent nodes.
    """
    if available_sources is None:
        return all_transforms

    source_funcs = []
    # construct the map from function -> dependents
    dependents: DependencyTableT = {}
    for func in all_transforms:
        returns_set = set(func.return_types)
        if not hasattr(func, "input_types"):
            if returns_set <= available_sources:
                source_funcs.append(func)
            continue

        for input_type in func.input_types:
            if input_type not in dependents:
                dependents[input_type] = []
            dependents[input_type].append(func)

    return _find_valid_transforms(dependents, available_sources) + source_funcs


def _find_valid_transforms(
    dependents: DependencyTableT, available_sources: Optional[Set[Enum]] = None
) -> List[TypedTransformFuncT]:
    """
    This does a breadth-first traversal of the graph described by
    dependents.  During the traversal, we only traverse nodes that
    have all of their inputs satisfied.
    """
    available_transforms = set(available_sources)
    valid_funcs = []
    traversing_transforms = []

    # get all of the starting nodes for the traversal
    for root in available_sources:
        if root not in dependents:
            continue
        traversing_transforms.extend(dependents[root])

    # if we haven't found any, this means that nothing depends on
    # the available_sources that were given
    if len(traversing_transforms) == 0:
        raise RuntimeError("No valid transforms found with given set of roots")

    # now we go through the entire graph, starting with the given
    # nodes, to find all of the transforms that *can* be executed,
    # meaning all the nodes that have their dependencies satisfied
    # as a result of the available_sources
    while len(traversing_transforms) > 0:
        transform = traversing_transforms.pop(0)

        if not set(transform.input_types).issubset(available_transforms):
            continue

        valid_funcs.append(transform)
        available_transforms |= set(transform.return_types)

        for return_type in transform.return_types:
            if return_type not in dependents:
                continue

            traversing_transforms.extend(dependents[return_type])

    return valid_funcs


def _append_to_node_list(func: TypedTransformFuncT) -> None:
    # the graph is specified implicitly by the enum
    # class being used
    graph_spec = type(func.return_types[0])
    if graph_spec not in ALL_TRANSFORMS:
        ALL_TRANSFORMS[graph_spec] = []
    func.uid = len(ALL_TRANSFORMS[graph_spec])
    ALL_TRANSFORMS[graph_spec].append(func)
