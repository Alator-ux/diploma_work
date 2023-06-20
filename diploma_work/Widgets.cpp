#include "Widgets.h"
#include <format>

bool string_item_getter(void* data, int index, const char** output)
{
    std::string* items = (std::string*)data;
    std::string& current_item = items[index];

    *output = current_item.c_str();

    return true;
}

bool primitive_item_getter(void* data, int index, const char** output)
{
    std::string str = "Object ";
    str.append(std::to_string(index));
    * output = str.c_str();

    return true;
}

char Vec3Selector::identifier = '0';