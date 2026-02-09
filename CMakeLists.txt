cmake_minimum_required(VERSION 3.14)
project(backend_notes)

set(CMAKE_CXX_STANDARD 17)

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# SDL2
set(SDL2_DIR "C:/SDL2-2.32.10/x86_64-w64-mingw32/lib/cmake/SDL2")
find_package(SDL2 REQUIRED)

# OpenGL/GLEW
find_package(OpenGL REQUIRED)
set(GLEW_INCLUDE_DIRS "C:/libs/glew-2.3.1/include")
set(GLEW_LIBRARIES "C:/libs/glew-2.3.1/lib/Release/x64/glew32.lib")

include_directories(${INCLUDE_DIR})

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
    ${SDL2_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)
target_compile_options(imgui PRIVATE -fPIC)

add_library(implot STATIC
    ${THIRD_PARTY_DIR}/implot/implot.cpp
    ${THIRD_PARTY_DIR}/implot/implot_demo.cpp
    ${THIRD_PARTY_DIR}/implot/implot_items.cpp
)
target_include_directories(implot PUBLIC ${THIRD_PARTY_DIR}/implot)
target_link_libraries(implot PRIVATE imgui)
target_compile_options(implot PRIVATE -fPIC)

add_executable(main ${SRC_DIR}/main.cpp)

target_include_directories(main PRIVATE 
    ${INCLUDE_DIR}
    ${THIRD_PARTY_DIR}/cppzmq
    "C:/libs/zeromq-4.3.5/include"
    "C:/SDL2-2.32.10/x86_64-w64-mingw32/include/SDL2"
)

target_link_libraries(main PRIVATE 
    imgui 
    implot
    "C:/libs/zeromq-4.3.5/lib/libzmq.dll.a"
    "C:/SDL2-2.32.10/x86_64-w64-mingw32/lib/libSDL2main.a"
    "C:/SDL2-2.32.10/x86_64-w64-mingw32/lib/libSDL2.dll.a"
    ${OPENGL_LIBRARIES} 
    ${GLEW_LIBRARIES}
    -lmingw32 
    -mconsole
)