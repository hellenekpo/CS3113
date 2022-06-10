//The old stuff
#include <iostream>
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>

//The new stuff
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

//Our window dimensions
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

//Background colours
const float BG_RED = 1.0f, BG_BLUE = 0.7f, BG_GREEN = 0.5f;
const float BG_OPACITY = 1.0f;

//Triangle colors
const float TRIANGLE_RED = 0.937f, TRIANGLE_BLUE = 0.082f, TRIANGLE_GREEN = 0.568f;
const float TRIANGLE_OPACITY = 1.0f;

//Our viewpoint-or out "camera's" position and dimensions

const int VIEWPORT_X = 0;
const int VIEWPORT_Y = 0;
const int VIEWPORT_WIDTH = WINDOW_WIDTH;
const int VIEWPORT_HEIGHT = WINDOW_HEIGHT;

//Our shader filepath; these are necessary for a number of things
//Not least, to actually draw our shapes
//We'll hae a whole lecture on these later
const char V_SHADER_PATH[] = "shaders/vertex.glsl";
const char F_SHADER_PATH[] = "shaders/fragment.glsl";

//And some variables
//Old stuff
SDL_Window* display_window;
bool game_is_running = true;

//New stuff
ShaderProgram program;

glm::mat4 view_matrix; // Defines the position (location and orientation) of the camera
glm::mat4 model_matrix; // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week
glm::mat4 projection_matrix; // Defines the characteristics of your camera, such as clip panes, field of view, projection method, etc.

void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    display_window = SDL_CreateWindow("Hellen Ekpo's Triangle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(display_window);
    SDL_GL_MakeCurrent(display_window, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    program.Load(V_SHADER_PATH, F_SHADER_PATH);
    //Initialize our view, model, and projection matrices
    view_matrix = glm::mat4(1.0f);
    model_matrix = glm::mat4(1.0f);
    projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    program.SetViewMatrix(view_matrix);
    program.SetProjectionMatrix(projection_matrix);
    //set the color of the triangle
    program.SetColor(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);
    glUseProgram(program.programID);
    
    //Old stuff
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}
void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            game_is_running = false;
        }

    }
}
void update() {
    
    
}
void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetModelMatrix(model_matrix);
    float vertices[] = {
        0.5f, -0.5f,
        0.0f, 0.5f,
        -0.5f, -0.5f
    };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(program.positionAttribute);
    
    SDL_GL_SwapWindow(display_window);
}
void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    //Part 1
    initialize();
    while (game_is_running) {
        process_input();
        update();
        render();
    }
    shutdown();
    return 0;
}
