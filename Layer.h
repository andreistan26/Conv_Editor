#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <glad/glad.h>
#include <string>

class Layer
{
public:
//TODO add init as preprocessor
    Layer(std::string window_title, int height, int width, std::string glsl_version);
    ~Layer();
    void init_imgui(bool use_dark_color);
    void on_new_frame();
    void render_layer();
    ImVec4* get_background();
    SDL_Window* get_window();
    // void on_quit_event_loop();
    ImGuiIO _io;
private:
    ImVec4 _background;
    int _height, _width;
    std::string _window_title;
    std::string _glsl_version;
    // bool _end;
    bool _use_dark_colors;
    SDL_Window* _window;
    SDL_GLContext _gl_context;
};

void init();
void init_sdl_attributes();
