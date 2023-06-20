#pragma once

// Std. Includes
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    LEFT_ROTATE,
    RIGHT_ROTATE,
    UP_ROTATE,
    DOWN_ROTATE
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 0.5f;

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    GLfloat Yaw;
    GLfloat Pitch;

    GLfloat MovementSpeed;
    GLfloat RotateSpeed = 1.0;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED)
    {
        this->Position = position;
        this->WorldUp = up;
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED)
    {
        this->Position = glm::vec3(posX, posY, posZ);
        this->WorldUp = glm::vec3(upX, upY, upZ);
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    void ProcessKeyboard(Camera_Movement direction)
    {
        std::cout << "Yaw=" << Yaw << "; Pitch=" << Pitch << "\n";
        GLfloat side_speed = this->MovementSpeed;// / 10;
        GLfloat velocity = this->MovementSpeed * 2;
        if (direction == FORWARD)
            this->Position += this->Front * velocity;
        if (direction == BACKWARD)
            this->Position -= this->Front * velocity;
        if (direction == LEFT)
            this->Position -= this->Right * side_speed;
        if (direction == RIGHT)
            this->Position += this->Right * side_speed;
        if (direction == UP)
            this->Position += this->Up * side_speed;
        if (direction == DOWN)
            this->Position -= this->Up * side_speed;
        if (direction == LEFT_ROTATE) {
            this->Yaw += RotateSpeed;
            updateCameraVectors();
        }
        if (direction == RIGHT_ROTATE) {
            this->Yaw -= RotateSpeed;
            updateCameraVectors();
        }
        if (direction == UP_ROTATE) {
            this->Pitch = glm::clamp(this->Pitch + RotateSpeed, -89.f, 89.f);
            updateCameraVectors();
        }
        if (direction == DOWN_ROTATE) {
            this->Pitch = glm::clamp(this->Pitch - RotateSpeed, -89.f, 89.f);
            updateCameraVectors();
        }
    }

    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = glm::normalize(front);
        
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }
};