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
    Layer(std::string window_title, int height, int width, std::string glsl_version);
    ~Layer();
    void init_imgui(ImGuiIO& io);
    void on_new_frame();
    void render_layer(ImGuiIO& io);
    inline ImVec4* get_background();
    SDL_Window* get_window();
private: 
    std::string _window_title;
    int _width;
    int _height;
    std::string _glsl_version;
    ImVec4 _background;
    SDL_Window* _window;
    SDL_GLContext _gl_context;
};

void init();
void init_sdl_attributes();
