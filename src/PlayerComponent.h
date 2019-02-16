//
// Created by Viktor Hundahl Strate on 15/02/2019.
//


#pragma once

#include <components/CameraComponent.h>
#include "Component.h"

class PlayerComponent : public Component
{
public:
    PlayerComponent(TransformComponent* transform, CameraComponent* camera, float yaw, float pitch);

    float MovementSpeed;
    float MouseSensitivity;

    float yaw, pitch;

    glm::vec3 camFront, camRight, camUp;

    void update(float dt) override;

    void updateCameraRotation();

    void ProcessKeyboard(float forwards, float sideways, float up, float deltaTime);

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    void ProcessMouseScroll(float yoffset);

private:
    CameraComponent* camera;
    TransformComponent* transform;
};

