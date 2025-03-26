#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ctime>
#include "../common/debugging.h"
#include "../common/shaders.h"

int main(int argc, char** argv) {
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    // Setup OpenGL 4.1 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(512, 512, "code_12_cs_raytracer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Errore durante l'inizializzazione di GLEW\n";
        return -1;
    }

    printout_opengl_glsl_info();

    // Quad full-screen (2D)
    GLuint positionAttribIndex = 0;
    float positions[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    // VAO e VBO
    GLuint positionsBuffer;
    glGenBuffers(1, &positionsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(positionAttribIndex);
    glVertexAttribPointer(positionAttribIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Shader per disegnare la texture
    shader texture_shader;
    texture_shader.create_program("./shaders/texture.vert", "./shaders/texture.frag");
    if (texture_shader.program == 0) {
        std::cerr << "Errore nella compilazione del texture shader" << std::endl;
        return -1;
    }

    // Shader di compute (raytracer)
    shader raytracer;
    raytracer.create_program("./shaders/cs_raytracer.comp");
    if (raytracer.program == 0) {
        std::cerr << "Errore nella compilazione del compute shader" << std::endl;
        return -1;
    }

    // Texture come output del compute shader
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 512, 0, GL_RGBA, GL_FLOAT, NULL);

    // Associa la texture all'image unit 0 per il compute shader
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    check_gl_errors(__LINE__, __FILE__);

    // Set uniform del texture_shader
    glUseProgram(texture_shader.program);
    glUniform1i(texture_shader["uColorImage"], 0);
    
    if (texture_shader["uColorImage"] == -1) {
        std::cerr << "Uniform 'uColorImage' non trovata!" << std::endl;
        return -1;
    }

    // Main loop
    int nf = 0;
    int cstart = clock();

    while (!glfwWindowShouldClose(window)) {
        if (clock() - cstart > CLOCKS_PER_SEC) {
            std::cout << nf << " FPS" << std::endl;
            nf = 0;
            cstart = clock();
        }
        nf++;

        // Esegui il compute shader
        glUseProgram(raytracer.program);
        glDispatchCompute(512 / 32, 512 / 32, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Passa al programma di rendering
        glUseProgram(texture_shader.program);
        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, texture);
        glClearColor(0.0, 0.0, 0.0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}