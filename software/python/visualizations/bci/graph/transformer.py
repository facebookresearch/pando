#!/usr/bin/env python3
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

import logging
from collections.abc import Iterable
from enum import Enum
from typing import (
    Any,
    Callable,
    Dict,
    List,
    NamedTuple,
    Optional,
    Set,
    Tuple,
    get_type_hints,
)

from .common import (
    DependencyTableT,
    NodeT,
    TransformFuncT,
    TypedTransformFuncT,
    SourceFuncT,
)


class TypeInfo(NamedTuple):
    key: Enum
    return_type: type
    args: List[type]
    func: TransformFuncT
    func_id: int
    dependencies: Optional[List[Enum]]


class Transformer:
    """
    Responsible for dealing with static computational graph definitions and
    executing transforms defined in the constructed graph
    """

    dependencies: DependencyTableT
    transforms_by_type: Dict[Enum, NodeT]
    cache: Dict[Enum, object]
    caching: bool

    def __init__(
        self,
        dependencies: DependencyTableT,
        transforms: Optional[Dict[Enum, TransformFuncT]] = None,
        sources: Optional[Dict[Enum, SourceFuncT]] = None,
        _verify_transforms: bool = True,
    ) -> None:
        self.dependencies = dependencies
        self.transforms_by_type = {}
        self.cache = {}
        self.caching = False
        self._max_id = 0
        self._default_kwargs_by_type = None

        if transforms is not None:
            self.add_transforms(transforms, _verify_transforms)

        if sources is not None:
            self.add_sources(sources)

    def __getitem__(self, key: Enum) -> object:
        return self.compute(key)

    @property
    def default_kwargs_by_type(self) -> Optional[Dict[Enum, Dict[str, Any]]]:
        return self._default_kwargs_by_type

    @default_kwargs_by_type.setter
    def default_kwargs_by_type(
        self, kwargs_by_type: Optional[Dict[Enum, Dict[str, Any]]]
    ) -> None:
        if kwargs_by_type is not None:
            # remove Nones
            kwargs_by_type = {
                key: val for key, val in kwargs_by_type.items() if val is not None
            }
        self._validate_kwargs_by_type(kwargs_by_type)
        self._default_kwargs_by_type = kwargs_by_type

    def clone(self) -> "Transformer":
        # we just create shallow copies of each member variable
        return Transformer(
            self.dependencies.copy(),
            self.transforms_by_type.copy(),
            _verify_transforms=False,
        )

    @property
    def all_types(self) -> Set[Enum]:
        return set(self.transforms_by_type.keys())

    def add_transforms(
        self, transforms: Dict[Enum, TransformFuncT], _verify_transforms: bool = True
    ) -> None:
        """
        Adds a set of transforms.  Note that the key used for each transform
        function given here must already exist in the dependency table given
        in the constructor
        """
        if _verify_transforms:
            for derived_type in transforms.keys():
                if derived_type not in self.dependencies:
                    raise RuntimeError(
                        f"{derived_type} has no dependencies so shouldn't be "
                        "defined as a transform type"
                    )
        self.transforms_by_type.update(transforms)

        # make sure we update the max unique id for functions so
        # we don't end up with collisions
        # we make the assumption that this transform has been
        # registered already using standard decorators / API functions
        # if we ever have a use case where this is not true,
        # we can easily update this to remove the assumption
        for transform_func in transforms.values():
            self._max_id = max(transform_func.uid, self._max_id)

    def add_sources(self, sources: Dict[Enum, SourceFuncT]) -> None:
        """
        Adds a set of sources, where a source is a function which has no
        dependencies defined in the dependency table given in the constructor
        """
        for derived_type, source_func in sources.items():
            self.add_source(derived_type, source_func)

    def add_source(self, derived_type: Enum, source: SourceFuncT) -> None:
        """
        Adds a single source, where a source is a function which has no
        dependencies defined in the dependency table
        """
        if not callable(source):
            raise RuntimeError(f"Provided {derived_type} needs to be callable")

        if not self._is_source_type(derived_type):
            raise RuntimeError(
                f"{derived_type} cannot be a source type since it has 1 "
                "or more dependencies defined"
            )

        if derived_type in self.transforms_by_type:
            raise RuntimeError(
                f"{derived_type} already set in transform function lookup table!"
            )

        wrapped_source = self._wrap_source(derived_type, source)
        self.transforms_by_type[derived_type] = wrapped_source
        self._max_id = max(wrapped_source.uid, self._max_id)

    def inputs(
        self, *args: Enum
    ) -> Callable[[TypedTransformFuncT], TypedTransformFuncT]:
        """
        Function decorator used for declaring input types to a function
        which will override any existing function dependency declarations
        associated with the function being decorated, but only for this
        specific graph instance.
        """
        if len(args) == 0:
            raise TypeError("Must specify at least one type")

        def decorator_inputs(func: TypedTransformFuncT) -> TypedTransformFuncT:
            if not hasattr(func, "return_types"):
                raise TypeError("@Transformer.returns() decorator missing")

            func.input_types = args

            # update the dependency table
            for return_type in func.return_types:
                self.dependencies[return_type] = func.input_types

            return func

        return decorator_inputs

    def returns(
        self, *args: Enum
    ) -> Callable[[TypedTransformFuncT], TypedTransformFuncT]:
        """
        Function decorator for declaring a new function which provides
        the given return types.  This overrides the existing functions
        set for the given return types for this specific graph instance.
        """
        if len(args) == 0:
            raise TypeError("Must specify at least one type")

        def decorator_returns(func: TypedTransformFuncT) -> TypedTransformFuncT:
            func.return_types = args
            func.uid = self._max_id + 1
            self._max_id += 1

            for return_type in func.return_types:
                self.transforms_by_type[return_type] = func

            return func

        return decorator_returns

    def clear_cache(self) -> None:
        """
        Resets the in-memory storage of all computed transforms
        """
        # to be used with a context manager in UniversalLoader
        self.cache = {}

    def exists(self, target: Enum) -> bool:
        return target in self.transforms_by_type

    def compute(
        self,
        target: Enum,
        kwargs_by_type: Optional[Dict[Enum, Dict[str, object]]] = None,
        **kwargs,
    ) -> object:
        """
        Determines the full list of dependencies for the given target and
        executes them in order for producing the returned result

        target is the type to be computed

        kwargs_by_type allows passing kwargs to intermediate transforms
            called when computing given target

        kwargs is for convenience of specifying kwargs for the transform
            defined for given target
        """
        # do some bookkeeping for figuring out what arguments to run with
        # TODO: invalidate cached results if effective kwargs changes!
        effective_kwargs_by_type = {}
        if self.default_kwargs_by_type is not None:
            effective_kwargs_by_type = self.default_kwargs_by_type.copy()
        if kwargs_by_type is not None:
            effective_kwargs_by_type.update(kwargs_by_type)
        if len(kwargs) > 0:
            # kwargs passed directly for the target type overrides the
            # defaults and passed in dictionary.  this is just for
            # a simple shorthand for passing args to the last transform
            # function call
            effective_kwargs_by_type[target] = kwargs
        # remove all Nones
        effective_kwargs_by_type = {
            key: val for key, val in effective_kwargs_by_type.items() if val is not None
        }
        self._validate_kwargs_by_type(effective_kwargs_by_type)

        cache: Dict[Enum, object] = self.cache if self.caching else {}
        transforms = self._get_ordered_transforms(target, set(cache.keys()))

        empty_kwargs = {}
        for derived_type, transform in transforms:
            if derived_type in cache:
                # already computed, skip.  Note that this case only happens
                # when some specified transform has multiple outputs
                continue

            # figure out what kwargs to use
            func_kwargs = empty_kwargs
            if derived_type in effective_kwargs_by_type:
                func_kwargs = effective_kwargs_by_type[derived_type]
            self._call_transform(derived_type, cache, transform, func_kwargs)

        return cache[target]

    def inspect(self, target: Enum) -> List[TypeInfo]:
        """
        Returns the ordered list of types that get computed for
        computing the given target type
        """
        transforms = self._get_ordered_transforms(target, set())
        transform_return_types = self._get_return_types_by_target(transforms)
        transform_args = self._get_args_by_target(transform_return_types)
        return [
            TypeInfo(
                key=key,
                return_type=transform_return_types[key],
                args=transform_args[key],
                func=func,
                func_id=func.uid,
                dependencies=self.dependencies[key]
                if key in self.dependencies
                else None,
            )
            for key, func in transforms
        ]

    def _call_transform(
        self,
        derived_type: Enum,
        cache: Dict[Enum, object],
        transform: TransformFuncT,
        kwargs: Dict[str, object],
    ) -> None:
        result = None
        if self._is_source_type(derived_type):
            # sources don't have any inputs
            result = transform(**kwargs)
        else:
            args = [cache[input_type] for input_type in transform.input_types]
            result = transform(*args, **kwargs)

        if len(transform.return_types) > 1:
            if not isinstance(result, Iterable):
                raise RuntimeError(
                    f"{derived_type} transform expected to return iterable"
                )
            if len(transform.return_types) != len(result):
                raise RuntimeError(
                    f"{derived_type} returned {len(result)} results, expected "
                    "{len(transform.return_types)}"
                )
            for output_type, result_item in zip(transform.return_types, result):
                cache[output_type] = result_item
        else:
            cache[derived_type] = result

    def _get_args_by_target(
        self, transform_return_types: Dict[Enum, type]
    ) -> Dict[Enum, List[type]]:
        args_by_target = {}
        for target in transform_return_types.keys():
            if target not in self.dependencies:
                args_by_target[target] = []
                continue

            args = [
                transform_return_types[dep]
                for dep in self.dependencies[target]
                if transform_return_types[dep] is not None
            ]
            args_by_target[target] = args
        return args_by_target

    def _get_return_types_by_target(
        self, transforms: List[Tuple[Enum, NodeT]]
    ) -> Dict[Enum, Optional[type]]:
        # We could do this in a more static manner but I'm not sure whether
        # we will be doing dynamic dependencies or anything at some point
        # in the near future

        transform_return_types: Dict[Enum, type] = {}
        for intermediate_target, transform in transforms:
            try:
                hints = get_type_hints(transform)
                target_type = hints["return"] if "return" in hints else None
                transform_return_types[intermediate_target] = target_type
            except TypeError:
                # There isn't really a more graceful way to handle this due
                # to how get_type_hints() is implemented
                transform_return_types[intermediate_target] = None
        return transform_return_types

    def _get_ordered_transforms(
        self, target: Enum, existing_transforms: Set[Enum]
    ) -> List[Tuple[Enum, NodeT]]:
        result = []
        traversed_transforms = set()
        self._get_ordered_transforms_recursive(
            target, existing_transforms, traversed_transforms, result
        )
        return result

    def _get_ordered_transforms_recursive(
        self,
        target: Enum,
        existing_transforms: Set[Enum],
        traversed_transforms: Set[Enum],
        result: List[Tuple[Enum, NodeT]],
    ) -> None:
        """
        This just amounts to a depth-first traversal through the graph, adding
        functions to the result list after each of the children have been
        added
        """
        if target in traversed_transforms and target not in existing_transforms:
            raise RuntimeError(f"Found cyclic dependency at {target}")
        traversed_transforms.add(target)

        if target in existing_transforms:
            # already processed this node in the graph
            return

        # add all the children
        if target in self.dependencies:
            for dep in self.dependencies[target]:
                self._get_ordered_transforms_recursive(
                    dep, existing_transforms, traversed_transforms, result
                )

        # process this node in the graph
        if target not in self.transforms_by_type:
            raise RuntimeError(f"Could not find {target} transform function")

        result.append((target, self.transforms_by_type[target]))
        existing_transforms.add(target)

    def _is_source_type(self, derived_type: Enum) -> bool:
        return derived_type not in self.dependencies or len(self.dependencies) == 0

    def _wrap_source(self, derived_type: Enum, source_func: SourceFuncT) -> SourceFuncT:
        wrapped_source = source_func
        if not hasattr(source_func, "return_types"):
            # this is for adhering to the properties of transform/source functions
            # defined in api.py
            def wrapped_source() -> object:
                return source_func()

            wrapped_source.return_types = [derived_type]
            # unique id will be out of order in this case but
            # the ordering doesn't matter
            wrapped_source.uid = self._max_id + 1
        return wrapped_source

    def _validate_kwargs_by_type(
        self, kwargs_by_type: Optional[Dict[Enum, Dict[str, Any]]]
    ) -> None:
        if kwargs_by_type is not None:
            for graph_type, type_kwargs in kwargs_by_type.items():
                self._validate_kwargs_for_type(graph_type, type_kwargs)

    def _validate_kwargs_for_type(
        self, graph_type: Enum, type_kwargs: Dict[str, Any]
    ) -> None:
        if graph_type not in self.transforms_by_type:
            raise ValueError(f"{graph_type} doesn't exist in graph")

        transform_func = self.transforms_by_type[graph_type]
        annotations = transform_func.__annotations__
        for keyword, value in type_kwargs.items():
            if keyword not in annotations:
                raise ValueError(f"{graph_type} has no argument named '{keyword}'")

            kwarg_type = annotations[keyword]
            try:
                if not isinstance(value, kwarg_type):
                    raise ValueError(
                        f"{graph_type}: expecting kwarg '{keyword}' to be "
                        f"of type {kwarg_type}, but got {type(value)}"
                    )
            except TypeError:
                logging.warning(
                    f"Could not validate kwarg {keyword} for {graph_type} "
                    f"with type annotation {kwarg_type} and provided "
                    f"value {value}. "
                    f"Don't worry everything is likely fine"
                )
