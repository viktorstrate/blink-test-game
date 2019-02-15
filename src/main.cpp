//
// Created by Viktor Hundahl Strate on 2019-02-03.
//

#include "graphics/Rendering.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <stb_image.h>

#include "Game.h"
#include "graphics/Shader.h"
#include "components/CameraComponent.h"
#include "Model.h"
#include "graphics/VertexBuffer.h"
#include "graphics/VertexArray.h"
#include "GameObject.h"
#include "PlayerComponent.h"

GameObject* player;

bool firstMouse = true;
float lastMouseX = 0, lastMouseY = 0;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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

    player->getComponent<PlayerComponent>()->ProcessKeyboard(forwards, sideways, up, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    if (firstMouse) {
        firstMouse = false;
        lastMouseX = xpos;
        lastMouseY = ypos;
    }

    float xoffset = (float) (xpos - lastMouseX);
    float yoffset = (float) (lastMouseY - ypos); // reversed since y-coordinates range from bottom to top
    lastMouseX = (float) xpos;
    lastMouseY = (float) ypos;

    player->getComponent<PlayerComponent>()->ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    player->getComponent<PlayerComponent>()->ProcessMouseScroll(yoffset);
}

glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
};

glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
};

int main(int argc, char** argv)
{
//    auto* camera = new GameObject();

    auto* trans = new TransformComponent();
    trans->position = glm::vec3(0.0f, 0.0f, 5.0f);
    trans->rotation = glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f));
//    trans->rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

    auto* cam = new CameraComponent(trans);

    auto* plComp = new PlayerComponent(trans, cam);

//    camera->addComponent(camTrans);
//    camera->addComponent(new CameraComponent(camera->getComponent<TransformComponent>()));

    player = new GameObject();
    player->addComponent(trans);
    player->addComponent(cam);
    player->addComponent(plComp);

    Game game("Test Game", 800, 600);

    glfwSetInputMode(game.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(game.window, mouse_callback);
    glfwSetScrollCallback(game.window, scroll_callback);

    Shader program ("assets/shaders/textured-flat.glsl");
    program.use();

    program.setInt("texture1", 0);
    program.setInt("texture2", 1);

    // Verticies that make up a cube
    float cubeVertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
            // Front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            // Left face
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // Right face
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // Top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left
    };

    float screenQuadVertices[] = {
            // Top left
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,

            // Bottom right
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
    };

    Texture diffuseTexture = Texture("assets/textures/container2.png");
    Texture specularTexture = Texture("assets/textures/container2_specular.png");

    Model character = Model("assets/models/nanosuit/nanosuit.obj");

    Shader lampShader("assets/shaders/lamp.glsl");
    Shader lightingShader("assets/shaders/color-light.glsl");
    Shader screenShader("assets/shaders/screen-texture.glsl");

    screenShader.use();
    screenShader.setInt("textureId", 0);

    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    VertexArray quadVAO;
    VertexBuffer quadVBO(screenQuadVertices, sizeof(screenQuadVertices), 4*sizeof(float));


    VertexAttribute quadAttrs[] = {
            VertexAttribute(2, GL_FLOAT, 0),
            VertexAttribute(2, GL_FLOAT, 2*sizeof(float))
    };

    quadVAO.addBuffer(quadVBO, quadAttrs, 2);

    VertexArray lampVAO;

    VertexBuffer VBO(cubeVertices, sizeof(cubeVertices), 8*sizeof(float));

    VertexAttribute attributes[]{
            VertexAttribute(3, GL_FLOAT, 0),
            VertexAttribute(3, GL_FLOAT, 3*sizeof(float)),
            VertexAttribute(3, GL_FLOAT, 6*sizeof(float))
    };

    lampVAO.addBuffer(VBO, attributes, 3);

    // Setup cube vertex array
    VertexArray cubeVAO;
    cubeVAO.addBuffer(VBO, attributes, 3);

    // Unbind vertex buffer and vertex array
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int FBO;

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // generate texture
    Texture texColorBuffer(game.screenWidth, game.screenHeight, GL_RGB);
    texColorBuffer.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer.ID, 0);

    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, game.screenWidth, game.screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Wireframe mode
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    int frameCount = 0;
    float fpsTime = 0;

    while (game.update()) {

        frameCount++;

        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fpsTime += deltaTime;

        if (frameCount%100 == 0) {
            glfwSetWindowTitle(game.window, std::string(
                    "LearnOpenGl - FPS: " + std::to_string((int) (1.0f/(fpsTime/100.0f)))).c_str());
            fpsTime = 0;
        }

        processInput(game.window);

        player->getComponent<PlayerComponent>()->update(deltaTime);

        glm::mat4 view = player->getComponent<CameraComponent>()->GetViewMatrix();

        glm::mat4 projection = player->getComponent<CameraComponent>()->getProjectionMatrix((float)game.screenWidth/(float)game.screenHeight);

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Draw lamp
        lampShader.use();

        lampShader.setMat4("view", view);
        lampShader.setMat4("projection", projection);

        for (int i = 0; i < 4; i++) {

            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));

            lampShader.setMat4("model", model);

            lampVAO.use();
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Draw cubes
        lightingShader.use();

        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);
        lightingShader.setVec3("viewPos", player->getComponent<TransformComponent>()->position);

        diffuseTexture.use(0);
        specularTexture.use(1);

        lightingShader.setFloat("material.shininess", 48.0f);

        lightingShader.setVec3("dirLight.direction", 0.2f, -2.0f, -0.2f);
        lightingShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
        lightingShader.setVec3("dirLight.specular", 0.f, 0.f, 0.f);

        for (int i = 0; i < 4; i++) {
            std::string base = "pointLights[" + std::to_string(i) + "]";
            lightingShader.setVec3(base + ".position", pointLightPositions[i]);
            lightingShader.setVec3(base + ".ambient", 0.05f, 0.05f, 0.05f);
            lightingShader.setVec3(base + ".diffuse", 0.8f, 0.7f, 0.6f);
            lightingShader.setVec3(base + ".specular", 1.0f, 1.0f, 1.0f);
            lightingShader.setFloat(base + ".constant", 1.0f);
            lightingShader.setFloat(base + ".linear", 0.09);
            lightingShader.setFloat(base + ".quadratic", 0.032);
        }

        lightingShader.setVec3("spotLight.position", player->getComponent<TransformComponent>()->position);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09);
        lightingShader.setFloat("spotLight.quadratic", 0.032);

        lightingShader.setVec3("spotLight.direction", player->getComponent<CameraComponent>()->front());
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.0f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        lampVAO.use();

        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
//            float angle = 20.0f*i;

//            model = glm::rotate(model, (float) glfwGetTime() * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, (float) glfwGetTime(), glm::vec3(0.0f, 0.f, 1.f));

            lightingShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Draw character
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.1f));
        lightingShader.setMat4("model", model);

        character.Draw(lightingShader);

        // Draw framebuffer texture to quad
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        quadVAO.use();
        glDisable(GL_DEPTH_TEST);
        texColorBuffer.use(0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glfwTerminate();
    return 0;
}