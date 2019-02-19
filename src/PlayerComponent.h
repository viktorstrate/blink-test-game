//
// Created by Viktor Hundahl Strate on 15/02/2019.
//


#pragma once

#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/DynamicComponent.h"

class PlayerComponent : public DynamicComponent
{
public:
    PlayerComponent();

    float MovementSpeed;
    float MouseSensitivity;

    float yaw, pitch;

    glm::vec3 camFront;

    void updateCameraRotation();

    void moveCamera(float forwards, float sideways, float up, double dt);

    void configure(Entity* entity) override;

    void onInput(GLFWwindow* window, double dt) override;

    void onMouseMove(double xpos, double ypos) override;

    void onMouseScroll(double horizontal, double vertical) override;

private:
    TransformComponent* transform;
    CameraComponent* camera;

    bool firstMouse = true;
    double lastMouseX, lastMouseY;
};

