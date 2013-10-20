project(Sartroscope)
cmake_minimum_required(VERSION 2.8)

find_package( OpenCV REQUIRED )

add_executable(Sartroscope main.cpp)
target_link_libraries( Sartroscope ${OpenCV_LIBS} )

