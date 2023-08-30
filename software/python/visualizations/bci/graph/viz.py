#!/usr/bin/env python3
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

from enum import Enum
from typing import Dict, List, Set

import graphviz

from .transformer import TypeInfo


def show_steps(execution_steps: List[TypeInfo]) -> graphviz.Digraph:
    diagram = graphviz.Digraph()
    func_id_to_name = {step.func_id: step.func.__name__ for step in execution_steps}

    type_to_func_id = {step.key: step.func_id for step in execution_steps}

    for func_id, func_name in func_id_to_name.items():
        diagram.node(str(func_id), label=func_name)

    # since we are looping over outgoing edges, we need to make sure we
    # don't end up duplicating edges in the visualization
    added_edges_by_func_id: Dict[int, Set[Enum]] = {}
    for step in execution_steps:
        if step.dependencies is None:
            continue

        if step.func_id not in added_edges_by_func_id:
            added_edges_by_func_id[step.func_id] = set()

        for dependency in step.dependencies:
            if dependency in added_edges_by_func_id[step.func_id]:
                continue

            dep_func_id = type_to_func_id[dependency]
            if dep_func_id in added_edges_by_func_id[step.func_id]:
                # since we are looping over incoming edges here, we need
                # to de-duplicate
                continue

            added_edges_by_func_id[step.func_id].add(dependency)
            diagram.edge(str(dep_func_id), str(step.func_id), label=str(dependency))

    return diagram
