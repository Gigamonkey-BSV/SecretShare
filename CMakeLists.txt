cmake_minimum_required (VERSION 3.16)
project (SecretShare CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE STRING "Modules for CMake" FORCE)

include (FetchContent)
FetchContent_Declare (json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable (json)

FetchContent_Declare (argh URL https://github.com/adishavit/argh/archive/refs/tags/v1.3.2.zip)
FetchContent_MakeAvailable (argh)

FetchContent_Declare (ctre URL https://github.com/hanickadot/compile-time-regular-expressions/archive/refs/tags/v3.9.0.zip)
FetchContent_MakeAvailable (ctre)

find_package (PkgConfig REQUIRED)
pkg_check_modules(Cryptopp REQUIRED IMPORTED_TARGET libcrypto++)

find_package (gigamonkey CONFIG REQUIRED)

add_executable (SecretShare secret_share.cpp)

target_link_libraries (SecretShare PUBLIC nlohmann_json argh Gigamonkey::gigamonkey)
target_include_directories (SecretShare PUBLIC include)

target_compile_features (SecretShare PUBLIC cxx_std_20)
set_target_properties (SecretShare PROPERTIES CXX_EXTENSIONS OFF)

install (DIRECTORY include/ DESTINATION include)
install (TARGETS SecretShare)

