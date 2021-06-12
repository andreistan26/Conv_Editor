#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "Stb/stb_image.h"
#include "Layer.h"
using namespace cv;


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
    Layer layer((std::string) "opencv", 700, 700, (std::string) "#version 450");
    layer.init_imgui(true);

    std::string image_path = samples::findFile("res/stack1scaled.jpeg");
    Mat img = imread(image_path, IMREAD_COLOR);
    Mat imgHSV;
    cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
    bool done = false;
    int my_image_width = 400;
    int my_image_height = 300;
    GLuint my_image_texture = 0;

    bool ret = LayerLoadTextureFromFile("res/rezultat.jpg", &my_image_texture, &my_image_width, &my_image_height);

    while (!done)
    {
        static double distance = 0;
        static int rectX = 56, rectY = 343, rectWidth = 27, rectHeight = 30;
        static int bgrValues[3] = {2, 3, 109};
        static int meanValues[3] = {8, 243, 105};
        static int thresh = 80;
        static bool hsv_bool = true;
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

        ImGui::Begin("Opencv");
        ImGui::Image((void *)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
        ImGui::End();

        ImGui::Begin("Edit Fields");
        ImGui::SliderInt("thresh", &thresh, 0, 200);
        ImGui::SliderInt("rectX", &rectX, 0, 199);
        ImGui::SliderInt("rectY", &rectY, 0, 399);
        ImGui::SliderInt("width", &rectWidth, 0, 60);
        ImGui::SliderInt("height", &rectHeight, 0, 100);
        ImGui::Text(std::to_string(distance).c_str());
        // ImGui::Checkbox("HSV", &hsv_bool);
        // ImGui::InputInt3("bgrValues", bgrValues);

        ImGui::End();
        layer.render_layer();
        cv::Vec3b bgrPixel(bgrValues[0], bgrValues[1], bgrValues[2]);
        //distanta pt mean
        cv::Scalar mean_mask_rgb(meanValues[0], meanValues[1], meanValues[2]);
        Mat3b bgr(bgrPixel);
        Mat3b hsv;
        cvtColor(bgr, hsv, COLOR_BGR2HSV);

        Vec3b hsvPixel(hsv.at<Vec3b>(0, 0));


        Scalar minHSV = cv::Scalar(hsvPixel.val[0] - thresh, hsvPixel.val[1] - thresh, hsvPixel.val[2] - thresh);
        Scalar maxHSV = cv::Scalar(hsvPixel.val[0] + thresh, hsvPixel.val[1] + thresh, hsvPixel.val[2] + thresh);

        cv::Mat maskHSV, resultHSV;
        cv::inRange(imgHSV, minHSV, maxHSV, maskHSV);

        cv::bitwise_and(imgHSV, imgHSV, resultHSV, maskHSV);
        rectangle(resultHSV, Point(rectX, rectY), Point(rectX + rectWidth, rectY + rectHeight), Scalar(0, 0, 255));
        Rect sub_rect(rectX, rectY, rectWidth, rectHeight);
        Mat sub_mat(resultHSV, sub_rect);
        Scalar val = mean(sub_mat);
        double delta = 0;
        for (int i = 0; i < 3; ++i)
        {
            delta += pow(val[i] - mean_mask_rgb[i], 2);
        }
        delta = sqrt(delta);
        distance = delta;
        imwrite("res/rezultat.jpg", resultHSV);
        ret = LayerLoadTextureFromFile("res/rezultat.jpg", &my_image_texture, &my_image_width, &my_image_height);

    }
    return 0;
}
