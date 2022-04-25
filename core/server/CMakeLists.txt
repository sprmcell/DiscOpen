set(CMAKE_CXX_FLAGS "-march=x86-64 -O2 -pipe -lpthread")
set(CMAKE_C_FLAGS "-march=x86-64 -O2 -pipe -lpthread")


file(GLOB SRC "src/*.cpp")

add_executable(discopen-server ${SRC})
# target_include_directories(opendisc-server PRIVATE ../common/include)
target_link_libraries(discopen-server PRIVATE common)