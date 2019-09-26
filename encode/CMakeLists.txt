file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/assets)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/run.bat)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/run.bat DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories(../src)

add_executable(encode main.cpp)
target_link_libraries(encode json)