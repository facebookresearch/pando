#!/usr/bin/env python3
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

import unittest
from enum import auto, Enum

from ..api import inputs, returns, create_graph, register_transform


class FakeType(Enum):
    TEST1 = auto()
    TEST2 = auto()
    TEST3 = auto()
    TEST4 = auto()
    TEST5 = auto()


@returns(FakeType.TEST1)
def compute_test1() -> int:
    return 5


@returns(FakeType.TEST2)
def compute_test2() -> int:
    return 2


@inputs(FakeType.TEST1, FakeType.TEST2)
@returns(FakeType.TEST3)
def compute_test3(one: int, two: int) -> int:
    return two - one


@inputs(FakeType.TEST1, FakeType.TEST3)
@returns(FakeType.TEST4)
def compute_test4(one: int, three: int, extra: int = 0) -> int:
    return three + one + extra


def compute_test5(one: int, four: int) -> int:
    return one + four


register_transform(
    compute_test5, inputs=[FakeType.TEST1, FakeType.TEST4], returns=FakeType.TEST5
)


class ConditionalType(Enum):
    SOURCE1 = auto()
    SOURCE2 = auto()
    TRANSFORM = auto()
    TRANSFORM2 = auto()


@returns(ConditionalType.SOURCE1)
def source1() -> int:
    return 1


@returns(ConditionalType.SOURCE2)
def source2() -> int:
    return 2


# Now we have 2 different functions which provide ConditionalType.TRANSFORM
# where the function to run will be selected by whichever source / input
# is marked as available
@inputs(ConditionalType.SOURCE1)
@returns(ConditionalType.TRANSFORM)
def transform1(source1: int) -> int:
    return source1


@inputs(ConditionalType.SOURCE2)
@returns(ConditionalType.TRANSFORM)
def transform1(source2: int) -> int:
    return source2


@inputs(ConditionalType.TRANSFORM)
@returns(ConditionalType.TRANSFORM2)
def transform2(transform1_val: int) -> int:
    return transform1_val + 1


class ApiTests(unittest.TestCase):
    def test_basic_graph(self) -> None:
        graph = create_graph(FakeType)
        self.assertEqual(graph[FakeType.TEST4], 2)
        self.assertEqual(graph[FakeType.TEST5], 7)

    def test_conditional_graph(self) -> None:
        source1_graph = create_graph(ConditionalType, {ConditionalType.SOURCE1})
        self.assertEqual(source1_graph.compute(ConditionalType.TRANSFORM), 1)
        self.assertEqual(source1_graph.compute(ConditionalType.TRANSFORM2), 2)

        source2_graph = create_graph(ConditionalType, {ConditionalType.SOURCE2})
        self.assertEqual(source2_graph.compute(ConditionalType.TRANSFORM), 2)
        self.assertEqual(source2_graph.compute(ConditionalType.TRANSFORM2), 3)
