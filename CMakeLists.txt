cmake_minimum_required(VERSION 2.8)
find_package(OpenGL REQUIRED)
project(mycluster)
add_subdirectory (external)

add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
)

include_directories(
        external/AntTweakBar-1.16/include/
        external/glfw-3.1.2/include/
        external/glew-1.13.0/include/
        external/assimp-3.0.1270/include/
        external/bullet-2.81-rev2613/src/
        .
)

set(ALL_LIBS
        ${OPENGL_LIBRARY}
        glfw
        GLEW_1130
)


add_executable(${PROJECT_NAME}
    main.cpp
    cubic.cpp
    cluster.cpp
    speedmeter.cpp
    sphere.cpp
    lib/GlHelper.cpp
    lib/Mat.cpp
    lib/vecmath.cpp
    SOIL/SOIL.c
    SOIL/stb_image_aug.c
    SOIL/image_helper.c
    SOIL/image_DXT.c
    common/shader.cpp
    common/shader.hpp
    )
target_link_libraries(${PROJECT_NAME}
        ${ALL_LIBS}
)

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )
