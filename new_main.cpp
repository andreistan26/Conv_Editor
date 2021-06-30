#define IMGUI_HAS_DOCK
#define IMGUI_HAS_VIEWPORT
#define USE_DARK_MODE
#include "Layer.h"
#include <iostream>
#include "ImGuiFileBrowser.h"
#include "image_loader.h"
#include "imgui_internal.h"
imgui_addons::ImGuiFileBrowser file_dialog; // As a class member or globally

bool show_dialog(std::string &filepath)
{
    bool save = false;
    bool open = false;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open", NULL))
                open = true;
            if (ImGui::MenuItem("Save", NULL))
                save = true;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if (open)
        ImGui::OpenPopup("Open File");
    if (save)
        ImGui::OpenPopup("Save File");
    if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".png,.jpeg,.jpg"))
    {
        filepath = file_dialog.selected_path;
        return true;
    }
    if (file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), ".png,.jpeg,.jpg"))
    {
        std::cout << file_dialog.selected_fn << std::endl;   // The name of the selected file or directory in case of Select Directory dialog mode
        std::cout << file_dialog.selected_path << std::endl; // The absolute path to the selected file
        std::cout << file_dialog.ext << std::endl;           // Access ext separately (For SAVE mode)

        //Do writing of files based on extension here
    }
    return false;
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
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.ConfigWindowsResizeFromEdges = true;
    std::string filename;

    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplSDL2_InitForOpenGL(layer.get_window(), layer.get_gl_context());
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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

        //            DOCKSPACE START
        ImGuiID dockspace_id;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar ;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("Dockspace", nullptr, window_flags);

        ImGui::PopStyleVar(3);

        if (ImGui::DockBuilderGetNode(ImGui::GetID("MyDockSpace")) == NULL)
        {
            dockspace_id = ImGui::GetID("MyDockSpace");

            ImGui::DockBuilderRemoveNode(dockspace_id);               
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);

            ImGuiID dock_main_id = dockspace_id; 
            ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, NULL, &dock_main_id);
            ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.2f, NULL, &dock_main_id);

            ImGui::DockBuilderDockWindow("Logs", dock_id_bottom);
            ImGui::DockBuilderDockWindow("Filters", dock_id_right);
            ImGui::DockBuilderDockWindow("Image Viewer", dock_main_id);
            ImGui::DockBuilderFinish(dockspace_id);
        }
        dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        ImGui::End();

        ImGui::Begin("Logs", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::End();

        ImGui::Begin("Filters", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::End();

        ImGui::Begin("Image Viewer", nullptr,  ImGuiWindowFlags_NoTitleBar);
        if (show_dialog(filename))
        {
            std::cout << "show img " << std::endl;
            std::cout << filename << std::endl;
            if (LayerLoadTextureFromFile(filename.c_str(), &my_image_texture, &my_image_width, &my_image_height))
            {
                std::cout << "suc" << std::endl;
            }
        }

        ImGui::Image((void *)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));

        ImGui::End();
        layer.render_layer(io);
    }
    return 0;
}
