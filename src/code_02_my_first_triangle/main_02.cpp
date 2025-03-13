#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../common/debugging.h"


int main(int argc, char** argv) {

	GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

/* Set OpenGL context hints */
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(512, 512, "code_02_my_first_triangle", NULL, NULL);


    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window); 

    /* Initialize the gle wrangler*/
    glewInit();

    /* query for the hardware and software specs and print the result on the console*/
    printout_opengl_glsl_info();

    ///* create render data in RAM */
    GLuint positionAttribIndex = 0;
    float positions[] = { 0.0, 0.0,  // 1st vertex
                          0.5, 0.0,  // 2nd vertex
                          0.5, 0.5,
                          0.0, 0.5
    };


    ///* create  a vertex array object */
    GLuint va;
    glGenVertexArrays(1, &va);
    glBindVertexArray(va);

    ///* create a buffer for the render data in video RAM */
    GLuint positionsBuffer;
    glCreateBuffers(1, &positionsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);

    ///* declare what data in RAM are filling the bufferin video RAM */
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(positionAttribIndex);
    ///* specify the data format */
    glVertexAttribPointer(positionAttribIndex, 2, GL_FLOAT, false, 0, 0);

    ///* create render data in RAM */
    GLuint colorAttribIndex = 1;
    float colors[] = {  1.0, 0.0, 0.0,    // 1st vertex's color
                        0.0, 1.0, 0.0,   // 2nd vertex's color
                        0.0, 0.0, 1.0,
                        1.0, 1.0, 1.0
                    };

    ///* create a buffer for the render data in video RAM */
    GLuint colorBuffer;
    glCreateBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);

    ///* declare what data in RAM are filling the bufferin video RAM */
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, colors, GL_STATIC_DRAW);

    glEnableVertexAttribArray(colorAttribIndex);
    ///* specify the data format */
    glVertexAttribPointer(colorAttribIndex, 3, GL_FLOAT, false, 0, 0);

    GLuint indices[] = { 0,1,2,0,2,3 };
    GLuint indexBuffer;
    glCreateBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, indices, GL_STATIC_DRAW);


    ///* create a vertex shader */
    std::string  vertex_shader_src = "#version 460\n \
        in vec2 aPosition;\
        in vec3 aColor;\
        out vec3 vColor;\
        uniform float uDelta;\
        void main(void)\
        {\
         gl_Position = vec4(aPosition+vec2(uDelta,0.0), 0.0, 1.0);\
         vColor = aColor;\
        }\
       ";
    const GLchar* vs_source = (const GLchar*)vertex_shader_src.c_str();
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vs_source, NULL);
    glCompileShader(vertex_shader);
  

    ///* create a fragment shader */
    std::string   fragment_shader_src = "#version 460 \n \
        layout(location = 0) out vec4 color;\
        in vec3 vColor;\
        uniform float uDelta;\
        void main(void)\
        {\
            color = vec4(vColor+vec3(uDelta,0.0,0.0), 1.0);\
        }";
    const GLchar* fs_source = (const GLchar*)fragment_shader_src.c_str();
    
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fs_source, NULL);
    glCompileShader(fragment_shader);
    

    GLuint program_shader = glCreateProgram();
    glAttachShader(program_shader, vertex_shader);
    glAttachShader(program_shader, fragment_shader);
     
    glBindAttribLocation(program_shader, positionAttribIndex, "aPosition");
    glBindAttribLocation(program_shader, colorAttribIndex, "aColor");
    glLinkProgram(program_shader);
   

    GLint linked;
    validate_shader_program(program_shader);
    glGetProgramiv(program_shader, GL_LINK_STATUS, &linked);
    if (linked) {
        glUseProgram(program_shader);
    }
    
    GLint loc = glGetUniformLocation(program_shader, "uDelta");

    /* cal glGetError and print out the result in a more verbose style
    * __LINE__ and __FILE__ are precompiler directive that replace the value with the
    * line and file of this call, so you know where the error happened
    */
    check_gl_errors(__LINE__, __FILE__);

    float d = 0.0001;
    float delta = 0;
    while (!glfwWindowShouldClose(window))
    {
        if (delta < 0 || delta > 0.5)
            d = -d;
        delta += d;

        glUniform1f(loc, delta);

        /* Render here */
        glClearColor(0.2, 0.2, 0.2, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

	return 0;
}