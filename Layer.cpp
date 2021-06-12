#include "Layer.h"

Layer::Layer(std::string window_title, int height, int width, std::string glsl_version)
{
    _window_title = window_title;
    _height = height;
    _width = width;
    _glsl_version = glsl_version;
    init_sdl_attributes();
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    _window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, window_flags);
    _gl_context = SDL_GL_CreateContext(_window);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_GL_MakeCurrent(_window, _gl_context);
    SDL_GL_SetSwapInterval(1);
}

void init_sdl_attributes()
{
    // DO SDL ERR CHECKING
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

void Layer::init_imgui(bool use_dark_color = true)
{
    bool err = gladLoadGL() == 0;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    _io = ImGui::GetIO();

    if (use_dark_color)
        ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(_window, _gl_context);
    ImGui_ImplOpenGL3_Init(_glsl_version.c_str());
    _background = ImVec4(1.0f, 1.0f, 0.60f, 1.0f);
}

Layer::~Layer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(_gl_context);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Layer::on_new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(_window);
    ImGui::NewFrame();
}

void Layer::render_layer()
{
    ImGui::Render();
    glViewport(0, 0, (int)_io.DisplaySize.x, (int)_io.DisplaySize.y);
    glClearColor(_background.x, _background.y, _background.z, _background.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(_window);
}

SDL_Window *Layer::get_window()
{
    return _window;
}

ImVec4 *Layer::get_background()
{
    return &_background;
}
// void Layer::on_event_quit_loop()
// {
//     while (SDL_PollEvent(&event))
//     {
//         ImGui_ImplSDL2_ProcessEvent(&event);
//         if (event.type == SDL_QUIT)
//             _end = true;
//         if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
//             _end = true;
//     }
// }

