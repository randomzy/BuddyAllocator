add_executable(generic_tests generic_tests.cpp)
target_include_directories(generic_tests PUBLIC ../include)
add_test(gen_tests generic_tests)