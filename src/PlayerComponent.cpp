//
// Created by Viktor Hundahl Strate on 15/02/2019.
//

#include "PlayerComponent.h"

#include <glfw/glfw3.h>

#include <iostream>

const float CAMERA_DEFAULT_SPEED = 3.5f;
const float CAMERA_DEFAULT_SENSITIVITY = 0.001f;

PlayerComponent::PlayerComponent(TransformComponent* transform, CameraComponent* camera)
    : transform(transform), camera(camera), MouseSensitivity(CAMERA_DEFAULT_SENSITIVITY), MovementSpeed(CAMERA_DEFAULT_SPEED)
{}

void PlayerComponent::ProcessKeyboard(float forwards, float sideways, float up, float deltaTime)
{

    glm::vec3 direction = glm::vec3(forwards, sideways, up);

    if (direction.x == 0 && direction.y == 0 && direction.z == 0)
        return;

    float velocity = MovementSpeed*deltaTime;

    direction = glm::normalize(direction)*velocity;

    transform->position += -direction.x * camera->front() + direction.y * glm::cross(camera->front(), glm::vec3(0.0f, 1.0f, 0.0f)) + direction.z*glm::vec3(0.0f, 1.0f, 0.0f);
}

void PlayerComponent::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

//    glm::mat4 rotateMat = glm::mat4(1.0f);
    transform->rotation = glm::rotate(transform->rotation, -xoffset, glm::vec3(0.0f, 1.0f, 0.0f));
    transform->rotation = glm::rotate(transform->rotation, yoffset, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::quat pitch = transform->rotation;
    pitch.x = 0;
    pitch.y = 0;
    pitch = glm::normalize(pitch);

    float angle = 2.0f*(glm::acos(pitch.z)-glm::radians(90.0f));

//    float maxAngle = glm::radians(20.0f);
//
//    if (angle > maxAngle)
//        transformComponent->rotation = glm::rotate(transformComponent->rotation, angle-maxAngle, glm::vec3(0.0f, 0.0f, 1.0f));
//
//    if (angle < -maxAngle)
//        transformComponent->rotation = glm::rotate(transformComponent->rotation, angle+maxAngle, glm::vec3(0.0f, 0.0f, 1.0f));
//
    glm::vec3 euler = glm::eulerAngles(transform->rotation);
//
//    euler.y = (float)glfwGetTime() / 1.0f;
//
//    transform->rotation = glm::quat(euler);

    if (constrainPitch) {
        std::cout << "Rotation: " << euler.x * 180.0f/3.14159f << ", " << euler.y * 180.0f/3.14159f << ", " << euler.z * 180.0f/3.14159f << std::endl;
        std::cout << "Pitch: " << pitch.z << " Angle: " << angle << std::endl;
    }
}

void PlayerComponent::update(float dt)
{
//    transform->rotation = glm::rotate(transform->rotation, dt/4.0f, glm::vec3(0.f, 1.f, 0.f));
//    transform->rotation = glm::rotate(transform->rotation, dt, glm::vec3(0.0f, 0.0f, 1.0f));

//    std::cout << euler.y << std::endl;
}

void PlayerComponent::ProcessMouseScroll(float yoffset)
{
    float min = 1.0f;
    float max = 45.0f;

    if (camera->FOV <= max && camera->FOV >= min)
        camera->FOV -= yoffset;

    if (camera->FOV <= min)
        camera->FOV = min;

    if (camera->FOV >= max)
        camera->FOV = max;
}