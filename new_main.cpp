#define STB_IMAGE_IMPLEMENTATION
#define IMGUI_HAS_DOCK
#define IMGUI_HAS_VIEWPORT 
#define USE_DARK_MODE
#include "Layer.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>
#include "ImGuiFileDialog.h"

bool LayerLoadTextureFromFile(const char *filename, GLuint *out_texture, int *out_width, int *out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char *image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

int main()
{
    init_sdl_attributes();
    
    const std::string window_title{"Editor"};
    const std::string glsl_version = "#version 130";
    const int height = 700;
    const int width = 900;
    Layer layer{window_title, height, width, glsl_version};

    GLuint my_image_texture = 0;
    layer.init_imgui();
    ImGuiIO& io = ImGui::GetIO();(void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    //io.ConfigWindowsResizeFromEdges = true;
    std::string filename;

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplSDL2_InitForOpenGL(layer.get_window(), layer.get_gl_context());
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(layer.get_window()))
                done = true;
        }
        layer.on_new_frame();

        int my_image_width, my_image_height;
        ImGui::Begin("open", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        if(ImGui::Button("Open File Browser"))
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", nullptr, ".");
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                filename = ImGuiFileDialog::Instance()->GetFilePathName();
                LayerLoadTextureFromFile(filename.c_str(), &my_image_texture, &my_image_width, &my_image_height);
            }
            ImGuiFileDialog::Instance()->Close();
        }
        ImGui::End();

        ImGui::Begin("Opencv");
        ImGui::Image((void *)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));

        ImGui::End();
        layer.render_layer(io);
    }
    return 0;
}
