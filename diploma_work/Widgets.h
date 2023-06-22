#ifndef WIDGETS_H
#define WIDGETS_H
#include "imgui.h"
#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <sstream>

bool string_item_getter(void* data, int index, const char** output);
class Widget abstract {
protected:
    const char* label;
    bool _activatable;
    bool _activated;
public:
    Widget(const char* label, bool activatable) : label(label), _activatable(activatable),
        _activated(true) {}
    virtual bool draw() = 0;
    virtual const void* get_value() = 0;
    void activate() {
        if (_activatable) {
            _activated = true;
        }
    };
    void disable() {
        if (_activatable) {
            _activated = false;
        }
    }
    bool activatable() {
        return _activatable;
    }
    bool activated() {
        return _activated;
    }
};
class InputString : public Widget {
    char content[256];
    int min_size;
    std::string str_content = "t";
public:
    InputString(const char* label, int min_size)
        : Widget(label, false), min_size(min_size) { }
    bool draw() override {
        bool drawn = ImGui::InputText(label, content, 256);
        if (strlen(content) == 0)
        {
            strcpy_s(content, "t");
        }
        if (drawn) {
            str_content = std::string(content);
        }
        return drawn;
    }
    const void* get_value() override {
        return (void*)(&str_content);
    }
};
class Row : public Widget {
    std::vector<std::shared_ptr <Widget>> childs;
public:
    Row(const char* label) : Widget(label, false), childs() {}
    Row(const char* label, std::vector<std::shared_ptr <Widget>>&& childs) : Widget(label, false) {
        set_childs(std::move(childs));
    }
    void set_childs(std::vector<std::shared_ptr <Widget>>&& childs) {
        this->childs = std::move(childs);
    }
    bool draw() override {
        ImGui::Text(label);
        bool was_pressed = false;
        for (size_t i = 0; i < childs.size() - 1; i++) {
            bool pressed = childs[i]->draw();
            ImGui::SameLine();
            if (pressed) {
                was_pressed = true;
            }
        }
        if (childs.back()->draw()) {
            was_pressed = true;
        }
        return was_pressed;
    }
    const void* get_value() override {
        return nullptr;
    }
    const std::vector<std::shared_ptr <Widget>>* get_childs() {
        return &childs;
    }
};
class CheckBox : public Widget {
public:
    CheckBox(const char* label, bool checked = false) : Widget(label, true) {
        this->_activated = checked;
    }
    bool draw() override {
        return ImGui::Checkbox(label, &_activated);
    }
    const void* get_value() override {
        return (void*)(&_activated);
    }
    void activate() {
        _activated = true;
    }
    void disable() { 
        _activated = false;
    }
};


class DropDownMenu : public Widget {
    std::vector<std::string> items;
    float width;
    int selectedItem = 0;
public:
    DropDownMenu(const char* label, const std::vector<std::string>& items, float width = -1) : Widget(label, true) {
        this->items = items;
        this->width = width;
    }
    bool draw() override {
        if (width == -1) {
            return ImGui::Combo(label, &selectedItem, string_item_getter,
                (void*)items.data(), (int)items.size());
        }
        ImGui::PushItemWidth(width);
        bool touched = ImGui::Combo(label, &selectedItem, string_item_getter,
            (void*)items.data(), (int)items.size());
        ImGui::PopItemWidth();
        return touched;
    }
    const void* get_item() {
        return (void*)&items[selectedItem];
    }
    const void* get_value() {
        return (void*)&selectedItem;
    }
};

class ColorChooser {
    const char* label;
    glm::vec4 color;
public:
    ColorChooser(const char* label) {
        this->label = label;
        this->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    bool draw() {
        return ImGui::ColorEdit3(label, (float*)&color);
    }
    glm::vec3 rgb_value() {
        return glm::vec3(color);
    }
};

class IntSlider {
    const char* label;
    int value;
    int min_value;
    int max_value;
public:
    IntSlider(const char * label, int min_value, int max_value) {
        this->label = label;
        this->value = min_value;
        this->min_value = min_value;
        this->max_value = max_value;
    }
    bool draw() {
        return ImGui::SliderInt(label, &value, min_value, max_value);
    }
    int get_value() {
        return value;
    }
};

class FloatSlider {
    const char* label;
    float value;
    float min_value;
    float max_value;
public:
    FloatSlider(const char* label, float min_value, float max_value) {
        this->label = label;
        this->value = min_value;
        this->min_value = min_value;
        this->max_value = max_value;
    }
    bool draw() {
        return ImGui::SliderFloat(label, &value, min_value, max_value);
    }
    float get_value() {
        return value;
    }
};

class RadioButton {
    const char* label;
    bool activated;
public:
    RadioButton(const char* label) {
        this->label = label;
        this->activated = false;
    }
    RadioButton(const char* label, bool activated) {
        this->label = label;
        this->activated = activated;
    }
    bool draw() {
        return ImGui::RadioButton(label, activated);
    }
    void activate(){
        this->activated = true;
    }
    void disable() {
        this->activated = false;
    }
};

class RadioButtonRow {
    std::vector<RadioButton> buttons;
    std::vector<std::string> labels;
    int active = 0;
public:
    RadioButtonRow(std::vector<std::string> labels) {
        this->labels = labels;
        bool flag = true;
        for (std::string& label : this->labels) {
            auto r = RadioButton(label.c_str(), flag);
            buttons.push_back(r);
            flag = false;
        }
    }
    bool draw() {
        bool was_pressed = false;
        for (size_t i = 0; i < buttons.size(); i++) {
            bool pressed = buttons[i].draw();
            if (i != buttons.size() - 1) {
                ImGui::SameLine();
            }
            if (pressed && active != i) {
                buttons[i].activate();
                buttons[active].disable();
                active = i;
                was_pressed = true;
            }
        }

        return was_pressed;
    }
    int get_value() {
        return active;
    }
    std::string get_label() {
        return active < 0 ? "" : labels[active];
    }
};

class TabBar {
    std::vector<std::string> labels;
    int active = 0;
public:
    TabBar(std::vector<std::string> labels) {
        this->labels = labels;
    }
    bool draw() {
        bool was_pressed = false;
        ImGui::BeginTabBar("aa");
        for (size_t i = 0; i < labels.size(); i++) {
            ImGui::SetNextItemWidth(100);
            if (ImGui::TabItemButton(labels[i].c_str(), active == i)) {
                was_pressed = true;
                active = i;
            }
        }
        ImGui::EndTabBar();
        return was_pressed;
    }
    int get_value() {
        return active;
    }
    std::string get_label() {
        return active < 0 ? "" : labels[active];
    }
};

class InputFloat {
    std::string label;
    float value = 0.0f;
    bool min_on = false;
    float min;
public:
    InputFloat(const std::string& label) {
        this->label = label;
    }
    InputFloat(const std::string& label, float init_value) : label(label), value(init_value) { }
    InputFloat(const std::string& label, float init_value, float min) :label(label), value(init_value),
    min(min), min_on(true) { }
    bool draw() {
        ImGui::PushItemWidth(60);
        bool touched = ImGui::InputFloat(label.c_str(), &value);
        ImGui::PopItemWidth();
        if (min_on && value < min) {
            value = min;
        }
        return touched;
    }
    float get_value() {
        return value;
    }
};

class InputSizeT {
    std::string label;
    int value = 0.0f;
    int min;
    bool min_on = false;
public:
    InputSizeT(const std::string& label) :label(label) { }
    InputSizeT(const std::string& label, int init_value) : label(label), value(init_value) { }
    InputSizeT(const std::string& label, int init_value, size_t min) : 
        label(label), value(init_value), min(min), min_on(true) { }
    bool draw() {
        ImGui::PushItemWidth(60);
        bool touched = ImGui::InputInt(label.c_str(), &value, 0);
        if (value < 0) {
            value = 0;
        }
        ImGui::PopItemWidth();
        return touched;
    }
    int get_value() {
        return value;
    }
};

class InputInt {
    std::string label;
    int value = 0.0f;
    int min;
    bool min_on = false;
public:
    InputInt(const std::string& label) : label(label) { }
    InputInt(const std::string& label, int init_value) :label(label), value(init_value) { }
    InputInt(const std::string& label, int init_value, int min) :label(label), value(init_value),
        min(min), min_on(true) {}
    bool draw() {
        ImGui::PushItemWidth(60);
        bool touched = ImGui::InputInt(label.c_str(), &value, 0);
        if (min_on && value < min) {
            value = min;
        }
        ImGui::PopItemWidth();
        return touched;
    }
    int get_value() {
        return value;
    }
};

class Vec3Selector {
    std::vector<InputFloat> text_fields;
    static char identifier;
    glm::vec3 value;
public:
    Vec3Selector(const glm::vec3& init_value) {
        char id[1];
        id[0] = identifier;
        std::string label = "r##";
        label.append(id);
        text_fields.push_back(InputFloat(label, init_value.x));
        label = "g##";
        label.append(id);
        text_fields.push_back(InputFloat(label, init_value.y));
        label = "b##";
        label.append(id);
        text_fields.push_back(InputFloat(label, init_value.z));
        identifier++;
    }
    bool draw() {
        bool touched = false;
        for (size_t i = 0; i < text_fields.size(); i++) {
            touched = touched || text_fields[i].draw();
            if (i != text_fields.size() - 1) {
                ImGui::SameLine();
            }
        }
        return touched;
    }
    glm::vec3 get_value() {
        value.x = text_fields[0].get_value();
        value.y = text_fields[1].get_value();
        value.z = text_fields[2].get_value();
        return value;
    }
};

#endif