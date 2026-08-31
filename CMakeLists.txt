cmake_minimum_required(VERSION 3.15)

project(ImgProc LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

set(FLTK_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/fltk)


file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
add_executable(ImgProc ${SRC_FILES})


target_include_directories(ImgProc PRIVATE
    third_party/stb
    third_party/other
)

target_link_libraries(ImgProc PRIVATE
    fltk::fltk
    OpenMP::OpenMP_CXX
)
