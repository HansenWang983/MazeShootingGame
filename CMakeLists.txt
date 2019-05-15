cmake_minimum_required (VERSION 3.8 FATAL_ERROR) 
project(MazeShootingGame CXX)

set(CMAKE_CXX_STANDARD 11)

# OS config
if (WIN32)
    option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
endif()
if(NOT WIN32)
    find_package(OpenGL REQUIRED)
endif()

# config root_directory
configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

# if compiling for visual studio, also use configure file for each project (specifically to set up working directory)
if(MSVC)
	configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user @ONLY)
endif(MSVC)

# config glfw
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory(3rd_party/glfw)
include_directories(3rd_party/glfw/include)
LIST(APPEND LIBS glfw ${GLFW_LIBRARIES})

# config glad
set(GLAD_SRC 3rd_party/glad/src/glad.c)
if (NOT WIN32)
    LIST(APPEND LIBS dl)
endif()
include_directories(3rd_party/glad/include)

# config glm
include_directories(3rd_party/glm)

# config assimp
add_subdirectory(3rd_party/assimp)
include_directories(3rd_party/assimp/include)
LIST(APPEND LIBS assimp ${ASSIMP_LIBRARY})

# include our headers
include_directories(include)

# get MazeShootingGame src files
aux_source_directory(./src MazeShootingGame_SRC)

set(SOURCE_FILES ${MazeShootingGame_SRC} ${GLAD_SRC})

# Compile
add_executable(MazeShootingGame ${SOURCE_FILES})

# Link
target_link_libraries(MazeShootingGame ${LIBS})