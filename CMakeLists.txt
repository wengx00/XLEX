cmake_minimum_required(VERSION 3.5)
project(XLEX VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g")

# add_subdirectory()

# add_library()

add_executable(${PROJECT_NAME}
    nfa.hpp
    dfa.hpp
    mdfa.hpp
    main.cpp
)

include_directories(${PROJECT_SOURCE_DIR})

# link_directories()

# target_link_directories()