#pragma once
#include <map>
#include <string>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Texture.h"
#include "Light.h"
#include "Material.h"
#include <glm/gtc/type_ptr.hpp>

struct OpenGLManager {
private:
    static OpenGLManager* instance;
    std::map<std::string, GLuint> _buffers;
    std::map<std::string, GLuint> _vao;
    std::map<std::string, GLuint> _framebuffers;
    std::map<std::string, Texture> _textures;
    std::map<std::string, GLuint> _renderbuffers;
    GLuint colorbuffer_attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    OpenGLManager() {
        _buffers = std::map<std::string, GLuint>();
        _vao = std::map<std::string, GLuint>();
    }
public:
    static OpenGLManager* get_instance() {
        if (!instance) {
            instance = new OpenGLManager();
        }
        return instance;
    }
    template <typename T>
    void init_vbo(const std::string& name, const T* arr, GLuint size, GLenum usage) {
        GLuint buffer;
        if (_buffers.find(name) == _buffers.end()) {
            glGenBuffers(1, &buffer);
        }
        else {
            buffer = _buffers[name];
        }
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, size, arr, usage);
        checkOpenGLerror();
        _buffers[name] = buffer;
    }
    template <typename T>
    bool refresh_vbo(const std::string& name, const T* arr, GLuint size, GLenum usage) {
        GLuint buffer = get_buffer_id(name);
        if (buffer == -1) {
            return false;
        }
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, size, arr, usage);
        checkOpenGLerror();
        return true;
    }
    template <typename T>
    void init_ibo(const std::string& name, const T* arr, GLuint size) {
        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, arr, GL_STATIC_DRAW);
        checkOpenGLerror();
        _buffers[name] = buffer;
    }
    bool buffer_exists(const std::string& name) {
        return _buffers[name] != NULL;
    }
    GLuint get_buffer_id(const std::string& name) {
        GLuint res = _buffers[name];
        if (res == NULL) {
            std::cout << "No buffer with name " << name << std::endl;
            res = -1;
        }
        return res;
    }
    Texture* get_texture(const std::string& name) {
        Texture* res;
        if (!_textures.count(name)) {
            std::cout << "No texture with name " << name << std::endl;
            res = nullptr;
        }
        else {
            res = &_textures[name];
        }
        return res;
    }
    void gen_vao(const std::string& name) {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        _vao[name] = vao;
    }
    void bind_vao(const std::string& name) {
        glBindVertexArray(_vao[name]);
    }
    void unbind_vao() {
        glBindVertexArray(0);
    }
    void init_hdr(const std::string& frame_name,
        const std::string& color_name,
        const std::string& depth_name,
        const GLuint width, const GLuint height) {
        gen_framebuffer(frame_name);
        bind_framebuffer(frame_name);
        attach_renderbuffer(depth_name, width, height);
        init_colorbuffer(color_name, width, height);
        attach_colorbuffer(color_name);
        unbind_framebuffer();
    }
    void gen_framebuffer(const std::string& frame_name) {
        GLuint buffer;
        glGenFramebuffers(1, &buffer);
        _framebuffers[frame_name] = buffer;
    }
    void attach_renderbuffer(const std::string& depth_name,
        const GLuint width, const GLuint height) {
        GLuint rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        _renderbuffers[depth_name] = rboDepth;
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderbuffers[depth_name]);
    }
    void attach_colorbuffer(const std::string& color_name, GLuint n = 0) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, colorbuffer_attachments[n], GL_TEXTURE_2D, _textures[color_name].getId(), 0);
    }
    void set_texture(const std::string& name, Texture& tex) {
        _textures[name] = tex;
    }
    void init_colorbuffer(const std::string& color_name, const GLuint width, const GLuint height) {
        Texture colorBuffer = Texture();
        colorBuffer.create_rgb_buffer(width, height);
        _textures[color_name] = colorBuffer;
    }
    void create_wrap_clamp_buffer(const std::string& color_name, const GLuint width, const GLuint height) {
        Texture colorBuffer = Texture();
        colorBuffer.create_wrap_clamp_buffer(width, height);
        _textures[color_name] = colorBuffer;
    }
    void create_rgba_buffer(const std::string& color_name, const GLuint width, const GLuint height) {
        Texture colorBuffer = Texture();
        colorBuffer.create_rgba_buffer(width, height);
        _textures[color_name] = colorBuffer;
    }
    void create_wrap_clamp_rgb16f_buffer(const std::string& color_name, const GLuint width, const GLuint height) {
        Texture colorBuffer = Texture();
        colorBuffer.create_wrap_clamp_rgb16f_buffer(width, height);
        _textures[color_name] = colorBuffer;
    }
    void create_point_buffer(const std::string& color_name, const GLuint width, const GLuint height) {
        Texture colorBuffer = Texture();
        colorBuffer.create_point_buffer(width, height);
        _textures[color_name] = colorBuffer;
    }
    void create_noise_texture(const std::string& color_name, const GLuint width, const GLuint height,
        const std::vector<glm::vec3>& noise) {
        Texture colorBuffer = Texture();
        colorBuffer.create_noise_texture(width, height, noise);
        _textures[color_name] = colorBuffer;
    }
    void bind_framebuffer(const std::string& frame_name) {
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffers[frame_name]);
    }
    void unbind_framebuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void set_shader_output_number(GLuint n) {
        if (n == 2) {
            GLuint arr[2];
            arr[0] = colorbuffer_attachments[0];
            arr[1] = colorbuffer_attachments[1];
            glDrawBuffers(n, arr);
        }
        else {
            GLuint arr[3];
            arr[0] = colorbuffer_attachments[0];
            arr[1] = colorbuffer_attachments[1];
            arr[2] = colorbuffer_attachments[2];
            glDrawBuffers(n, arr);
        }
    }
    void check_framebuffer_completeness() {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }
    static void checkOpenGLerror() {
        GLenum errCode;
        // Коды ошибок можно смотреть тут
        // https://www.khronos.org/opengl/wiki/OpenGL_Error
        if ((errCode = glGetError()) != GL_NO_ERROR) {
            std::cout << "OpenGl error! (" << errCode << "): " << glewGetErrorString(errCode) << std::endl;
        }
    }

    void release() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        for (auto it = _buffers.begin(); it != _buffers.end(); it++) {
            glDeleteBuffers(1, &it->second);
        }
        for (auto it = _vao.begin(); it != _vao.end(); it++) {
            glDeleteBuffers(1, &it->second);
        }
    }
};

class Shader {
    enum class ShaderType { vertex, fragment, geometry };
    GLuint program;
    std::stringstream ss;
    // Функция печати лога шейдера
    void ShaderLog(unsigned int shader)
    {
        int infologLen = 0;
        int charsWritten = 0;
        char* infoLog;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
        if (infologLen > 1)
        {
            infoLog = new char[infologLen];
            if (infoLog == NULL)
            {
                std::cout << "ERROR: Could not allocate InfoLog buffer" << std::endl;
                exit(1);
            }
            glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
            std::cout << "InfoLog: " << infoLog << "\n\n\n";
            delete[] infoLog;
        }
        else {
            std::cout << "There are no syntax errors\n";
        }
    }
    void read_shader(const char* path) {
        std::ifstream shader_file(path);
        std::string line;

        if (shader_file.is_open())
        {
            while (std::getline(shader_file, line))
            {
                ss << line << '\n';
            }
            shader_file.close();
        }
        else std::cout << "Unable to open file";
    }
public:
    GLuint compile_shader(ShaderType shader_type, const char* source) {
        GLuint shader;
        if (shader_type == ShaderType::vertex) {
            shader = glCreateShader(GL_VERTEX_SHADER);
            std::cout << "Vertex shader info:\n";
        }
        else if (shader_type == ShaderType::fragment) {
            shader = glCreateShader(GL_FRAGMENT_SHADER);
            std::cout << "Fragment shader info:\n";
        }
        else {
            shader = glCreateShader(GL_GEOMETRY_SHADER);
            std::cout << "Gemotry shader info:\n";
        }
        read_shader(source);
        std::string content = ss.str();
        const char* c_str_content = content.c_str();
        glShaderSource(shader, 1, &c_str_content, NULL);
        glCompileShader(shader);
        ShaderLog(shader);
        //delete[] content;
        ss.str(std::string()); // clearing stringstream
        return shader;
    }
    void release() {
        // Передавая ноль, мы отключаем шейдрную программу
        glUseProgram(0);
        // Удаляем шейдерную программу
        glDeleteProgram(program);
    }

    void init_shader(const std::string& vertex_source = "", const std::string& frag_source = "", 
        const std::string& geometry_source = "") {

        program = glCreateProgram();
        std::cout << "Program {" << program << "}\n";
        GLuint shader;
        if (vertex_source != "") {
            shader = compile_shader(ShaderType::vertex, vertex_source.c_str());
            glAttachShader(program, shader);
        }
        // Создаем программу и прикрепляем шейдеры к ней
        if (geometry_source != "") {
            shader = compile_shader(ShaderType::geometry, geometry_source.c_str());
            glAttachShader(program, shader);
        }
        if (frag_source != "") {
            shader = compile_shader(ShaderType::fragment, frag_source.c_str());
            glAttachShader(program, shader);
        }
        // Линкуем шейдерную программу
        glLinkProgram(program);
        // Проверяем статус сборки
        int link_ok;
        glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
        if (!link_ok)
        {
            std::cout << "error attach shaders \n";
            return;
        }
        else {
            std::cout << "Program {" << program << "} was successfully compiled\n";
        }
        std::cout << "-----------------------------\n";
    }
    Shader(){
        program = 0;
        ss = std::stringstream();
    }
    Shader(const Shader& other) {
        program = other.program;
        ss = std::stringstream();
    }
    GLuint get_program_id() {
        return program;
    }
    GLint get_uniform_location(const char* name)
    {
        GLint res = glGetUniformLocation(program, name);
        if (res == -1)
        {
            std::cout << "could not bind uniform " << name << std::endl;
            //error
        }
        return res;
    }
    GLint get_attrib_location(const char* name)
    {
        GLint res = glGetAttribLocation(program, name);
        if (res == -1)
        {
            std::cout << "could not bind attrib " << name << std::endl;
            //error
        }
        return res;
    }
    void use_program() {
        glUseProgram(program);
    }
    void disable_program() {
        glUseProgram(0);
    }
    void uniform1i(GLint location, int v) {
        glUniform1i(location, v);
    }
    void uniform1i(const char* name, int v) {
        GLint location = get_uniform_location(name);
        glUniform1i(location, v);
    }
    void uniform1f(GLint location, GLfloat v) {
        glUniform1f(location, v);
    }
    void uniform1f(const char* name, GLfloat v) {
        GLint location = get_uniform_location(name);
        glUniform1f(location, v);
    }
    void uniform2f(GLint location, const glm::vec2& data) {
        glUniform2f(location, data.x, data.y);
    }
    void uniform2f(const char* name, const glm::vec2& data) {
        GLint location = get_uniform_location(name);
        glUniform2f(location, data.x, data.y);
    }
    void uniform3f(int location, float v0, float v1, float v2)
    {
        glUniform3f(location, v0, v1, v2);
    }
    void uniform3f(const char* name, float v0, float v1, float v2)
    {
        GLint location = get_uniform_location(name);
        glUniform3fARB(location, v0, v1, v2);
    }
    void uniform3f(const char* name, const glm::vec3& v)
    {
        GLint location = get_uniform_location(name);
        glUniform3fARB(location, v.x, v.y, v.z);
    }
    void uniform4f(int location, float v0, float v1, float v2, float v3)
    {
        glUniform4f(location, v0, v1, v2, v3);
    }
    void uniform4f(const char* name, float v0, float v1, float v2, float v3)
    {
        GLint location = get_uniform_location(name);
        glUniform4fARB(location, v0, v1, v2, v3);
    }
    void uniform4f(const char* name, const glm::vec3& v)
    {
        GLint location = get_uniform_location(name);
        glUniform4fARB(location, v.x, v.y, v.z, 1);
    }
    void uniformPointLight(const PointLight& v, const std::string& pref = "")
    {
        uniform3f(v.get_pos_name(pref).c_str(), v.position);
        uniform3f(v.get_ambient_name(pref).c_str(), v.ambient);
        uniform3f(v.get_dif_name(pref).c_str(), v.diffuse);
        uniform3f(v.get_spec_name(pref).c_str(), v.specular);
        uniform3f(v.get_atten_name(pref).c_str(), v.attenuation);
    }
    void uniformDirectionLight(const DirectionLight& v, const std::string& pref = "")
    {
        uniform3f(v.get_dir_name(pref).c_str(), v.direction);
        uniform3f(v.get_ambient_name(pref).c_str(), v.ambient);
        uniform3f(v.get_dif_name(pref).c_str(), v.diffuse);
        uniform3f(v.get_spec_name(pref).c_str(), v.specular);
    }
    void uniformFlashLight(const FlashLight& v, const std::string& pref = "")
    {
        uniform3f(v.get_pos_name(pref).c_str(), v.position);
        uniform3f(v.get_ambient_name(pref).c_str(), v.ambient);
        uniform3f(v.get_dif_name(pref).c_str(), v.diffuse);
        uniform3f(v.get_spec_name(pref).c_str(), v.specular);
        uniform3f(v.get_atten_name(pref).c_str(), v.attenuation);
        uniform3f(v.get_dir_name(pref).c_str(), v.direction);
        uniform1f(v.get_cutOff_name(pref).c_str(), v.cutOff);
    }
    void uniformMaterial(const Material& v, const std::string& pref = "")
    {
        uniform1i(v.get_texture_name().c_str(), 0);
        uniform3f(v.get_ambient_name(pref).c_str(), v.ambient);
        uniform3f(v.get_dif_name(pref).c_str(), v.diffuse);
        uniform3f(v.get_spec_name(pref).c_str(), v.specular);
        uniform3f(v.get_emission_name(pref).c_str(), v.emission);
        uniform1f(v.get_shininess_name(pref).c_str(), v.shininess);
    }
    void uniformVec3Array(const std::string& arr_name, const std::vector<glm::vec3>& arr) {
        for (size_t i = 0; i < arr.size(); i++) {
            std::string name = arr_name + "[" + std::to_string(i) + "]";
            GLint location = get_uniform_location(name.c_str());
            uniform3f(location, arr[i].x, arr[i].y, arr[i].z);
        }
    }
    void uniform1iv(const char* name, const int* data, size_t i = 1) {
        GLint loc = get_uniform_location(name);
        glUniform1iv(loc, i, data);
    }
    void uniformMatrix4fv(GLint location, const glm::mat4 mat) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
    void uniformMatrix4fv(const char* name, const float* data, size_t count = 1) {
        GLint location = get_uniform_location(name);
        glUniformMatrix4fv(location, count, GL_FALSE, data);
    }
    void uniform4fv(GLint location, const glm::vec4 data, size_t count = 1) {
        glUniform4fv(location, 1, glm::value_ptr(data));
    }
    void uniform4fv(const char* name, const float* data, size_t count = 1) {
        GLint location = get_uniform_location(name);
        glUniform4fv(location, count, data);
    }
};

