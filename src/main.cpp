#include <GL/glew.h>
#include <SDL.h>
#include <windows.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <zmq_addon.hpp>
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
    
};

void run_gui(location *loc) {
    // здесь работает GUI поток
    // Выводит данные из ГЛОБАЛЬНОЙ структуры location
}

void run_server(location *loc) {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");
    
    for (;;) {
        zmq::message_t request;
        socket.recv(request);



    }
    // Здесь работает поток серверный
    // Записывает данные в ГЛОБАЛЬНУЮ структуру location
}

int main(int argc, char* argv[]){
    static location locationInfo;

    try {
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::rep);
        std::cout << "ZMQ Context created successfully!" << std::endl;
    } catch (const zmq::error_t& e) {
        std::cerr << "ZMQ Error: " << e.what() << std::endl;
    }
    
    std::thread gui_thread(run_gui, &locationInfo);
    std::thread server_thread(run_server, &locationInfo);

    gui_thread.join();
    server_thread.join();

    return 0;
}