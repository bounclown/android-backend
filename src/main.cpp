#include <GL/glew.h>
#include <SDL.h>
#include <windows.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "implot.h"

extern "C" int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return main(__argc, __argv);
}

struct location {
    float latitude;
    float longitude;
    float altitude;
    // и т.д.
};

void run_gui(location *loc) {
    // здесь работает GUI поток
    // Выводит данные из ГЛОБАЛЬНОЙ структуры location
}

void run_server(location *loc) {
    // Здесь работает поток серверный
    // Записывает данные в ГЛОБАЛЬНУЮ структуру location
}

int main(int argc, char* argv[]){
    static location locationInfo;

    std::thread gui_thread(run_gui, &locationInfo);
    std::thread server_thread(run_server, &locationInfo);

    gui_thread.join();
    server_thread.join();

    return 0;
}