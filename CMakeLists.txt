cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(
    vulkan-voxel
    DESCRIPTION "vulkan voxel engine project; C++ with GLFW, GLM, STB, TINYOBJLOADER and ImGUI"
    HOMEPAGE_URL https://github.com/eharquin/vulkan-voxel
    LANGUAGES CXX)

#===========================================================================================
# OPTIONAL LIBRARiES
#
# The following libraries can be pulled down and added to the project by turnning the
# desired options on.
set(ENABLE_GLM   ON CACHE BOOL "Add GLM to the project" FORCE)
set(ENABLE_STB   ON CACHE BOOL "Add STB to the project" FORCE)
set(ENABLE_TINYOBJLOADER   ON CACHE BOOL "Add TINYOBJLOADER to the project" FORCE)
set(ENABLE_IMGUI   ON CACHE BOOL "Add imGUI to the project" FORCE)
#===========================================================================================


# Set directory paths
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(ASSET_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(ASSET_BINARY_DIR ${CMAKE_BINARY_DIR}/assets)
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_BINARY_DIR ${CMAKE_BINARY_DIR}/shaders)


file(GLOB_RECURSE SRC ${SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SRC})

include_directories(${INCLUDE_DIR})

# Perform dependency linkage
include(${CMAKE_DIR}/LinkGLFW.cmake)
LinkGLFW(${PROJECT_NAME} PRIVATE)

# Find and link Vulkan 
find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
else()
    message(FATAL_ERROR "Vulkan was not found on the system")
endif()


# ----- Copy assets ----------------------------------
add_custom_command(
    COMMENT "Copy assets to build dir"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_SOURCE_DIR} ${ASSET_BINARY_DIR}
    OUTPUT  ${ASSET_BINARY_DIR}
    DEPENDS ${ASSET_SOURCE_DIR}
)

add_custom_target(
    copy_assets ALL
    DEPENDS ${ASSET_BINARY_DIR}
)

add_dependencies(${PROJECT_NAME} copy_assets)
# ----------------------------------------------------


# ----- Compile shaders ------------------------------
file(GLOB SHADERS
  ${SHADER_SOURCE_DIR}/*.vert
  ${SHADER_SOURCE_DIR}/*.frag
  ${SHADER_SOURCE_DIR}/*.comp
  ${SHADER_SOURCE_DIR}/*.geom
  ${SHADER_SOURCE_DIR}/*.tesc
  ${SHADER_SOURCE_DIR}/*.tese
  ${SHADER_SOURCE_DIR}/*.mesh
  ${SHADER_SOURCE_DIR}/*.task
  ${SHADER_SOURCE_DIR}/*.rgen
  ${SHADER_SOURCE_DIR}/*.rchit
  ${SHADER_SOURCE_DIR}/*.rmiss)

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(
      COMMENT "Compiling shader ${FILENAME}.spv"
      OUTPUT  ${SHADER_BINARY_DIR}/${FILENAME}.spv
      COMMAND ${Vulkan_GLSLC_EXECUTABLE} -o ${SHADER_BINARY_DIR}/${FILENAME}.spv ${SHADER}
      DEPENDS ${SHADER}
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
endforeach()

add_custom_target(
    compile_shaders ALL
    DEPENDS ${SPV_SHADERS}
)

add_dependencies(${PROJECT_NAME} compile_shaders)
# ----------------------------------------------------



# ----- Copy shaders ---------------------------------
add_custom_command(
    COMMENT "Copy shaders to build dir"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_SOURCE_DIR} ${SHADER_BINARY_DIR}
    OUTPUT  ${SHADER_BINARY_DIR}
    DEPENDS ${SHADER_SOURCE_DIR}
)

add_custom_target(
    copy_shaders ALL
    DEPENDS ${SHADER_BINARY_DIR}
)

add_dependencies(${PROJECT_NAME} copy_shaders)
# ----------------------------------------------------


# Handle the optional libraries
if (${ENABLE_GLM})
    include(${CMAKE_DIR}/LinkGLM.cmake)
    LinkGLM(${PROJECT_NAME} PRIVATE)
endif()

if (${ENABLE_STB})
    include(${CMAKE_DIR}/LinkSTB.cmake)
    LinkSTB(${PROJECT_NAME} PRIVATE)
endif()

if(${ENABLE_TINYOBJLOADER})
    include(${CMAKE_DIR}/LinkTINYOBJLOADER.cmake)
    LinkTINYOBJLOADER(${PROJECT_NAME} PRIVATE)
endif()

if(${ENABLE_IMGUI})
    include(${CMAKE_DIR}/LinkImGUI.cmake)
    LinkImGUI(${PROJECT_NAME} PRIVATE)
endif()


# Enable C++17
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Set project folders
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${PROJECT_NAME})
