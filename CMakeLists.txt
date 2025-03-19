cmake_minimum_required(VERSION 3.15)
project(StreetFighter)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set library dependency versions
set(RAYLIB_VERSION 5.5)
set(IMGUI_VERSION v1.91.9b)
set(RLIMGUI_VERSION main)

# External directory for libraries
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

# Function to download and extract a dependency
function(download_and_extract NAME URL DIR FOLDER_NAME)
    set(ARCHIVE "${DIR}.zip")
    
    if (NOT EXISTS ${DIR})
        message(STATUS "Downloading ${NAME} from ${URL}")
        file(DOWNLOAD ${URL} ${ARCHIVE} SHOW_PROGRESS)
        
        # Extract the archive
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${ARCHIVE} WORKING_DIRECTORY ${EXTERNAL_DIR})
        
        # Find any folder starting with name ${FOLDER_NAME}
        file(GLOB LIB_FOLDER "${EXTERNAL_DIR}/${FOLDER_NAME}*")

        # Get the first match (if multiple, take the first one found)
        list(GET LIB_FOLDER 0 LIB_EXTRACT_DIR)

        # Ensure we found a valid directory
        if (NOT EXISTS ${LIB_EXTRACT_DIR})
            message(FATAL_ERROR "${NAME} extracted directory not found in ${EXTERNAL_DIR}")
        else()
            # Rename extracted folder (remove version suffix)
            execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${LIB_EXTRACT_DIR} ${EXTERNAL_DIR}/${FOLDER_NAME})
        endif()

        file(REMOVE ${ARCHIVE})
    endif()
endfunction()

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

# Set external libraries' paths
set(RAYLIB_DIR ${EXTERNAL_DIR}/raylib)
set(IMGUI_DIR ${EXTERNAL_DIR}/imgui)
set(RLIMGUI_DIR ${EXTERNAL_DIR}/rlImGui)

# Download and extract dependencies
download_and_extract("Raylib" ${RAYLIB_ZIP_URL} ${RAYLIB_DIR} "raylib")
download_and_extract("Dear ImGui" "https://github.com/ocornut/imgui/archive/refs/tags/${IMGUI_VERSION}.zip" ${IMGUI_DIR} "imgui")
download_and_extract("rlImGui" "https://github.com/raylib-extras/rlImGui/archive/refs/heads/${RLIMGUI_VERSION}.zip" ${RLIMGUI_DIR} "rlImGui")

# Include libraries' directories
include_directories(${RAYLIB_DIR}/include ${IMGUI_DIR} ${RLIMGUI_DIR})

# Automatically find all source files in the 'src' directory
file(GLOB_RECURSE SOURCES src/*.cpp)

# Include ImGui and rlImGui source files
file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp)
file(GLOB RLIMGUI_SOURCES
    ${RLIMGUI_DIR}/rlImGui.cpp)

# Define the executable
add_executable(StreetFighter ${SOURCES} ${IMGUI_SOURCES} ${RLIMGUI_SOURCES})

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

# Copy resources folder to the build directory
add_custom_target(CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/resources
)
