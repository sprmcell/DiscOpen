set(CMAKE_CXX_FLAGS "-march=x86-64 -O2 -pipe -std=c++2a -g")
set(CMAKE_C_FLAGS "-march=x86-64 -O2 -pipe -g")

file(GLOB SRC "src/*.cpp")

add_executable(opendisc-client ${SRC})
# target_include_directories(opendisc-client PRIVATE ../common/include)
target_link_libraries(opendisc-client PRIVATE common)