cmake_minimum_required(VERSION 3.20)
project(MathExpressionsSolver LANGUAGES CXX)

# Cmake configuration
add_compile_options("/utf-8")

# C++ standard settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Automatically add all .cpp
file(GLOB_RECURSE SRC_FILES
    "src/*.cpp"
    "src/core/*.cpp"
    "src/logic/*.cpp"
    "src/util/*.cpp"
)

# Add external fmt library source
set(FMT_PATH "${CMAKE_SOURCE_DIR}/external/fmtlib/fmt_12.0.0")
set(FMT_SRC "${FMT_PATH}/src/fmt.cpp")

# Add include directories
include_directories(
    "${CMAKE_SOURCE_DIR}/src"
    "${FMT_PATH}"
)

# Define executable target
add_executable(MathExpressionsSolver
    ${SRC_FILES}
    ${FMT_SRC}
)

# Output directory configuration
set_target_properties(MathExpressionsSolver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
