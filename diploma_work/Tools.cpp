#include "Tools.h"

constexpr float eps = 0.001f; // TODO потом как-нибудь на нормальное заменить
bool vec3_equal(const glm::vec3& f, const glm::vec3& s) {
    bool res = true;
    res = res && glm::abs(f.x - s.x) < eps;
    res = res && glm::abs(f.y - s.y) < eps;
    res = res && glm::abs(f.z - s.z) < eps;
    return res;
}

std::vector<std::string> split(const std::string& str, char delim) {
    std::stringstream ss(str);
    std::vector<std::string> res;
    std::string temp;
    while (std::getline(ss, temp, delim)) {
        res.push_back(temp);
    }
    return res;
}


std::vector<float> one_dim_gkernel(int size, float sigma) {
    std::vector<float> res(size);
    float disp = 0.f;
    if (sigma == 0) {
        float avg = size * 0.5f;
        for (int i = 0; i < size; i++) {
            disp += std::pow(i + 1 - avg, 2);
        }
        disp /= size;
    }
    else {
        disp = sigma * sigma;
    }
    float denom = 1 / std::sqrt(2 * M_PI * disp);
    for (int i = 0; i < size; i++) {
        float x_sq = i * i;
        res[i] = std::exp(-x_sq / (2.f * disp)) * denom;
    }
    return res;
}

void print_vec3(const glm::vec3& value, const std::string& prefix, const std::string& postfix) {
    std::cout << prefix 
        << "vec3(x=" << value.x << ",y=" << value.y << ",z=" << value.z << ")" 
        << postfix << std::endl;
}