#define STB_IMAGE_IMPLEMENTATION

#include "image.h"

static bool reverse = false;
static int blur_core_size = 1;
// static float blur_sigma = 10;
static int size_xy[2] = {0};
static float brightness = 0;
static float contrast = 0;
static bool round_frame = false;
static int frame_width[2] = {0};

Mat image_src;
Mat image_show;

bool LoadTextureFromMat(Mat mat, SDL_Texture **texture_ptr)
{
    extern SDL_Renderer *g_renderer;
    int width = mat.size[1];
    int height = mat.size[0];
    int channels = mat.channels();

    SDL_Log("w:%d, h:%d, c:%d", width, height, channels);

    unsigned char *data = mat.data;

    if (data == nullptr)
    {
        return false;
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void *)data, width, height, channels * 8, channels * width,
                                                    0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

    if (surface == nullptr)
    {
        SDL_Log("Failed to create SDL surface: %s\n", SDL_GetError());
        return false;
    }

    *texture_ptr = SDL_CreateTextureFromSurface(g_renderer, surface);

    if ((*texture_ptr) == nullptr)
    {
        SDL_Log("Failed to create SDL texture: %s\n", SDL_GetError());
    }

    SDL_FreeSurface(surface);

    return true;
}

Mat Blur(Mat src)
{
    SDL_Log("Blur");
    Mat dst;
    int ksize = blur_core_size * 2 - 1;
    cv::GaussianBlur(src, dst, cv::Size(ksize, ksize), 0.0);
    return dst;
}

Mat Resize(Mat src)
{
    SDL_Log("Resize");
    Mat dst;
    cv::resize(src, dst, cv::Size(size_xy[0], size_xy[1]), 0.0, 0.0, cv::INTER_AREA);
    return dst;
}

Mat AddFrame(Mat src)
{
    SDL_Log("AddFrame");
    Mat dst = src; // todo

    return dst;
}

Mat SetBrightnessAndContrast(Mat src)
{
    SDL_Log("SetBrightness");
    Mat dst;
    float alpha = 0;
    if (contrast > 0)
    {
        alpha = 1.0 / (1.001 - contrast);
    }
    else
    {
        alpha = 1.0 + contrast;
    }
    float beta = (brightness - contrast) * 127;
    src.convertTo(dst, -1, alpha, beta);
    return dst;
}

void ClearVar(int width, int height)
{
    reverse = false;
    blur_core_size = 1;
    // static float blur_sigma = 10;
    size_xy[0] = width > 0 ? width : 0;
    size_xy[1] = height > 0 ? height : 0;
    brightness = 0;
    contrast = 0;
    round_frame = false;
    frame_width[0] = 0;
    frame_width[1] = 0;
}

void ImageWindow(bool refresh)
{
    ImGui::Begin("Image");
    static SDL_Texture *my_texture = nullptr;

    if (refresh && !image_src.empty())
    {
        SDL_Log("ImageWindow, refresh------------------");
        Mat temp = image_src;
        if (fabs(brightness) >= 0.001)
            temp = SetBrightnessAndContrast(temp);
        if (fabs(contrast) >= 0.001)
            temp = SetBrightnessAndContrast(temp);
        if (blur_core_size > 1)
            temp = Blur(temp);
        if (size_xy[0] > 0 && size_xy[1] > 0 &&
            (size_xy[0] != image_src.size[1] || size_xy[1] != image_src.size[0]))
            temp = Resize(temp);
        if (round_frame || frame_width[0] > 0 || frame_width[1] > 0)
            temp = AddFrame(temp);
        image_show = temp;

        bool ret = LoadTextureFromMat(image_show, &my_texture);
    }

    if (!image_show.empty() && my_texture != nullptr)
    {
        float ratio = (float)image_show.size[0] / image_show.size[1];
        float w = ImGui::GetColumnWidth();
        float h = ratio * w;
        ImGui::Image(my_texture, ImVec2(w, h), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImGui::GetStyleColorVec4(ImGuiCol_Border));
    }

    ImGui::End();
}

bool SettingWindow()
{
    bool refresh = false;
    ImGui::Begin("Setting");

    ImGui::Button("Open", ImVec2(-1, 50));

    ImGui::SeparatorText("Common");
    refresh |= ImGui::Checkbox("Reverse Image", &reverse);
    refresh |= ImGui::DragInt2("Image Size", size_xy, 2.0f, 1, 4000);
    refresh |= ImGui::SliderFloat("Brightness", &brightness, -1, 1);
    refresh |= ImGui::SliderFloat("Contrast", &contrast, -1, 1);

    ImGui::SeparatorText("Blur");
    refresh |= ImGui::DragInt("Blur Core Size", &blur_core_size, 0.2f, 1, 100);
    // refresh |= ImGui::DragFloat("Blur Sigma", &blur_sigma, 1.0f, 1, 100);

    ImGui::SeparatorText("Frame");
    refresh |= ImGui::Checkbox("Round", &round_frame);
    refresh |= ImGui::DragInt2("Frame Width", frame_width, 0.5f, 0, 1000);

    ImGui::NewLine();
    ImGui::Button("Generate", ImVec2(-1, 50));

    ImGui::End();
    return refresh;
}