
cmake_minimum_required (VERSION 3.16)
project (SecretShare CXX)

find_package (Gigamonkey CONFIG REQUIRED)

add_executable (SecretShare secret_share.cpp)

target_link_libraries (SecretShare PUBLIC gigamonkey::gigamonkey)
target_include_directories (SecretShare PUBLIC include)

target_compile_features (SecretShare PUBLIC cxx_std_20)
set_target_properties (SecretShare PROPERTIES CXX_EXTENSIONS OFF)

install (DIRECTORY include/ DESTINATION include)
install (TARGETS SecretShare)

