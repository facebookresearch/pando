#!/usr/bin/env python3
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

from enum import Enum
from typing import Callable, Dict, List, Type, Union

GraphSpecT = Type[Enum]
DataDictT = Dict[Enum, object]
TransformFuncT = Callable[[DataDictT], object]
# TransformFuncT will be deleted and this will be renamed
# once the migration is over
TypedTransformFuncT = Callable[..., object]

# source functions don't depend on any data
SourceFuncT = Callable[[], object]
NodeT = Union[TransformFuncT, SourceFuncT, TypedTransformFuncT]
DependencyTableT = Dict[Enum, List[Enum]]
TransformTableT = Dict[Enum, TypedTransformFuncT]
