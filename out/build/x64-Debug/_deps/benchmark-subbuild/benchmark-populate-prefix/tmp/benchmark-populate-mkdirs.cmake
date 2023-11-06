# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Algoritms/template-cpp/template-cpp/out/build/x64-Debug/_deps/benchmark-src"
  "D:/Algoritms/template-cpp/template-cpp/out/build/x64-Debug/_deps/benchmark-build"
  "D:/Algoritms/template-cpp/template-cpp/out/build/x64-Debug/_deps/benchmark-subbuild/benchmark-populate-prefix"
  "D:/Algoritms/template-cpp/template-cpp/out/build/x64-Debug/_deps/benchmark-subbuild/benchmark-populate-prefix/tmp"
  "D:/Algoritms/template-cpp/template-cpp/out/build/x64-Debug/_deps/benchmark-subbuild/benchmark-populate-prefix/src/benchmark-populate-stamp"
  "D:/Algoritms/template-cpp/template-cpp/out/build/x64-Debug/_deps/benchmark-subbuild/benchmark-populate-prefix/src"
  "D:/Algoritms/template-cpp/template-cpp/out/build/x64-Debug/_deps/benchmark-subbuild/benchmark-populate-prefix/src/benchmark-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Algoritms/template-cpp/template-cpp/out/build/x64-Debug/_deps/benchmark-subbuild/benchmark-populate-prefix/src/benchmark-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Algoritms/template-cpp/template-cpp/out/build/x64-Debug/_deps/benchmark-subbuild/benchmark-populate-prefix/src/benchmark-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
