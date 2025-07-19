cmake_minimum_required(VERSION 3.12.4)
project(wf24 C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile_commands.json generation for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the output path for the executable
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Find and include SDL2 library from vendor directory
set(SDL2_PATH "${PROJECT_SOURCE_DIR}/vendor/sdl2")

# Add SDL2 as a library
add_library(SDL2 SHARED IMPORTED)
set_target_properties(SDL2 PROPERTIES
    IMPORTED_LOCATION "${SDL2_PATH}/SDL2.dll"
    IMPORTED_IMPLIB "${SDL2_PATH}/lib/libSDL2.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_PATH}/include"
)

add_library(SDL2main STATIC IMPORTED)
set_target_properties(SDL2main PROPERTIES
    IMPORTED_LOCATION "${SDL2_PATH}/lib/libSDL2main.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_PATH}/include"
)

# Add LVGL subdirectory from vendor directory
add_subdirectory(vendor/lvgl)

# Configure LVGL with SDL2 include directories
target_include_directories(lvgl PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${SDL2_PATH}/include
)

# Create the main executable
add_executable(main
    ${PROJECT_SOURCE_DIR}/src/main.c
    ${PROJECT_SOURCE_DIR}/src/ui.c
    ${PROJECT_SOURCE_DIR}/src/saturn_v.c
)

# Set up include directories for the main target
target_include_directories(main PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}
    ${SDL2_PATH}/include
)

# Define LVGL configuration as a simple include
target_compile_definitions(main PRIVATE LV_CONF_INCLUDE_SIMPLE)

# Link libraries
target_link_libraries(main lvgl lvgl::examples lvgl::demos lvgl::thorvg SDL2main SDL2)

# On Windows, SDL2 applications need to be built as GUI applications
if (WIN32)
    # For MinGW, we need to link with the correct subsystem
    # target_link_options(main PRIVATE "-mwindows")
    # Add console for debugging
    target_compile_definitions(main PRIVATE SDL_MAIN_HANDLED)
endif()

# Custom target to run the executable
add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)

# Copy SDL2.dll to output directory
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_PATH}/SDL2.dll"
    "${EXECUTABLE_OUTPUT_PATH}/SDL2.dll"
)

# Create a symlink to compile_commands.json in the project root for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_command(
        TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${PROJECT_SOURCE_DIR}/compile_commands.json
    )
endif() 
