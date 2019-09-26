file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/encoded.json)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/encoded.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


include_directories(../json.h)
include_directories(../src)

add_executable(decode main.cpp)
target_link_libraries(decode json)