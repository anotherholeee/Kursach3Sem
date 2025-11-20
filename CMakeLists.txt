cmake_minimum_required(VERSION 4.0)
project(kursach)

set(CMAKE_CXX_STANDARD 20)

add_executable(kursach main.cpp
        TransportException.cpp
        TransportException.h
        Time.cpp
        Time.h
        Route.cpp
        Route.h
        Trip.cpp
        Trip.h
        TransportSystem.cpp
        TransportSystem.h
        Menu.cpp
        Menu.h)
