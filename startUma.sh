#!/bin/bash

# Helper script to generate C++ executable
rm -rf easyUma
g++ -o easyUma easyUma.cpp -I/usr/include/jsoncpp -lcurl -ljsoncpp
