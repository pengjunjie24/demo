cmake_minimum_required(VERSION 2.8.12)

project(thirdparty C CXX)

#程序编译方式:DEBUG OR RELEASE
option(RELEASE "program compile release module" OFF)
if(RELEASE)
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "USE RELEASE MODULE")
else()
    set(CMAKE_BUILD_TYPE "Debug")
    message(STATUS "USE DEBUG MODULE")
endif()

#设置C++编译选项
set(CXX_FLAGS
    -g
    -DCHECK_PTHREAD_RETURN_VALUE
    -D_FILE_OFFSET_BITS=64
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wno-unused-parameter
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    -Wshadow
    -Wwrite-strings
    -march=native
    -rdynamic
)

#sanitizer
option(USE_SANITIZER "This is a option for sanitizer" OFF)
if(USE_SANITIZER)
    set(CXX_FLAGS
    -fsanitize=address
    -fno-omit-frame-pointer
    -fsanitize-recover=address
    )

    message(STATUS "use address sanitizer")
endif()

#设置编译器
set(CMAKE_CXX_COMPILER "g++")
add_compile_options(-std=c++14)
#设置Debug模式选项
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g2 -ggdb")
#设置Release模式选项
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -finline-limit=1000 -DNDEBUG")
#可执行文件存放路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#库文件存放路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(
    ${PROJECT_SOURCE_DIR}
)

#获取子目录名称的宏
macro(SUBDIRLIST result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

add_subdirectory(fsmcpp)