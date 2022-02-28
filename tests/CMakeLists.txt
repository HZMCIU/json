project(json_test)
cmake_minimum_required(VERSION 3.0)
add_library(unity STATIC unity/src/unity.c)
add_library(json_lib  ../json.c)

set(unity_tests
    parse_literals
    parse_numbers
    parse_strings
    parse_arrays
    parse_objects
    operate_utility)

foreach(unity_test ${unity_tests})
   add_executable("${unity_test}" "${unity_test}.c") 
   target_link_libraries("${unity_test}" json_lib unity)
   target_include_directories("${unity_test}" PUBLIC ../ unity/src/)
endforeach()

#add_executable(test_parse_literals test_parse_literals.c)
#
#target_link_libraries(test_parse_literals json_lib unity)
#
#target_include_directories(test_parse_literals PUBLIC ../ unity/src/)
target_include_directories(json_lib PUBLIC ../)

enable_testing()

#add_test(NAME test_parse_literals COMMAND test_parse_literals)
#add_test(NAME test_parse_numbers COMMAND test_parse_numbers)


foreach(unity_test ${unity_tests})
    add_test(NAME "${unity_test}" COMMAND "./${unity_test}")
endforeach()

