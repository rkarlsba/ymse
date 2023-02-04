# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/roy/src/git/espressif/esp-idf/components/bootloader/subproject"
  "/Users/roy/src/git/rkarlsba/ymse/ardugreier/espen/esp32/litentest/build/bootloader"
  "/Users/roy/src/git/rkarlsba/ymse/ardugreier/espen/esp32/litentest/build/bootloader-prefix"
  "/Users/roy/src/git/rkarlsba/ymse/ardugreier/espen/esp32/litentest/build/bootloader-prefix/tmp"
  "/Users/roy/src/git/rkarlsba/ymse/ardugreier/espen/esp32/litentest/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/roy/src/git/rkarlsba/ymse/ardugreier/espen/esp32/litentest/build/bootloader-prefix/src"
  "/Users/roy/src/git/rkarlsba/ymse/ardugreier/espen/esp32/litentest/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/roy/src/git/rkarlsba/ymse/ardugreier/espen/esp32/litentest/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/roy/src/git/rkarlsba/ymse/ardugreier/espen/esp32/litentest/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
