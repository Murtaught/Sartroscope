project(Sartroscope)
cmake_minimum_required(VERSION 2.8)

find_package( OpenCV REQUIRED )

add_executable(Sartroscope main.cpp)
target_link_libraries( Sartroscope ${OpenCV_LIBS} )

add_custom_command(TARGET Sartroscope
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   "${PROJECT_SOURCE_DIR}/haarcascade_eye.xml"
                   ${PROJECT_BINARY_DIR})

