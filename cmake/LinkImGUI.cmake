include(FetchContent)

macro(LinkImGUI TARGET ACCESS)
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.89.9
    )

    FetchContent_GetProperties(imgui)

    if (NOT imgui_POPULATED)
        FetchContent_Populate(imgui)
    endif()

    message(STATUS ${imgui_SOURCE_DIR})

    file(GLOB imgui_SOURCE
        ${imgui_SOURCE_DIR}/*.cpp
        ${imgui_SOURCE_DIR}/*.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.*
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.*)

    message(STATUS ${imgui_SOURCE})

    target_include_directories(${TARGET} ${ACCESS} ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    target_sources(${TARGET} ${ACCESS} ${imgui_SOURCE})
endmacro()