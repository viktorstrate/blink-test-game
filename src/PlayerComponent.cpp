//
// Created by Viktor Hundahl Strate on 15/02/2019.
//

#include "PlayerComponent.h"

#include <glfw/glfw3.h>

#include <iostream>
#include <World.h>

#define WORLD_UP glm::vec3(0.0f, 1.0f, 0.0f)

const float CAMERA_DEFAULT_SPEED = 3.5f;
const float CAMERA_DEFAULT_SENSITIVITY = 0.3f;

PlayerComponent::PlayerComponent(World* world, TransformComponent* transform, CameraComponent* camera, float yaw, float pitch)
    : transform(transform), camera(camera), MouseSensitivity(CAMERA_DEFAULT_SENSITIVITY), MovementSpeed(CAMERA_DEFAULT_SPEED),
      yaw(yaw), pitch(pitch), camFront(0.f), Component(world)
{
    updateCameraRotation();
}

void PlayerComponent::ProcessKeyboard(float forwards, float sideways, float up)
{

    glm::vec3 direction = glm::vec3(forwards, sideways, up);

    if (direction.x == 0 && direction.y == 0 && direction.z == 0)
        return;

    float velocity = MovementSpeed * world->game->deltaTime();

    direction = glm::normalize(direction)*velocity;

    glm::vec3 camForward = camera->front();
    camForward.y = 0;
    camForward = glm::normalize(camForward);

    transform->position += -direction.x * camForward + direction.y * glm::cross(camForward, glm::vec3(0.0f, 1.0f, 0.0f)) + direction.z*glm::vec3(0.0f, 1.0f, 0.0f);
}

void PlayerComponent::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    yaw -= xoffset;
    pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraRotation();
}

void PlayerComponent::update(float dt)
{
//    transform->rotation = glm::rotate(transform->rotation, dt/4.0f, glm::vec3(0.f, 1.f, 0.f));
//    transform->rotation = glm::rotate(transform->rotation, dt, glm::vec3(0.0f, 0.0f, 1.0f));

//    std::cout << euler.y << std::endl;
}

void PlayerComponent::updateCameraRotation()
{
    glm::vec3 pitchVec(0.f);

    pitchVec.x = cos(glm::radians(pitch));
    pitchVec.z = sin(glm::radians(pitch));
    pitchVec = glm::normalize(pitchVec);

    transform->rotation = glm::quat(1.0f, 0.f, 0.f, 0.f);
    transform->rotation = transform->rotation * pitchVec;

    transform->rotation = glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)) * transform->rotation;
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