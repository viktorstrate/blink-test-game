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

PlayerComponent::PlayerComponent()
    : MouseSensitivity(CAMERA_DEFAULT_SENSITIVITY), MovementSpeed(CAMERA_DEFAULT_SPEED),
      yaw(0.0f), pitch(0.0f), camFront(0.f), DynamicComponent()
{}

void PlayerComponent::configure(Entity* entity)
{
    Component::configure(entity);
    transform = entity->get<TransformComponent>();
    camera = entity->get<CameraComponent>();
    updateCameraRotation();
}

void PlayerComponent::onInput(GLFWwindow* window, double dt)
{
    float forwards = 0, sideways = 0, up = 0;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        forwards += -1;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        forwards += 1;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        sideways += -1;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        sideways += 1;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        up += -1;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        up += 1;

    moveCamera(forwards, sideways, up, dt);
}

void PlayerComponent::onMouseMove(double xpos, double ypos)
{
    if (firstMouse) {
        firstMouse = false;
        lastMouseX = xpos;
        lastMouseY = ypos;
    }

    float xoffset = (xpos - lastMouseX);
    float yoffset = (lastMouseY - ypos); // reversed since y-coordinates range from bottom to top
    lastMouseX = (float) xpos;
    lastMouseY = (float) ypos;

    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    yaw -= xoffset;
    pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraRotation();
}

void PlayerComponent::onMouseScroll(double horizontal, double vertical)
{
    float min = 1.0f;
    float max = 45.0f;

    if (camera->FOV <= max && camera->FOV >= min)
        camera->FOV -= vertical;

    if (camera->FOV <= min)
        camera->FOV = min;

    if (camera->FOV >= max)
        camera->FOV = max;
}

void PlayerComponent::moveCamera(float forwards, float sideways, float up, double dt)
{

    glm::vec3 direction = glm::vec3(forwards, sideways, up);

    if (direction.x == 0 && direction.y == 0 && direction.z == 0)
        return;

    auto velocity = static_cast<float>(MovementSpeed*dt);

    direction = glm::normalize(direction)*velocity;

    glm::vec3 camForward = camera->front(transform);
    camForward.y = 0;
    camForward = glm::normalize(camForward);

    transform->position += -direction.x * camForward + direction.y * glm::cross(camForward, glm::vec3(0.0f, 1.0f, 0.0f)) + direction.z*glm::vec3(0.0f, 1.0f, 0.0f);
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