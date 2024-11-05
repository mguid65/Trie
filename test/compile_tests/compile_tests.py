from utility import *

import unittest


class TestCompilationSanityChecks(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if not check_compiler_exists("g++"):
            print("Failed to find compiler")
            exit(1)

        cls.compiler = Compiler("g++", "-I../../include", "-std=c++20")

    def test_same_size_comparison(self):
        self.assertTrue(self.compiler.compiles("tests/typo.cpp"))

    @classmethod
    def tearDownClass(cls):
        cls.compiler.cleanup()


if __name__ == '__main__':
    unittest.main()
