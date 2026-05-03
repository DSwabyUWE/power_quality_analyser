cmake_minimum_required(VERSION 3.15)
project(power_quality_analyser C)

set(CMAKE_C_STANDARD 99)

add_executable(power_quality_analyser
        main.c
        waveform.c
        io.c
        )

target_link_libraries(power_quality_analyser m)
