# Minimum required CMake version
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Project name
project("Logger")

# Create shared library
file(GLOB_RECURSE LOGGER_SRC_FILES
     ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/Logger/*.h
     ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/Logger/*.cpp)
add_library(Logger SHARED ${LOGGER_SRC_FILES})

# Add subprojects
if(NOT TARGET FileSystem)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../FileSystem" FileSystem)
endif()
#if(NOT TARGET Input)
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Input" Input)
#endif()
#if(NOT TARGET LuaEmbedder)
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../LuaEmbedder" LuaEmbedder)
#endif()
#if(NOT TARGET MPL)
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../MPL" MPL)
#endif()
#if(NOT TARGET Network)
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Network" Network)
#endif()
#if(NOT TARGET Renderer)
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Renderer" Renderer)
#endif()
# Link subproject shared libraries
target_link_libraries(Logger
					  FileSystem)

if(WIN32)
  # Include directories
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../../Externals/include/Windows"
		      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Source")
		      
  # Link external dynamic libraries
  target_link_libraries(Logger
			${EXTERNAL_WINDOWS_DYNAMIC_LIBRARIES})
elseif(UNIX)
  # Include directories
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../../Externals/include/Linux"
		      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Source")

  # Link external shared libraries
  target_link_libraries(Logger
			${EXTERNAL_UNIX_SHARED_LIBRARIES})
endif()
