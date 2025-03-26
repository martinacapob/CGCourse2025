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

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Imposta contesto OpenGL 4.1 core (necessario su macOS)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Crea finestra
    window = glfwCreateWindow(512, 512, "code_11_fsq_raytracer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inizializza GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Errore durante l'inizializzazione di GLEW\n";
        return -1;
    }

    // Stampa info su OpenGL/GLSL
    printout_opengl_glsl_info();

    /// --- Creazione geometria: un full-screen quad in NDC ---
    GLuint positionAttribIndex = 0;
    float positions[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    // Crea VAO
    GLuint va;
    glGenVertexArrays(1, &va);
    glBindVertexArray(va);

    // Crea VBO
    GLuint positionsBuffer;
    glGenBuffers(1, &positionsBuffer); // ✅ Compatibile con OpenGL 4.1
    glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    // Specifica formato attributo
    glEnableVertexAttribArray(positionAttribIndex);
    glVertexAttribPointer(positionAttribIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);

    /// --- Carica gli shader ---
    shader raytracer;
    raytracer.create_program("./shaders/basic.vert", "./shaders/raytracer.frag");

    if (raytracer.program == 0) {
        std::cerr << "Errore nella creazione dello shader program" << std::endl;
        return -1;
    }

    check_gl_errors(__LINE__, __FILE__);

    glUseProgram(raytracer.program);

    /// --- Main loop ---
    int nf = 0;
    int cstart = clock();
    while (!glfwWindowShouldClose(window)) {
        if (clock() - cstart > CLOCKS_PER_SEC) {
            std::cout << nf << " FPS" << std::endl;
            nf = 0;
            cstart = clock();
        }
        nf++;

        // Clear screen
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

       glfwSwapBuffers(window);

              glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}