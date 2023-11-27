// Autor: Marko Erdelji

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>   
#include <GLFW/glfw3.h>
#include "TestBed.h"
#include <iostream>
#include <glm/glm.hpp>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

unsigned int compileShader(GLenum type, const char* source); 

unsigned int createShader(const char* vsSource, const char* fsSource);

void renderTachometer(unsigned int tachometerShader, unsigned int textureShader,unsigned int tachometerPointsShader, unsigned int rpmTexture, unsigned int tachometerVAO, unsigned int halfCircleVAO, unsigned int pointVAO, const Car& car)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rpmTexture);

    glUseProgram(textureShader);
    glBindVertexArray(halfCircleVAO);

    int halfCircleTransformLocation = glGetUniformLocation(textureShader, "transform");
    glUniform2f(halfCircleTransformLocation, -0.5, -0.80);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 101);

    glUseProgram(tachometerPointsShader);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glBindVertexArray(pointVAO);
    int colorLocation = glGetUniformLocation(tachometerPointsShader, "color");
    int linesTransformLocation = glGetUniformLocation(tachometerPointsShader, "transform");
    glUniform2f(linesTransformLocation, -0.5, -0.80);
    glUniform4f(colorLocation, 0.8f,0.0f,0.0f,1.0f);

    glDrawArrays(GL_POINTS, 0, 5);

    glUseProgram(tachometerShader);

    glBindVertexArray(tachometerVAO);

    float tachometerValue = car.getTachometer();

    float normalizedTachometer = tachometerValue / 4000.0f * 180.0f;

    float rotationAngle = glm::radians(std::min(normalizedTachometer, 180.0f));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.5, -0.80, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, -rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));

    int modelLocation = glGetUniformLocation(tachometerShader, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

    float time = glfwGetTime();

    float red = sin(time) * 0.5f + 0.5f;
    float green = cos(time) * 0.5f + 0.5f;
    float blue = 0.0f;  

    int needleColorLocation = glGetUniformLocation(tachometerShader, "needleColor");
    glUniform4f(needleColorLocation, red, green, blue,1.0);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}



void renderProgressBar(unsigned int progressBarShader, unsigned int progressBarVAO, const Car& car)
{
    glUseProgram(progressBarShader);
    glBindVertexArray(progressBarVAO);

    float maxDistance = 1.0f;
    float distance = car.getOdometer();

    int numRectanglesToFill = static_cast<int>(fmod(distance, maxDistance) / maxDistance * 11);

    int colorLocation = glGetUniformLocation(progressBarShader, "rectColor");
    int rectLocation = glGetUniformLocation(progressBarShader, "rectPos");


    for (int i = 0; i < 10; ++i)
    {
        float color[4] = { 0.0, 0.0, 0.0, 1.0 };
        float pos[2] = { 0,i * 0.1 };
        if (i < numRectanglesToFill)
        {
            float colorFactor = static_cast<float>(i) / 10.0f;

            color[0] = colorFactor;    
            color[1] = 1.0 - colorFactor;
            color[2] = 0.0;              
        }

        glUniform4fv(colorLocation, 1, color);
        glUniform2fv(rectLocation, 1, pos);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

void renderIndicators(unsigned int colorTransformShader, unsigned int vaoE, unsigned int vaoB, const Car& car)
{
    glUseProgram(colorTransformShader);

    float indicatorSpacing = 0.1f;  
    float eTranslationX = -0.5f + 0.2 + indicatorSpacing;
    float aTranslationX = eTranslationX + indicatorSpacing;

    int colorLocation = glGetUniformLocation(colorTransformShader, "color");
    int transformLocation = glGetUniformLocation(colorTransformShader, "transform");

    glLineWidth(7);
    glBindVertexArray(vaoE);
    if (car.getCheckEngineLight())
        glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f); 
    else
        glUniform4f(colorLocation, 0.5f, 0.0f, 0.0f, 1.0f); 
    glUniform2f(transformLocation, eTranslationX, -0.80);
    glDrawArrays(GL_LINE_STRIP, 0, 7);

    glBindVertexArray(vaoB);
    if (car.getBatteryProblemLight())
        glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f); 
    else
        glUniform4f(colorLocation, 0.5f, 0.0f, 0.0f, 1.0f);
    glUniform2f(transformLocation, aTranslationX, -0.80);
    glDrawArrays(GL_LINE_STRIP, 0, 11);
}

double lastColorChangeTime = 0.0;
bool isBlack = true;
void renderCircleIndicator(unsigned int circleIndicatorShader, unsigned int circleVAO, const Car& car)
{
    glUseProgram(circleIndicatorShader);

    float indicatorSpacing = 0.1f;
    float translationX = -0.5f + 0.6;

    int colorLocation = glGetUniformLocation(circleIndicatorShader, "color");
    int transformLocation = glGetUniformLocation(circleIndicatorShader, "transform");

    glBindVertexArray(circleVAO);
    int currentGear = car.getGear();

    float flickerInterval = 1.0f / static_cast<float>(currentGear*2);

    if (currentGear == 0) {
        glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    double currentTime = glfwGetTime();
    if (currentTime - lastColorChangeTime >= flickerInterval && currentGear>0) {
        lastColorChangeTime = currentTime;
        if (isBlack) {
            isBlack = false;
            glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f); 
        }
        else {
            isBlack = true;
            glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
        }
    }

    glUniform2f(transformLocation, translationX, -0.77);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 101);

    glBindVertexArray(0);
}

void renderBat(unsigned int shader, unsigned int batVAO, float* batVertices, float batX, float batY, float batTransparency, const Car& car)
{
    glUseProgram(shader);
    glBindVertexArray(batVAO);

    int translationLocation = glGetUniformLocation(shader, "transform");
    glUniform2f(translationLocation, batX, batY);

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);  

    if (car.getGear() == 1 || car.getGear() == 0)
    {
        glUniform4f(glGetUniformLocation(shader, "color"), 0.0f, 0.0f, 0.0f, batTransparency);
        glDrawArrays(GL_POINTS, 0, 12); 
    }
    else if (car.getGear() == 2)
    {
        glUniform4f(glGetUniformLocation(shader, "color"), 0.0f, 0.0f, 0.0f, batTransparency);
        glDrawArrays(GL_LINE_STRIP, 0, 12);
    }
    else
    {
        glUniform4f(glGetUniformLocation(shader, "color"), 0.0f, 0.0f, 0.0f, batTransparency);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
    }

    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE); 

    glBindVertexArray(0);
    glUseProgram(0);
}


void renderStudentInfo(unsigned int studentInfoVAO, unsigned int studentTexture, unsigned int shaderProgram)
{
    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);
    float translationX = 0.5;
    float translationY = -0.72;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, studentTexture);
    glUniform2f(glGetUniformLocation(shaderProgram, "transform"), translationX, translationY);

    glBindVertexArray(studentInfoVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



void renderInteriorAndWindow(unsigned int colorShader,unsigned int textureShader, unsigned int interiorVAO,unsigned int interiorCornerVao,unsigned int windowVAO, unsigned int interiorTexture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, interiorTexture);

    glUseProgram(textureShader);

    glBindVertexArray(interiorVAO);
    glUniform2f(glGetUniformLocation(textureShader, "transform"), 0, -0);


    glDrawArrays(GL_TRIANGLE_FAN, 0, 5); 
    glBindVertexArray(0);

    glUseProgram(colorShader);
    glUniform4f(glGetUniformLocation(colorShader, "color"), 0.53f, 0.81f, 0.98f, 1.0f);
    glBindVertexArray(windowVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glUniform4f(glGetUniformLocation(colorShader, "color"), 0.05f, 0.05f, 0.05f, 1.0f);
    glBindVertexArray(interiorCornerVao);
    glDrawArrays(GL_LINES, 0, 8);

    glBindVertexArray(0);
    glUseProgram(0);
}

int main(void)
{
    Car car = getCar();

    if (!glfwInit()) 
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window; 
    unsigned int wWidth = mode->width;
    unsigned int wHeight = mode->height;
    const char wTitle[] = "Batmobile";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL); 
    if (window == NULL) 
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate(); 
        return 2; 
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowMonitor(window, primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);

    if (glewInit() != GLEW_OK) 
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    float tachometerVertices[] = {
    -0.02f, 0.0f,   
    0.0f, 0.2f,     
    0.02f, 0.0f    
    };

    float progressBarVertices[] =
    {
        0.9, -0.5,  
        0.95, -0.5,  
        0.95, -0.4,  
        0.9, -0.4   
    };
    unsigned int progressBarIndices[] = {
        0, 1, 2, 
        2, 3, 0  
    };

    float halfCircleVertices[101 * 4];

    for (int i = 0; i <= 100; ++i) {
        float angle = (M_PI * i) / 100;
        float x = 0.2f * cos(angle);
        float y = 0.2f * sin(angle);

        float s = (cos(angle) + 1.0f) * 0.5f;
        float t = (sin(angle) + 1.0f) * 0.5f;

        halfCircleVertices[i * 4] = x;
        halfCircleVertices[i * 4 + 1] = y;
        halfCircleVertices[i * 4 + 2] = s;
        halfCircleVertices[i * 4 + 3] = t;
    }

    const int numRpmPoints = 5;
    float pointVertices[numRpmPoints * 2];

    float radius = 0.2f;

    for (int i = 0; i < numRpmPoints; ++i) {
        float rpm = i * 1000.0f;
        float angle = (rpm / 4000.0f) * M_PI;

        if (i == 0) {
            angle = 0.0f;
        }

        float x = radius * cos(angle);
        float y = radius * sin(angle);

        pointVertices[i * 2] = x;
        pointVertices[i * 2 + 1] = y;
    }

    unsigned int pointVAO, pointVBO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertices), pointVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int stride = (2 + 4) * sizeof(float);
    unsigned int rpmMeterTexture;
    glGenTextures(1, &rpmMeterTexture);
    glBindTexture(GL_TEXTURE_2D, rpmMeterTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("tachometer.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    unsigned int tachometerVAO;
    glGenVertexArrays(1, &tachometerVAO);
    glBindVertexArray(tachometerVAO);

    unsigned int tachometerVBO;
    glGenBuffers(1, &tachometerVBO);
    glBindBuffer(GL_ARRAY_BUFFER, tachometerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tachometerVertices), tachometerVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int tachometerShader = createShader("tachometer.vert", "tachometer.frag");


    unsigned int progressBarVAO;
    glGenVertexArrays(1, &progressBarVAO);
    glBindVertexArray(progressBarVAO);

    unsigned int progressBarVBO;
    glGenBuffers(1, &progressBarVBO);
    glBindBuffer(GL_ARRAY_BUFFER, progressBarVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(progressBarVertices), progressBarVertices, GL_STATIC_DRAW);

    unsigned int progressBarEBO;
    glGenBuffers(1, &progressBarEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, progressBarEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(progressBarIndices), progressBarIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int progressBarShader = createShader("progressBar.vert", "progressBar.frag");

    unsigned int halfCircleVAO, halfCircleVBO;
    glGenVertexArrays(1, &halfCircleVAO);
    glGenBuffers(1, &halfCircleVBO);
    glBindVertexArray(halfCircleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, halfCircleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(halfCircleVertices), halfCircleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float verticesE[] = {
     0.06f, 0.0f,     
     0.04f, 0.0f,    
     0.04f, 0.03f,    
     0.06f, 0.03f,    
     0.04f, 0.03f,    
     0.04f, 0.06f,    
     0.06f, 0.06f     
    };

    float verticesB[] = {
        0.0f, 0.0f,
        0.0f, 0.03f,
        0.0f, 0.06f,
        0.02f, 0.06f,
        0.02f, 0.037f,
        0.015f, 0.03f,
        0.0f, 0.03f,
        0.015f, 0.03f,
        0.02f, 0.023f,
        0.02f,0.00f,
        0.0f, 0.0f
    };
    unsigned int vaoE, vboE, vaoB, vboB;

    glGenVertexArrays(1, &vaoE);
    glGenBuffers(1, &vboE);

    glBindVertexArray(vaoE);
    glBindBuffer(GL_ARRAY_BUFFER, vboE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesE), verticesE, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &vaoB);
    glGenBuffers(1, &vboB);

    glBindVertexArray(vaoB);
    glBindBuffer(GL_ARRAY_BUFFER, vboB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesB), verticesB, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int colorTransformShader = createShader("color_transform.vert", "color_transform.frag");
    unsigned int circleIndicatorShader = createShader("circleIndicatorShader.vert", "circleIndicatorShader.frag");
    unsigned int tachometerPointsShader = createShader("tachometer_points.vert", "tachometer_points.frag");

float interiorVertices[] = {
    -1.0f,  1.0f,    0.0f, 0.0f,  
    -1.0f, -1.0f,    0.0f, 1.0f,  
     1.0f, -1.0f,    1.0f, 1.0f,  
     1.0f,  1.0f,    1.0f, 0.0f,  
    -1.0f,  1.0f,    0.0f, 0.0f,  
};




    unsigned int interiorVAO, interiorVBO;
    glGenVertexArrays(1, &interiorVAO);
    glGenBuffers(1, &interiorVBO);

    glBindVertexArray(interiorVAO);

    glBindBuffer(GL_ARRAY_BUFFER, interiorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interiorVertices), interiorVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    unsigned int interiorTexture;
    glGenTextures(1, &interiorTexture);
    glBindTexture(GL_TEXTURE_2D, interiorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(false);
    data = stbi_load("batmobile_interior.jpg", &width, &height, &nrChannels, 3);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    unsigned int textureShader = createShader("texture.vert", "texture.frag");


    float interiorCornerVertices[] = {
    -1.0f, 1.0f,
    -0.6f, 0.6f,

    1.0f, 1.0f,
    0.6f, 0.6f,

    1.0f, -1.0f,
    0.8f, -0.4f,

    -1.0f, -1.0f,
    -0.8f, -0.4f
    };


    unsigned int interiorCornerVAO, interiorCornerVBO;
    glGenVertexArrays(1, &interiorCornerVAO);
    glGenBuffers(1, &interiorCornerVBO);

    glBindVertexArray(interiorCornerVAO);

    glBindBuffer(GL_ARRAY_BUFFER, interiorCornerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interiorCornerVertices), interiorCornerVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    float windowVertices[] = {
     -0.6f, 0.6f,
     0.6f, 0.6f,
     0.8f, -0.4f,
     -0.8f, -0.4f
    };;


    unsigned int windowVAO, windowVBO;
    glGenVertexArrays(1, &windowVAO);
    glGenBuffers(1, &windowVBO);

    glBindVertexArray(windowVAO);

    glBindBuffer(GL_ARRAY_BUFFER, windowVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), windowVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    unsigned int colorShader = createShader("color.vert", "color.frag");

    const int numCircleSegments = 100;
    float circleVertices[(numCircleSegments + 1) * 2];

    float aspectRatio = static_cast<float>(mode->width) / static_cast<float>(mode->height);
    for (int i = 0; i <= numCircleSegments; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(numCircleSegments);
        float x = (0.03 * cosf(theta))/ aspectRatio;
        float y = 0.03 * sinf(theta);

        circleVertices[i * 2] = x;
        circleVertices[i * 2 + 1] = y;
    }


    unsigned int circleVAO, circleVBO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    float batVertices[] = {
        0.0f, -0.05f,    
        -0.05f, 0.00f,
        -0.03f, 0.03f,
        -0.015f, 0.03f,
        -0.015f, 0.06f,
        -0.005f, 0.05f,
         0.005f, 0.05f,
         0.015f, 0.06f,
         0.015f, 0.03f,
        0.03f, 0.03f,
        0.05f, 0.00f,
        0.0f, -0.05f,

    };
    unsigned int batVAO, batVBO;

    glGenVertexArrays(1, &batVAO);
    glGenBuffers(1, &batVBO);

    glBindVertexArray(batVAO);
    glBindBuffer(GL_ARRAY_BUFFER, batVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(batVertices), batVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float studentInfoVertices[] = {
        0.2f,  0.2f,     1.0f, 1.0f, 
        0.2f, -0.2f,     1.0f, 0.0f, 
       -0.2f, -0.2f,     0.0f, 0.0f,
       -0.2f,  0.2f,     0.0f, 1.0f 
    };

    unsigned int studentInfoIndices[] = {
        0, 1, 3, 
        1, 2, 3  
    };

    unsigned int studentInfoVAO, studentInfoVBO, studentInfoEBO;
    glGenVertexArrays(1, &studentInfoVAO);
    glGenBuffers(1, &studentInfoVBO);
    glGenBuffers(1, &studentInfoEBO);

    glBindVertexArray(studentInfoVAO);

    glBindBuffer(GL_ARRAY_BUFFER, studentInfoVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(studentInfoVertices), studentInfoVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, studentInfoEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(studentInfoIndices), studentInfoIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int studentTexture;
    glGenTextures(1, &studentTexture);
    glBindTexture(GL_TEXTURE_2D, studentTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("studentIndex.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    startSimulation(&car);

    float batSpeed = 0.01f;
    float batXPosition = 0.0f;
    float batYPosition = 0.0f;
    float batTransparency = 1.0f;
    while (!glfwWindowShouldClose(window)) 
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            car.setCheckEngine(true);
        }
        else
        {
            car.setCheckEngine(false);
        }

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            car.setBatteryLight(true);
        }
        else
        {
            car.setBatteryLight(false);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            batYPosition += batSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            batYPosition -= batSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            batXPosition -= batSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            batXPosition += batSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            batTransparency = 0.3f;
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            batTransparency = 1.0f;
        }
        float halfBatWidth = 0.1f;
        float halfBatHeight = 0.1f;
        float trapezoidTop = 0.6f;
        float trapezoidBottom = -0.4f;

        float trapezoidLeft = -0.8f + (batYPosition - trapezoidBottom) / (trapezoidTop - trapezoidBottom) * 0.25f;
        float trapezoidRight = 0.8f - (batYPosition - trapezoidBottom) / (trapezoidTop - trapezoidBottom) * 0.25f;

        if (batYPosition + halfBatHeight > trapezoidTop) {
            batYPosition = trapezoidTop - halfBatHeight;
        }
        else if (batYPosition - halfBatHeight < trapezoidBottom) {
            batYPosition = trapezoidBottom + halfBatHeight;
        }

        if (batXPosition - halfBatWidth < trapezoidLeft) {
            batXPosition = trapezoidLeft + halfBatWidth;
        }
        if (batXPosition + halfBatWidth > trapezoidRight) {
            batXPosition = trapezoidRight - halfBatWidth;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        
        renderInteriorAndWindow(colorShader, textureShader, interiorVAO, interiorCornerVAO, windowVAO,interiorTexture);
        renderProgressBar(progressBarShader, progressBarVAO, car);
        renderTachometer(tachometerShader, textureShader,tachometerPointsShader,rpmMeterTexture, tachometerVAO, halfCircleVAO, pointVAO,car);
        renderIndicators(colorTransformShader, vaoE, vaoB, car);
        renderCircleIndicator(circleIndicatorShader, circleVAO, car);
        renderBat(colorTransformShader, batVAO, batVertices,batXPosition,batYPosition,batTransparency,car);
        renderStudentInfo(studentInfoVAO, studentTexture, textureShader);
        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    endSimulation(&car);
    glDeleteBuffers(1, &tachometerVBO);
    glDeleteVertexArrays(1, &tachometerVAO);
    glDeleteProgram(tachometerShader);

    glDeleteBuffers(1, &progressBarVBO);
    glDeleteBuffers(1, &progressBarEBO);
    glDeleteVertexArrays(1, &progressBarVAO);
    glDeleteProgram(progressBarShader);

    glDeleteBuffers(1, &halfCircleVBO);
    glDeleteVertexArrays(1, &halfCircleVAO);

    glDeleteBuffers(1, &vboE);
    glDeleteVertexArrays(1, &vaoE);
    glDeleteBuffers(1, &vboB);
    glDeleteVertexArrays(1, &vaoB);
    glDeleteProgram(colorTransformShader);

    glDeleteTextures(1, &interiorTexture);
    glDeleteBuffers(1, &interiorVBO);
    glDeleteVertexArrays(1, &interiorVAO);
    glDeleteProgram(textureShader);

    glDeleteBuffers(1, &interiorCornerVBO);
    glDeleteVertexArrays(1, &interiorCornerVAO);

    glDeleteBuffers(1, &windowVBO);
    glDeleteVertexArrays(1, &windowVAO);
    glDeleteProgram(colorShader);

    glDeleteBuffers(1, &circleVBO);
    glDeleteVertexArrays(1, &circleVAO);

    glDeleteBuffers(1, &batVBO);
    glDeleteVertexArrays(1, &batVAO);

    glDeleteBuffers(1, &studentInfoVBO);
    glDeleteBuffers(1, &studentInfoEBO);
    glDeleteVertexArrays(1, &studentInfoVAO);
    glDeleteTextures(1, &studentTexture);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
