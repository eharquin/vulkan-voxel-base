include(FetchContent)

macro(LinkTINYOBJLOADER TARGET ACCESS)
    FetchContent_Declare(
        tinyobjloader
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader
        GIT_TAG v2.0-rc1
    )

    FetchContent_GetProperties(tinyobjloader)

    if (NOT tinyobjloader_POPULATED)
        FetchContent_Populate(tinyobjloader)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${tinyobjloader_SOURCE_DIR})
endmacro()