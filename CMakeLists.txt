cmake_minimum_required(VERSION 3.10)

# Change the line below to change the name of the project
project(v5_easyUma)

# Required libraries are below
find_package(Boost REQUIRED COMPONENTS system jsoncpp multiprecision)
find_package(CURL REQUIRED)

add_executable(v4chat v4_chat.cpp)
target_link_libraries(v5 Boost::boost Boost::system Boost::multiprecision CURL::libcurl)

