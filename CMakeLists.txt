cmake_minimum_required(VERSION 3.30)
project(bcs323_mini_project C)

set(CMAKE_C_STANDARD 11)

add_executable(bcs323_mini_project master.c
        worker.c
        search.c
        ipc_utils.c)
