cmake_minimum_required(VERSION 4.1)
project(CompilerDesignLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# As you start new experiments, add them here:
add_subdirectory(exp01_lexer_llvm)
# add_subdirectory(exp02_handwritten_parser)