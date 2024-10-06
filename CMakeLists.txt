cmake_minimum_required(VERSION 3.26)
project(SPO_Lab1 C)

set(CMAKE_C_STANDARD 11)

add_executable(SPO_Lab1 main.c
        tree.c
        tree.h
        error.c
        error.h
        lex.yy.c
        main.h
        parser.tab.c
        parser.tab.h
        parser.c
        parser.h
)
