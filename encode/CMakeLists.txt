set(TEST_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

file(REMOVE_RECURSE TEST_DEST_DIR)
file(COPY ${TEST_SRC_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories(../src)

add_executable(encode main.cpp)
target_link_libraries(encode json)