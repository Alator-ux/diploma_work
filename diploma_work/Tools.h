#pragma once
#include <random>
#include <numeric>
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <GLM/ext/matrix_transform.hpp>
#include <queue>
#include <set>
#include <chrono>
#include <stack>
#include <sstream>
template <typename T>
struct Random {
    /// <summary>
    /// Устанавливает текущим зерном текущее время
    /// </summary>
    static void set_seed() {
        std::srand(static_cast <unsigned> (std::time(0)));
    }
    /// <summary>
    /// Устанавливает текущим зерном переданный параметр
    /// </summary>
    /// <param name="seed"></param>
    static void set_seed(unsigned seed) {
        std::srand(seed);
    }
    /// <summary>
    /// Возвращает случайное число от 0.0 до 1.0 включительно
    /// </summary>
    static T random() {
        return static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
    }
    /// <summary>
    /// Возвращает случайное число от 'from' до 'to' включительно
    /// </summary>
    static T random(T from, T to) {
        if (from == to) {
            return from;
        }
        return from + static_cast<T>(rand()) / (static_cast<T>(RAND_MAX) / static_cast<T>(to - from));
    }
};
class Timer {
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
    long long _duration;
    long long total;
    int status;
    long long milliseconds()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }
public:
    Timer() {
        clear();
    }
    void start() {
        status = 1;
        start_time = std::chrono::high_resolution_clock::now();
    }
    void end() {
        status = 0;
        end_time = std::chrono::high_resolution_clock::now();
        _duration = milliseconds();
    }
    void check_point() {
        status = 2;
        end_time = std::chrono::high_resolution_clock::now();
        _duration = milliseconds();
    }
    void sum_total() {
        total += _duration;
    }
    long long duration() {
        if (status == 0) {
            throw std::exception("Timer is still running");
        }
        return _duration;
    }
    void print() {
        std::cout << "Time: " << duration() << std::endl;
    }
    void print_total() {
        std::cout << "Total time: " << total << std::endl;
    }
    void clear() {
        total = 0;
        status = 0;
    }
};
bool vec3_equal(const glm::vec3& f, const glm::vec3& s);
struct Vec3Hash {
    size_t operator()(const glm::vec3& v) const {
        size_t h1 = std::hash<float>()(v.x);
        size_t h2 = std::hash<float>()(v.y);
        size_t h3 = std::hash<float>()(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};
template <typename Iterator,
    typename R = typename std::iterator_traits<Iterator>::reference>
auto deref(std::pair<Iterator, Iterator> p)
-> std::pair<R, R>
{
    return { *p.first, *p.second };
}
template<typename T>
class DeepLookStack : public std::stack<T>
{
    int top = -1;
public:
    void push(const T& elem) {
        std::stack<T>::push(elem);
        top++;
    }
    T peek(size_t left_shift = 0) {
        if (this->empty())
        {
            throw std::exception("Stack is empty");
        }
        if (top - left_shift < 0) {
            throw std::exception(top - left_shift + " < 0");
        }
        return this->c[top - left_shift];
    }
    void pop() {
        std::stack<T>::pop();
        top--;
    }
};
template <typename T>
class Matrix
{
    std::vector<T> inner_;
    unsigned int dimx_, dimy_;

public:
    Matrix() : dimx_(0), dimy_(0) {}
    Matrix(unsigned int dimx, unsigned int dimy, const T& def_value)
        : dimx_(dimx), dimy_(dimy)
    {
        inner_ = std::vector<T>(dimx_ * dimy_);
        fill(def_value);
    }
    T& operator()(unsigned int x, unsigned int y)
    {
        if (x >= dimx_ || y >= dimy_)
            throw std::out_of_range("matrix indices out of range"); // ouch
        return inner_[dimx_ * y + x];
    }
    void fill(const T& value) {
        std::generate(inner_.begin(), inner_.end(), [&value]() { return value; });
    }
    void clear() {
        inner_ = std::vector<T>(dimx_ * dimy_);
    }
    T* data() {
        return inner_.data();
    }
    unsigned int size() {
        return dimx_ * dimy_;
    }
};

template<typename T>
T convert(const void* pointer) {
    return *((const T*)pointer);
}

std::vector<std::string> split(const std::string& str, char delim);

std::vector<float> one_dim_gkernel(int size, float sigma = 0);

void print_vec3(const glm::vec3& value, const std::string& prefix = "", const std::string& postfix = "");