cmake_minimum_required(VERSION 3.14)
project(backend_notes)

set(CMAKE_CXX_STANDARD 17)

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(LIB_BIN_DIR ${THIRD_PARTY_DIR}/lib_bin)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(SDL2_INCLUDE_DIR "${LIB_BIN_DIR}/SDL2/include/SDL2")
set(SDL2_LIBS 
    "${LIB_BIN_DIR}/SDL2/lib/libSDL2main.a" 
    "${LIB_BIN_DIR}/SDL2/lib/libSDL2.dll.a"
)

set(ZMQ_INCLUDE_DIR "${LIB_BIN_DIR}/zeromq/include")
set(ZMQ_LIB "${LIB_BIN_DIR}/zeromq/lib/libzmq.dll.a")

find_package(OpenGL REQUIRED)
set(GLEW_INCLUDE_DIR "${LIB_BIN_DIR}/glew/include")
set(GLEW_LIB "${LIB_BIN_DIR}/glew/lib/glew32.lib")

add_library(imgui STATIC
    ${THIRD_PARTY_DIR}/imgui/imgui.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_sdl2.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC 
    ${THIRD_PARTY_DIR}/imgui 
    ${THIRD_PARTY_DIR}/imgui/backends
    "${LIB_BIN_DIR}/SDL2/include"
    "${LIB_BIN_DIR}/glew/include"
)

add_library(implot STATIC
    ${THIRD_PARTY_DIR}/implot/implot.cpp
    ${THIRD_PARTY_DIR}/implot/implot_demo.cpp
    ${THIRD_PARTY_DIR}/implot/implot_items.cpp
)
target_include_directories(implot PUBLIC ${THIRD_PARTY_DIR}/implot)
target_link_libraries(implot PRIVATE imgui)

add_executable(main ${SRC_DIR}/main.cpp)

target_include_directories(main PRIVATE 
    ${INCLUDE_DIR}
    ${THIRD_PARTY_DIR}/cppzmq
    ${THIRD_PARTY_DIR}/json/include
    "${LIB_BIN_DIR}/zeromq/include"
    "${LIB_BIN_DIR}/glew/include"
    "${LIB_BIN_DIR}/SDL2/include"
    ${THIRD_PARTY_DIR}/imgui
    ${THIRD_PARTY_DIR}/imgui/backends
    ${THIRD_PARTY_DIR}/implot
)

target_link_libraries(main PRIVATE 
    imgui 
    implot
    ${ZMQ_LIB}
    ${SDL2_LIBS}
    ${OPENGL_LIBRARIES} 
    ${GLEW_LIB}
    -lmingw32 
    -mconsole
)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${LIB_BIN_DIR}/SDL2/bin/SDL2.dll"
    "${LIB_BIN_DIR}/zeromq/bin/libzmq.dll"
    "${LIB_BIN_DIR}/zeromq/bin/libsodium-26.dll"
    "${LIB_BIN_DIR}/glew/bin/glew32.dll"
    $<TARGET_FILE_DIR:main>
)