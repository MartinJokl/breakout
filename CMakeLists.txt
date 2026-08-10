cmake_minimum_required(VERSION 4.4.0)
project(breakout)

add_executable(${PROJECT_NAME} 
    main.c
    game.h
    game.c
    shader.h
    shader.c
    texture.h
    texture.c
    vector.h
    vector.c
    matrix.c
    matrix.h
    spriteRenderer.h
    spriteRenderer.c
    file.h
    file.c
    gameObject.h
    gameObject.c
    gameLevel.h
    gameLevel.c
    list.h
    list.c
    textureManager.h
    textureManager.c
    ballObject.c
    ballObject.h
    postProcessor.h
    postProcessor.c)

# copy shader and asset files
file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS 
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.*" 
     "${CMAKE_CURRENT_SOURCE_DIR}/assets/*.*"
)
set(COPY_OUTPUTS "")
foreach(SRC_FILE ${ASSET_FILES})
    file(RELATIVE_PATH REL_FILE "${CMAKE_CURRENT_SOURCE_DIR}" "${SRC_FILE}")
    set(DST_FILE "${CMAKE_CURRENT_BINARY_DIR}/${REL_FILE}")

    add_custom_command(
        OUTPUT "${DST_FILE}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SRC_FILE}" "${DST_FILE}"
        DEPENDS "${SRC_FILE}"
        COMMENT "Copying ${REL_FILE}"
    )
    list(APPEND COPY_OUTPUTS "${DST_FILE}")
endforeach()
add_custom_target(copy_assets DEPENDS ${COPY_OUTPUTS})
add_dependencies(${PROJECT_NAME} copy_assets)

# include
include_directories(include)

# GLAD
add_library(glad SHARED ${CMAKE_SOURCE_DIR}/lib/glad.c)
target_link_libraries(${PROJECT_NAME} glad)

# STB
add_library(stb SHARED ${CMAKE_SOURCE_DIR}/lib/stb.c)
target_link_libraries(${PROJECT_NAME} stb)

# GLFW
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libglfw3.a)

# freetype
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libfreetype.a)