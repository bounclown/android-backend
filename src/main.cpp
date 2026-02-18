#include <glew.h>
#include <SDL.h>
#include <windows.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <fstream>
#include <iomanip>

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

    try {
        socket.bind("tcp://*:5555");
        std::cout << "|Сервер| прослушивание на порту 5555" << std::endl;
    } catch(const zmq::error_t& e) {
        std::cerr << "|Сервер| ошибочка" << e.what() << std::endl;
        return;
    }
    
    
    for (;;) {
        zmq::message_t request;
        auto res = socket.recv(request, zmq::recv_flags::none);
        if (!res) continue;

        try {
            std::string msg_str(static_cast<char*>(request.data()), request.size());
            auto j = nlohmann::json::parse(msg_str);

            loc->latitude = j.value("lat", 0.0f);
            loc->longitude = j.value("lon", 0.0f);
            loc->altitude = j.value("alt", 0.0f);

            std::ofstream file("last_location.json");
            if (file.is_open()) {
                file << std::setw(4) << j << std::endl;
                file.close();
            }
            socket.send(zmq::str_buffer("ACK"), zmq::send_flags::none);

        } catch (const std::exception& e) {
            std::cerr << "[Сервер] ошибка с файлом: " << e.what() << std::endl;
            socket.send(zmq::str_buffer("ERROR"), zmq::send_flags::none);
        }
    }
}

int main(int argc, char* argv[]){
    static location locationInfo;

    std::thread gui_thread(run_gui, &locationInfo);
    std::thread server_thread(run_server, &locationInfo);

    gui_thread.join();
    server_thread.join();

    return 0;
}