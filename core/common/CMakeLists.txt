set(CMAKE_CXX_FLAGS "-march=x86-64 -O2 -pipe -std=c++2a")
set(CMAKE_C_FLAGS "-march=x86-64 -O2 -pipe")

file(GLOB SRC "src/*.cpp")

add_library(common STATIC ${SRC})
target_include_directories(common PUBLIC include/)
