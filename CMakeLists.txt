cmake_minimum_required (VERSION 3.16)

if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message (FATAL_ERROR "In-source builds are not allowed")
endif ()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE STRING "Modules for CMake" FORCE)

# ---------------------------------------------------------
#  Project info
# ---------------------------------------------------------
project (
  data
  VERSION 0.0.34
  DESCRIPTION "functinal data structures, mathematics, cryptography, networking, wrappers, and other high-level programming constructs in c++"
  LANGUAGES CXX
)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------
#  Build type setup (default to Debug for single-config)
# ---------------------------------------------------------
if (NOT CMAKE_CONFIGURATION_TYPES)   # True for Make/Ninja
    if (NOT CMAKE_BUILD_TYPE)
        set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the build type." FORCE)
    endif ()
endif ()

# List of supported build types
set (_supported_build_types
    "Debug"
    "Release"
    "RelWithDebInfo"
    "MinSizeRel"
)

set (_build_type_descriptions
    "Debug          - For development. Enables debug symbols, sanitizers, and minimal optimization."
    "Release        - For distribution. Full optimization, no debug symbols, no assertions."
    "RelWithDebInfo - Optimized but retains debug info. Good for profiling and crash investigation."
    "MinSizeRel     - Focuses on shrinking the binary. Useful for constrained environments."
)

# ---------------------------------------------------------
#  User message explaining usage
# ---------------------------------------------------------
message (STATUS "")
message (STATUS "Build type selected: ${CMAKE_BUILD_TYPE}")
message (STATUS "To choose a different build type, configure with:")
message (STATUS "    cmake -DCMAKE_BUILD_TYPE=<type> ..")
message (STATUS "Available build types:")
foreach (line IN LISTS _build_type_descriptions)
    message (STATUS "  • ${line}")
endforeach ()
message (STATUS "")

# ---------------------------------------------------------
#  Compiler flags
# ---------------------------------------------------------

# Base flags for each type (C and C++)
set (CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE      "-O3 -DNDEBUG")

set (CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")

set (CMAKE_C_FLAGS_DEBUG        "-O1 -g3 -fno-omit-frame-pointer")
set (CMAKE_CXX_FLAGS_DEBUG      "-O1 -g3 -fno-omit-frame-pointer")

# ---------------------------------------------------------
#  Sanitizers (enabled only in Debug)
# ---------------------------------------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message (STATUS "Enabling sanitizers for Debug build.")

    set (_san_flags "-fsanitize=address,undefined")

    add_compile_options (${_san_flags})
    add_link_options (${_san_flags})
endif ()

# ---------------------------------------------------------
# Concepts diagnostics depth (GCC/Clang only)
# ---------------------------------------------------------
include (CheckCXXCompilerFlag)

check_cxx_compiler_flag ("-fconcepts-diagnostics-depth=20" HAS_CONCEPTS_DEPTH)

if (HAS_CONCEPTS_DEPTH)
    add_compile_options ("-fconcepts-diagnostics-depth=20")
endif ()

# ---------------------------------------------------------
# Dependencies
# ---------------------------------------------------------

find_package (Boost 1.86 COMPONENTS thread chrono log_setup log REQUIRED)
# if we do not have boost
#   * linux:
#     * go to the boost website, download and extract the latest version.
#     * `./bootstrap.sh`
#     * `./b2`
#     * `sudo ./b2 install`

add_definitions ("-DHAS_BOOST")

find_package (PkgConfig REQUIRED)

IF (WIN32)
  pkg_check_modules (cryptopp REQUIRED IMPORTED_TARGET cryptopp)
ELSE ()
  pkg_check_modules (cryptopp REQUIRED IMPORTED_TARGET libcrypto++)
  # If we do not have crypto++, go to `https://github.com/weidai11/cryptopp` and follow the instructions.
ENDIF ()

include (FetchContent)
cmake_policy (SET CMP0135 NEW)

FetchContent_Declare (json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable (json)

FetchContent_Declare (ctre URL https://github.com/hanickadot/compile-time-regular-expressions/archive/refs/tags/v3.9.0.zip)
FetchContent_MakeAvailable (ctre)

find_package (data CONFIG REQUIRED)

find_package (gigamonkey CONFIG REQUIRED)

add_executable (SecretShare secret_share.cpp)

target_link_libraries (SecretShare PUBLIC Gigamonkey::base58_check Data::io Data::crypto nlohmann_json)
target_include_directories (SecretShare PUBLIC include)

target_compile_features (SecretShare PUBLIC cxx_std_23)
set_target_properties (SecretShare PROPERTIES CXX_EXTENSIONS OFF)

install (DIRECTORY include/ DESTINATION include)
install (TARGETS SecretShare)
