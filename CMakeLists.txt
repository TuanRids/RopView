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

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi") 
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG") 

project (RopView VERSION 1.0)

# Use the vcpkg.json to install the required packages
find_package(Boost REQUIRED COMPONENTS filesystem system)
find_package(ZeroMQ CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(hdf5 CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6Charts REQUIRED)
enable_language(CUDA)
find_package(CUDAToolkit REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(unofficial-qwt CONFIG REQUIRED)
find_package(VTK REQUIRED COMPONENTS 
  RenderingVolumeOpenGL2 
  RenderingVolume 
  RenderingCore 
  InteractionStyle 
  RenderingContextOpenGL2 
  RenderingOpenGL2)


pkg_check_modules(libusb REQUIRED IMPORTED_TARGET libusb-1.0)
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_ARCHITECTURES 75) # adjust based on our architecture
# Add source to this project's executable.
file(GLOB_RECURSE SOURCES
    "Source/**/*.cpp"
    "Source/**/*.c"
    "Source/**/*.cc"
    "Source/**/*.hpp"
    "Source/**/*.cu"
    "Source/**/*.qrc"
)
#set(CMAKE_AUTOMOC ON)

# Add the header files that contain Q_OBJECT
set(HEADER_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/event/ZoomableGraphicsView.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MainUI/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Model3D/MainViewport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Frames/SviewFrame.h

)
# Use qt6_wrap_cpp to process the headers with Q_OBJECT
qt6_wrap_cpp(MOC_SOURCES ${HEADER_FILES})
qt_add_resources(RopView_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shader/shaders.qrc)


#if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2")
#endif()
#set(CMAKE_CXX_STANDARD 20) 


# Combine all sources and moc-generated sources
add_executable (RopView RopView.cpp pch.h ${SOURCES} ${MOC_SOURCES} ${RopView_SOURCES} Source/Validate/fileformat.rc  "Source/MainUI/FacObsFrames/ObserverMgr.cpp")


# Depending on the compiler (MSVC or GCC), add specific flags or settings
if (MSVC)
    set(CMAKE_ASM_MASM_SOURCE_FILE_EXTENSIONS asm)
    enable_language(ASM_MASM)
else()
    enable_language(ASM)
endif()


# Add precompiled header
target_precompile_headers(RopView PRIVATE "pch.h")

# Add include directories for headers

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_include_directories(RopView PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

#link libraries
target_link_libraries(RopView PRIVATE ${OpenCV_LIBS})
target_link_libraries(RopView PRIVATE opencv_ml opencv_dnn opencv_core opencv_flann)
target_link_libraries(RopView PRIVATE Qt6::Widgets Qt6::OpenGL)
target_link_libraries(RopView PRIVATE Qt6::Charts)
target_link_libraries(RopView PRIVATE Vulkan::Vulkan)
target_link_libraries(RopView PRIVATE glm::glm)
target_link_libraries(RopView PRIVATE PkgConfig::libusb)
target_link_libraries(RopView PRIVATE ${VTK_LIBRARIES})
target_link_libraries(RopView PRIVATE CUDA::cudart)
target_link_libraries(RopView PRIVATE spdlog::spdlog)
target_link_libraries(RopView PRIVATE unofficial::qwt::qwt)


#target_link_libraries(RopView CUDA::cudart)

set(QT_PLUGIN_PATH "C:/vcpkg/vcpkg/installed/x64-windows/Qt6/plugins/platforms")
# Add library directories for linking
if (MSVC)
    # Enable MASM (Microsoft Macro Assembler) in MSVC
    set(CMAKE_ASM_MASM_SOURCE_FILE_EXTENSIONS asm)
    enable_language(ASM_MASM)
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Lib/Release")
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Lib/Debug")
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET RopView PROPERTY CXX_STANDARD 20)
endif()

# Link libraries
target_link_libraries(RopView PRIVATE Boost::filesystem Boost::system OlympusNDT.Instrumentation.lib)
target_link_libraries(RopView PRIVATE hdf5::hdf5-shared hdf5::hdf5_hl-shared hdf5::hdf5_tools-shared)

include_directories(${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

# Custom command for copying files after build
add_custom_command(TARGET RopView POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config.ini" "$<TARGET_FILE_DIR:RopView>"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/sample" "$<TARGET_FILE_DIR:RopView>/sample"    
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/QssTemplate" "$<TARGET_FILE_DIR:RopView>/QssTemplate"    
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/seting_lib.bat" "$<TARGET_FILE_DIR:RopView>"
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR} cmd /c seting_lib.bat
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Lib/x64-release DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/out/build)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Lib/x64-debug DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/out/build)
