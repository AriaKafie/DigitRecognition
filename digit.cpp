
#include <cfloat>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

constexpr int Dim = 28;

float model[10][Dim * Dim];

float distance(float *a, float *b)
{
    float dist = 0;

    for (int i = 0; i < Dim * Dim; i++)
    {
        dist += (a[i] - b[i]) * (a[i] - b[i]);
    }

    return std::sqrt(dist);
}

void normalize(std::string img_path, float *dest)
{
    cv::Mat img = cv::imread(img_path, cv::IMREAD_GRAYSCALE);
    
    if (img.empty())
    {
        std::cerr << "Error: Could not open " << img_path << std::endl;
        std::exit(-1);
    }

    cv::Mat img_resized;
    cv::resize(img, img_resized, cv::Size(Dim, Dim));

    unsigned char *data = img_resized.data;

    int sum = 0;
    for (int i = 0; i < Dim * Dim; sum += data[i++]);
    int avg = sum / (Dim * Dim);

    bool invert = avg > 127;
    
    for (int i = 0; i < Dim * Dim; i++)
        dest[i] = (invert ? 255 - data[i] : data[i]) / 255.0f;
}

int main()
{
    std::cout << "Training..." << std::endl;
    
    float normalized[Dim * Dim];
    
    for (int digit = 0; digit < 10; digit++)
    {
        int img_count = 0;
        
        for (const auto& entry : std::filesystem::directory_iterator(std::string("./dataset/") + std::to_string(digit)))
        {
            ++img_count;
            
            if (entry.is_regular_file())
            {
                normalize(entry.path(), normalized);

                for (int i = 0; i < Dim * Dim; i++)
                    model[digit][i] += (normalized[i] - model[digit][i]) / img_count;
            }
        }
    }

    std::cout << "Done training" << std::endl;

    for (;;)
    {
        std::string img_path;
        std::getline(std::cin, img_path);

        normalize(img_path, normalized);

        float min_distance = FLT_MAX;
        int digit_match;

        for (int digit = 0; digit < 10; digit++)
        {
            float dist = distance(model[digit], normalized);

            if (dist < min_distance)
            {
                min_distance = dist;
                digit_match = digit;
            }
        }

        std::cout << "It's the number " << std::to_string(digit_match) << std::endl;
    }
    
    return 0;
}
