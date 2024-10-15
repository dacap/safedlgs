# Copyright (c) 2024 David Capello

cmake_minimum_required(VERSION 3.23)
project(safedlgs)

add_executable(dlgs dlgs.c)
target_compile_options(dlgs PUBLIC -DUNICODE -D_UNICODE -W1 -WX)
target_link_options(dlgs PUBLIC -SUBSYSTEM:CONSOLE
  $<$<CONFIG:Release>:-NODEFAULTLIB -ENTRY:wmain>)
target_link_libraries(dlgs PRIVATE shlwapi)

add_executable(test test.c)
target_compile_options(test PUBLIC -DUNICODE -D_UNICODE -W1 -WX)
target_link_options(test PUBLIC -SUBSYSTEM:WINDOWS
  $<$<CONFIG:Release>:-NODEFAULTLIB -ENTRY:wWinMain>)
target_link_libraries(test PRIVATE shlwapi)
add_dependencies(test dlgs)
