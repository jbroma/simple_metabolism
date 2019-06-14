cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME sm)

project(${PROJECT_NAME})

#set(CMAKE_C_COMPILER "gcc-8")
#set(CMAKE_CXX_COMPILER "g++-8")

set(CMAKE_CXX_FLAGS "-O3 -Wall -Werror -Wno-missing-braces -pedantic")
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

find_package(Curses REQUIRED)

file(GLOB_RECURSE HEADER "include/*.h" "include/*.hpp")
file(GLOB_RECURSE SOURCE "static/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME}
                            PRIVATE 
                            ${CURSES_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} 
                        PRIVATE
                        ${CURSES_LIBRARIES}
                        pthread)
