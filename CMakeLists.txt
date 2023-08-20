# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the project name
project (game_engine)

# Create a sources variable with a link to all cpp files to compile
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")

# Add an executable with the above sources
add_executable(game_engine ${SOURCES} ${HEADERS})

add_subdirectory(dependencies/glfw)

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
find_package(Vulkan)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
link_directories(${VULKAN_PATH}/Bin;${VULKAN_PATH}/Lib;)

target_link_libraries(game_engine glfw vulkan-1)

target_include_directories(game_engine
    PUBLIC 
        ${GLFW_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}
        ${VULKAN_INCLUDE_DIRS}
)
IF (NOT Vulkan_FOUND)
    MESSAGE("ERROR")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

