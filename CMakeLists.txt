cmake_minimum_required(VERSION 3.15)
project(StreetFighter)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set raylib version
set(RAYLIB_VERSION 5.5)

# Set raylib download URL based on OS and compiler
if (WIN32)
    if (MSVC)
        set(RAYLIB_ZIP_URL "https://github.com/raysan5/raylib/releases/download/${RAYLIB_VERSION}/raylib-${RAYLIB_VERSION}_win64_msvc16.zip")
    else()
        set(RAYLIB_ZIP_URL "https://github.com/raysan5/raylib/releases/download/${RAYLIB_VERSION}/raylib-${RAYLIB_VERSION}_win64_mingw-w64.zip")
    endif()
elseif (APPLE)
    set(RAYLIB_ZIP_URL "https://github.com/raysan5/raylib/releases/download/${RAYLIB_VERSION}/raylib-${RAYLIB_VERSION}_macos.tar.gz")
elseif (UNIX)
    set(RAYLIB_ZIP_URL "https://github.com/raysan5/raylib/releases/download/${RAYLIB_VERSION}/raylib-${RAYLIB_VERSION}_linux_amd64.tar.gz")
endif()

# Set paths
set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/external/raylib)
set(RAYLIB_ARCHIVE ${CMAKE_SOURCE_DIR}/external/raylib.zip)

# Download and extract raylib if not present
if (NOT EXISTS ${RAYLIB_DIR})
    message(STATUS "Downloading raylib from ${RAYLIB_ZIP_URL}")
    file(DOWNLOAD ${RAYLIB_ZIP_URL} ${RAYLIB_ARCHIVE} SHOW_PROGRESS)

    # Extract the archive
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${RAYLIB_ARCHIVE} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/external)

    # Find any folder named "raylib-*"
    file(GLOB RAYLIB_FOLDER ${CMAKE_SOURCE_DIR}/external/raylib-*)

    # Get the first match (if multiple, take the first one found)
    list(GET RAYLIB_FOLDER 0 RAYLIB_EXTRACT_DIR)

    # Ensure we found a valid directory
    if (NOT EXISTS ${RAYLIB_EXTRACT_DIR})
        message(FATAL_ERROR "raylib extracted directory not found in external/")
    endif()
    
    # Rename extracted folder (remove version suffix)
    execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${RAYLIB_EXTRACT_DIR} ${RAYLIB_DIR})

    # Clean up the archive
    file(REMOVE ${RAYLIB_ARCHIVE})
endif()

# Include raylib headers
include_directories(${RAYLIB_DIR}/include)

# Automatically find all source files in the 'src' directory
file(GLOB_RECURSE SOURCES src/*.cpp)

# Define the executable
add_executable(StreetFighter ${SOURCES})

# Detect OS and Compiler
if (WIN32)
    if (MSVC)
        set(RAYLIB_LIB "${CMAKE_SOURCE_DIR}/external/raylib/lib/libraylib.lib")
        set(EXTRA_LIBS winmm gdi32 opengl32)  # Windows-specific libraries
    
    else()
        set(RAYLIB_LIB "${CMAKE_SOURCE_DIR}/external/raylib/lib/libraylib.a")
        set(EXTRA_LIBS -lopengl32 -lgdi32 -lwinmm)  # Windows-specific libraries
    
    endif()

elseif (APPLE)
    set(RAYLIB_LIB "${CMAKE_SOURCE_DIR}/external/raylib/lib/libraylib.a")
    set(EXTRA_LIBS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")  # macOS frameworks

elseif (UNIX)
    set(RAYLIB_LIB "${CMAKE_SOURCE_DIR}/external/raylib/lib/libraylib.a")
    set(EXTRA_LIBS m pthread dl X11)  # Linux-specific libraries

endif()

# Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIB} ${EXTRA_LIBS})

# Compiler-specific settings
if (MSVC)
    message(STATUS "Using MSVC Compiler")
    target_compile_options(StreetFighter PRIVATE /W4)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "Using GCC Compiler")
    target_compile_options(StreetFighter PRIVATE -Wall -Wextra -Wpedantic)
endif()
