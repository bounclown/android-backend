#include <glew.h>
#include <SDL.h>
#include <windows.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <ctime>

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
    long long timestamp;
};

void run_gui(location* loc) {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) return;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_Window* window = SDL_CreateWindow("Telemetry Monitor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 300, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    glewInit();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::StyleColorsDark();
    
    float max_alt = -1e9, min_alt = 1e9;
    long long last_timestamp = 0;

    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) done = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (loc->timestamp > last_timestamp) {
            last_timestamp = loc->timestamp;
            if (loc->altitude > max_alt) max_alt = loc->altitude;
            if (loc->altitude < min_alt) min_alt = loc->altitude;
        }

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("Data Terminal", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "SYSTEM TELEMETRY");
        ImGui::Separator();

        if (loc->timestamp > 0) {
            ImGui::Text("Status: "); ImGui::SameLine();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "ONLINE");

            time_t total_sec = (time_t)(loc->timestamp / 1000);
            struct tm* tm_info = localtime(&total_sec);
            char t_str[32];
            strftime(t_str, sizeof(t_str), "%H:%M:%S", tm_info);
            ImGui::Text("Last Sync: %s", t_str);
            
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::SeparatorText("Coordinates");
            
            ImGui::BulletText("Latitude:  %.6f", loc->latitude);
            ImGui::BulletText("Longitude: %.6f", loc->longitude);
            
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::SeparatorText("Altitude");
            
            ImGui::Text("Current:  %.2f m", loc->altitude);
            ImGui::Text("Max:      %.2f m", (max_alt < -1e6 ? 0 : max_alt));
            ImGui::Text("Min:      %.2f m", (min_alt > 1e6 ? 0 : min_alt));

        } else {
            ImGui::Text("Status: "); ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "AWAITING DATA...");
            ImGui::ProgressBar(0.0f, ImVec2(-1, 0), "No connection");
        }

        ImGui::Separator();
        if (ImGui::Button("Reset Session", ImVec2(-1, 30))) {
            max_alt = -1e9; min_alt = 1e9;
            loc->timestamp = 0;
        }

        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
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
            loc->timestamp = j.value("time", 0LL);

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