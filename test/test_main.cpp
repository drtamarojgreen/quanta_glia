#include <iostream>

// Declarations of all test functions
void test_analyze_empty();
void test_analyze_warning_only();
void test_analyze_type_error();
void test_analyze_value_error();
void test_analyze_syntax_error();
void test_analyze_import_error();
void test_analyze_other_error();
void test_analyze_warning_and_error_same_line();
void test_analyze_case_insensitive();

void test_produce_no_value_errors();
void test_produce_single_value_error();
void test_produce_multiple_value_errors();
void test_produce_string_replacement();

void test_log_reader_file();
void test_log_reader_stdin();

void test_environment_create_directory();
void test_environment_copy_source();
void test_environment_run_command();

void test_try_alternatives_basic_execute();
void test_try_alternatives_replace_line();
void test_try_alternatives_replace_range();
void test_try_alternatives_multiple_alternatives();
void test_try_alternatives_multiple_runs();

void test_full_pipeline_simple_case(); // From test_integration.cpp
void test_sample_cpp_log_analysis();   // From test_sample_logs.cpp
void test_sample_python_log_analysis(); // From test_sample_logs.cpp
void test_sample_java_log_analysis();   // From test_sample_logs.cpp

void run_glia_arch_tests(); // From test_glia_architecture.cpp
void run_tui_tests(); // From test_tui.cpp


int main() {
    std::cout << "Running tests...\n";
    
    run_glia_arch_tests();
    run_tui_tests();

    test_analyze_empty();
    test_analyze_warning_only();
    test_analyze_type_error();
    test_analyze_value_error();
    test_analyze_syntax_error();
    test_analyze_import_error();
    test_analyze_other_error();
    test_analyze_warning_and_error_same_line();
    test_analyze_case_insensitive();

    test_produce_no_value_errors();
    test_produce_single_value_error();
    test_produce_multiple_value_errors();
    test_produce_string_replacement();

    test_log_reader_file();
    test_log_reader_stdin();

    test_environment_create_directory();
    test_environment_copy_source();
    test_environment_run_command();

    test_try_alternatives_basic_execute();
    test_try_alternatives_replace_line();
    test_try_alternatives_replace_range();
    test_try_alternatives_multiple_alternatives();
    test_try_alternatives_multiple_runs();

    // New tests
    test_full_pipeline_simple_case();
    test_sample_cpp_log_analysis();
    test_sample_python_log_analysis();
    test_sample_java_log_analysis();

    std::cout << "All tests passed.\n";
    return 0;
}
