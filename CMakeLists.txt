cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Include the VCPKG toolchain file
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "c:/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    message(STATUS "Using vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif()


project (RopView VERSION 1.0)

# Use the vcpkg.json to install the required packages
find_package(Boost REQUIRED COMPONENTS filesystem system)
find_package(ZeroMQ CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Add source to this project's executable.
file(GLOB SOURCES "Source/*.cpp" "Source/*.h" "Source/*.hpp" )

message("Sources: ${SOURCES}")

add_executable (RopView "RopView.cpp" "pch.h" ${SOURCES} "Source/Omconnect/OmConnect.h" "Source/Omconnect/OmConnect.cpp" "Source/OmConnect/loadjson.cpp")
target_precompile_headers(RopView PRIVATE "pch.h")

# Add include directories for headers
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")

# Add library directories for linking
if (MSVC)
    # For Release build
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Lib/Release")
    # For Debug build
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Lib/Debug")
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RopView PROPERTY CXX_ STANDARD 20)
endif()

# Link Boost libraries
target_link_libraries(RopView PRIVATE Boost::filesystem Boost::system OlympusNDT.Instrumentation.lib)

include_directories(${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
# lib & dll OMNICSAN 
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Lib/x64-release DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/out/build)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/x64-release DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/out/build)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Lib/x64-debug DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/out/build)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/x64-debug DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/out/build)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Config.json DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-debug/settings)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Config.json DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-release/settings)

