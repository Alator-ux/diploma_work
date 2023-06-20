#pragma once
#include "Figure.h"
#include "Primitives.h"
#include <glm/fwd.hpp>
#include <vector>

template <typename T>
class Selector {
    std::vector<T>* storage;
    int current_item;
public:
    Selector() {
        this->storage = nullptr;
        this->current_item = -1;
    }
    Selector(std::vector<T>* storage) {
        this->storage = storage;
        this->current_item = -1;
    }
    void set_item_index(int index) {
        this->current_item = index;
    }
    T* get_item() {
        if (current_item < 0) {
            return nullptr;
        }
        return &(*storage)[current_item];
    }
};