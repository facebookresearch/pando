#!/usr/bin/env python3
# Note: you can freely edit this file and changes will not be overwritten.
# See README.md in //python/wheel/ for more details.

import six
import unittest


class BasicTestCase(unittest.TestCase):
    """
    Some simple tests copied from six directly
    https://github.com/benjaminp/six/blob/master/test_six.py
    """
    def test_integer_types(self):
        assert isinstance(1, six.integer_types)
        assert isinstance(-1, six.integer_types)
        assert isinstance(six.MAXSIZE + 23, six.integer_types)
        assert not isinstance(.1, six.integer_types)

    def test_string_types(self):
        assert isinstance("hi", six.string_types)
        assert isinstance(six.u("hi"), six.string_types)
        assert issubclass(six.text_type, six.string_types)

    def test_text_type(self):
        assert type(six.u("hi")) is six.text_type

    def test_binary_type(self):
        assert type(six.b("hi")) is six.binary_type


if __name__ == '__main__':
    unittest.main()
