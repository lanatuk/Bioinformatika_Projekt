add_executable(graphContigs main.cpp)

add_library(Node Node.hpp Node.cpp)

target_link_libraries(graphContigs Node)