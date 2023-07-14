cmake_minimum_required(VERSION 3.26.4)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "-Wall -Wfatal-errors -Wshadow")

project(qoiEncoderDecoder)
file(GLOB_RECURSE src "*.cpp")
add_executable(qoi_encoder_decoder ${src})
install(TARGETS qoi_encoder_decoder DESTINATION ~/bin)