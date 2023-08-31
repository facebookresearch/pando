#!/usr/bin/env python3
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

import unittest
from enum import auto, Enum
from typing import Tuple

from ..api import inputs, returns, create_graph


class TestType(Enum):
    TYPE1 = auto()
    TYPE2 = auto()
    TYPE3 = auto()
    TYPE4 = auto()
    TYPE5 = auto()


class StatefulTransform:
    def __init__(self) -> None:
        self.calls = 0

    def transform(self) -> int:
        self.calls += 1
        return self.calls


@inputs(TestType.TYPE2, TestType.TYPE3)
@returns(TestType.TYPE1)
def compute_one(two: int, three: int, extra: int = 0) -> int:
    return two + three + extra


@inputs(TestType.TYPE4)
@returns(TestType.TYPE3)
def compute_three(four: int, extra: int = 0) -> int:
    return 20 + four + extra


@returns(TestType.TYPE4)
def source_four() -> int:
    return 30


@returns(TestType.TYPE2)
def source_two() -> int:
    return 10


class CyclicType(Enum):
    ONE = auto()
    TWO = auto()


@inputs(CyclicType.ONE)
@returns(CyclicType.TWO)
def cyclic_two(one: int) -> int:
    one + 1


@inputs(CyclicType.TWO)
@returns(CyclicType.ONE)
def cyclic_one(two: int) -> int:
    return two + 2


class MultiOutputType(Enum):
    ONE = auto()
    TWO = auto()


@returns(MultiOutputType.ONE, MultiOutputType.TWO)
def multiout() -> Tuple[int, float]:
    return 1, 2.5


class TransformerTests(unittest.TestCase):
    def test_recursive_dependencies(self) -> None:
        graph = create_graph(TestType)
        self.assertEqual(graph.compute(TestType.TYPE1), 60)

    def test_cyclic_dependencies(self) -> None:
        graph = create_graph(CyclicType)
        self.assertRaises(RuntimeError, graph.compute, CyclicType.ONE)

    def test_caching(self) -> None:
        stateful_transform = StatefulTransform()

        class StatefulType(Enum):
            ONE = auto()

        @returns(StatefulType.ONE)
        def stateful_one() -> int:
            return stateful_transform.transform()

        graph = create_graph(StatefulType)

        graph.caching = True
        graph.compute(StatefulType.ONE)
        self.assertEqual(graph.compute(StatefulType.ONE), 1)
        self.assertEqual(graph.compute(StatefulType.ONE), 1)

        graph.clear_cache()
        self.assertEqual(graph.compute(StatefulType.ONE), 2)

        graph.caching = False
        self.assertEqual(graph.compute(StatefulType.ONE), 3)
        self.assertEqual(graph.compute(StatefulType.ONE), 4)

    def test_inspect(self) -> None:
        graph = create_graph(TestType)
        info_list = graph.inspect(TestType.TYPE1)
        self.assertEqual(len(info_list), 4)
        self.assertEqual(
            [info.key for info in info_list],
            [TestType.TYPE2, TestType.TYPE4, TestType.TYPE3, TestType.TYPE1],
        )

    def test_kwargs(self) -> None:
        graph = create_graph(TestType)
        self.assertEqual(graph.compute(TestType.TYPE1, extra=10), 70)

    def test_kwargs_by_type(self) -> None:
        graph = create_graph(TestType)
        self.assertEqual(
            graph.compute(
                TestType.TYPE1,
                kwargs_by_type={
                    TestType.TYPE1: {"extra": 10},
                    TestType.TYPE3: {"extra": 15},
                },
            ),
            85,
        )

    def test_default_kwargs_by_type(self) -> None:
        graph = create_graph(TestType)
        graph.default_kwargs_by_type = {
            TestType.TYPE1: {"extra": 10},
            TestType.TYPE3: {"extra": 15},
        }
        # the defaults specified above should be overridden by what is passed
        # into the kwargs_by_type argument in the graph.compute() call
        self.assertEqual(
            graph.compute(
                TestType.TYPE1, kwargs_by_type={TestType.TYPE1: {"extra": 15}}
            ),
            90,
        )

    def test_kwargs_override(self) -> None:
        graph = create_graph(TestType)
        graph.default_kwargs_by_type = {
            TestType.TYPE1: {"extra": 10},
            TestType.TYPE3: {"extra": 15},
        }
        # all other arguments for TYPE1 should be overridden by the kwargs
        # of the graph.compute() function
        self.assertEqual(
            graph.compute(
                TestType.TYPE1, kwargs_by_type={TestType.TYPE1: {"extra": 15}}, extra=0
            ),
            75,
        )

    def test_kwargs_invalid_transform_type(self) -> None:
        graph = create_graph(TestType)
        with self.assertRaises(ValueError) as error:
            graph.default_kwargs_by_type = {TestType.TYPE5: {"extra": 10}}
        self.assertEqual(str(error.exception), "TestType.TYPE5 doesn't exist in graph")

    def test_kwargs_invalid_arg_name(self) -> None:
        graph = create_graph(TestType)
        with self.assertRaises(ValueError) as error:
            graph.default_kwargs_by_type = {TestType.TYPE1: {"wrong": 10}}
        self.assertEqual(
            str(error.exception), "TestType.TYPE1 has no argument named 'wrong'"
        )

    def test_kwargs_invalid_arg_type(self) -> None:
        graph = create_graph(TestType)
        with self.assertRaises(ValueError) as error:
            graph.default_kwargs_by_type = {TestType.TYPE1: {"extra": "wrong"}}
        self.assertEqual(
            str(error.exception),
            "TestType.TYPE1: expecting kwarg 'extra' to be of type <class 'int'>, but got <class 'str'>",
        )

    def test_multioutput(self) -> None:
        graph = create_graph(MultiOutputType)
        self.assertEqual(graph.compute(MultiOutputType.ONE), 1)
        self.assertEqual(graph.compute(MultiOutputType.TWO), 2.5)

    def test_copy_basic(self) -> None:
        graph = create_graph(TestType)
        graph2 = graph.clone()

        self.assertEqual(graph.compute(TestType.TYPE3), graph2.compute(TestType.TYPE3))

    def test_override_basic(self) -> None:
        graph = create_graph(TestType)
        graph2 = graph.clone()

        @graph2.inputs(TestType.TYPE4)
        @graph2.returns(TestType.TYPE3)
        def override_func(four: int) -> int:
            return four - 20

        self.assertEqual(graph.compute(TestType.TYPE3), 50)
        self.assertEqual(graph2.compute(TestType.TYPE3), 10)


if __name__ == "__main__":
    unittest.main()
