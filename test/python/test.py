#!/usr/local/bin/python

import unittest
import subprocess
import os
import shutil

class TestGraphitCompiler(unittest.TestCase):
    first_time_setup = True

    @classmethod
    def setUpClass(cls):
        build_dir = "../../build"
        if not os.path.isdir(build_dir):
            print "build the binaries"
            #shutil.rmtree("../../build_dir")
            os.mkdir(build_dir)
            os.chdir(build_dir)
            subprocess.call(["cmake", ".."])
            subprocess.call(["make"])
        else:
            os.chdir(build_dir)

        cwd = os.getcwd()

        cls.root_test_input_dir = "../test/input/"
        cls.cpp_compiler = "g++"
        cls.compile_flags = "-std=c++11"
        cls.include_path = "../src/runtime_lib"
        cls.output_file_name = "test.cpp"
        cls.executable_file_name = "test.o"


    def setUp(self):
        self.clean_up()

    #def tearDown(self):
        #self.clean_up()

    def clean_up(self):
        #clean up previously generated files
        if os.path.isfile(self.output_file_name):
            os.remove(self.output_file_name)
        if os.path.isfile(self.executable_file_name):
            os.remove(self.executable_file_name)

    # utilities for setting up tests

    def basic_compile_test(self, input_file_name):
        # "-f" and "-o" must not have space in the string, otherwise it doesn't read correctly
        graphit_compile_cmd = ["bin/graphitc", "-f", self.root_test_input_dir + input_file_name, "-o" , self.output_file_name]
        # check the return code of the call as a way to check if compilation happened correctly
        self.assertEqual(subprocess.call(graphit_compile_cmd), 0)
        # check if g++ compilation succeeded
        self.assertEqual(
            subprocess.call([self.cpp_compiler, self.compile_flags, "-I", self.include_path , self.output_file_name, "-o", self.executable_file_name]),
            0)
        self.assertEqual(subprocess.call(["./"+ self.executable_file_name]), 0)

    # expect to fail graphit compiler
    def basic_compile_test_graphit_compile_fail(self, input_file_name):
        graphit_compile_cmd = ["bin/graphitc", "-f", self.root_test_input_dir + input_file_name, "-o" , self.output_file_name]
        self.assertNotEqual(subprocess.call(graphit_compile_cmd), 0)

    # expect to fail c++ compiler (but will NOT fail graphit compiler)
    def basic_compile_test_cpp_compile_fail(self, input_file_name):
        graphit_compile_cmd = ["bin/graphitc", "-f", self.root_test_input_dir + input_file_name, "-o" , self.output_file_name]
        self.assertEqual(subprocess.call(graphit_compile_cmd), 0)
        self.assertNotEqual(
            subprocess.call([self.cpp_compiler, self.output_file_name, "-o", self.executable_file_name]),
            0)

    def expect_output_val(self, input_file_name, expected_output_val):
        self.basic_compile_test(input_file_name)
        proc = subprocess.Popen(["./"+ self.executable_file_name], stdout=subprocess.PIPE)
        #check the value printed to stdout is as expected
        output = proc.stdout.readline()
        print "output: " + output.strip()
        self.assertEqual(float(output.strip()), expected_output_val)

    # actual test cases

    def test_main(self):
        self.basic_compile_test("simple_main.gt")

    def test_main_fail(self):
        self.basic_compile_test_graphit_compile_fail("simple_main_fail.gt")

    def test_main_expect(self):
        self.expect_output_val("simple_main.gt", 4)

    def test_simple_func_add_no_return(self):
        self.basic_compile_test_cpp_compile_fail("simple_func_add_no_return.gt")

    def test_main_print_add(self):
        self.basic_compile_test("main_print_add.gt")

    def test_main_print_add_expect(self):
        self.expect_output_val("main_print_add.gt", 9)

    def test_simple_array(self):
        self.basic_compile_test("simple_array.gt")

    def test_simple_multi_arrays(self):
        self.basic_compile_test("simple_multi_arrays.gt")

    def test_simple_edgeset(self):
        self.basic_compile_test("simple_edgeset.gt")

    def test_simple_variable(self):
        self.basic_compile_test("simple_variable.gt")

    def test_simple_variable_expect(self):
        self.expect_output_val("simple_variable.gt", 0)

    def test_simple_vector_sum(self):
        self.basic_compile_test("simple_vector_sum.gt")

    def test_simple_vector_sum_expect(self):
        self.expect_output_val("simple_vector_sum.gt", 5)

    def test_simple_vertexset_apply(self):
        self.basic_compile_test("simple_vertexset_apply.gt")

    def test_simple_vertexset_apply_expect(self):
        self.expect_output_val("simple_vertexset_apply.gt", 10)

    def test_simple_vertex_edge_load_expect(self):
        self.expect_output_val("simple_vertex_edge_load.gt", 5)

    def test_simple_edgeset_apply(self):
        self.basic_compile_test("simple_edgeset_apply.gt")

    def test_simple_edgeset_apply_expect(self):
        self.expect_output_val("simple_edgeset_apply.gt", 7)

    def test_simple_for_loop(self):
        self.basic_compile_test("simple_for_loop.gt")

    def test_outdegree_sum(self):
        self.basic_compile_test("outdegree_sum.gt")


if __name__ == '__main__':
    unittest.main()
    # used for enabling a specific test

    # suite = unittest.TestLoader().loadTestsFromTestCase(TestGraphitCompiler)
    # suite = unittest.TestSuite()
    # suite.addTest(TestGraphitCompiler('test_main_print_add'))

    # suite = unittest.TestSuite()
    # suite.addTest(TestGraphitCompiler('test_simple_multi_arrays'))


    # suite = unittest.TestSuite()
    # suite.addTest(TestGraphitCompiler('test_simple_array'))

    # suite = unittest.TestSuite()
    # suite.addTest(TestGraphitCompiler('test_simple_edgeset'))

    # suite = unittest.TestSuite()
    # suite.addTest(TestGraphitCompiler('test_simple_vertex_edge_load_expect'))

    unittest.TextTestRunner(verbosity=2).run(suite)
