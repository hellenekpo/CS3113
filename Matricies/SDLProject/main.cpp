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

#define LOG(statement) std::cout << statement << '\n'
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
//Our window dimensions
const int WINDOW_WIDTH = 1240;
const int WINDOW_HEIGHT = 900;

const float TRANS_VALUE = 0.025f;

//Background colours
const float BG_RED = 1.0f, BG_BLUE = 0.7f, BG_GREEN = 0.5f;
const float BG_OPACITY = 1.0f;

//Triangle colors
const float TRIANGLE_RED = 0.937f, TRIANGLE_BLUE = 0.082f, TRIANGLE_GREEN = 0.568f;
const float TRIANGLE_OPACITY = 1.0f;

const float TRIANGLE1_RED = 0.070f, TRIANGLE1_BLUE = 0.972f, TRIANGLE1_GREEN = 0.745f;
const float TRIANGLE1_OPACITY = 1.0f;

//Our viewpoint-or out "camera's" position and dimensions

const int VIEWPORT_X = 0;
const int VIEWPORT_Y = 0;
const int VIEWPORT_WIDTH = WINDOW_WIDTH;
const int VIEWPORT_HEIGHT = WINDOW_HEIGHT;

//Our shader filepath; these are necessary for a number of things
//Not least, to actually draw our shapes
//We'll hae a whole lecture on these later
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
const char PLAYER_SPRITE1[] = "pink1.png";
const char PLAYER_SPRITE2[] = "pink2.png";
GLuint player_texture_id;
GLuint player_texture_id1;
const float GROWTH_FACTOR = 2.5f;
const float SHRINK_FACTOR = 0.99f;
const int MAX_FRAME = 40;

const float INIT_TRIANGLE_ANGLE = glm::radians(45.0); //opengl uses radians
const float ROT_ANGLEN = glm::radians(1.5f);
int frame_counter = 0;
bool is_growing = true;

//And some variables
//Old stuff
SDL_Window* display_window;
bool game_is_running = true;

//New stuff
ShaderProgram program;
ShaderProgram program1;

glm::mat4 view_matrix; // Defines the position (location and orientation) of the camera
glm::mat4 model_matrix; // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week
glm::mat4 model_matrix1;
glm::mat4 projection_matrix; // Defines the characteristics of your camera, such as clip panes, field of view, projection method, etc.

const char MAT_SEP = '\t';

float model_x;
float model_y;
float triangle_x = 0.0f;
float triangle_x1 = 0.0f;
float previous_ticks = 0.0f;
float triangle_rotate = 0.0f;
const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0; // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0; // this value MUST be zero

void print_matrix(glm::mat4 &matrix, int size)
{
    for (auto row = 0 ; row < size ; row++)
    {
        for (auto col = 0 ; col < size ; col++)
        {
            std::cout << matrix[row][col] << MAT_SEP;
        }
        
        std::cout << "\n";
    }
}

GLuint load_texture(const char* filepath) {
    //step 1: loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    if (image == NULL) {
        LOG("Unable to laod image. Make sure your filepath is correct");
        assert(false);
    }
    
    //Step 2: generating and bind a texture id
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    //step 3: setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    //STEP 4: releasing our file from memory and returning our texture id
    stbi_image_free(image);
    return textureID;
}

void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    display_window = SDL_CreateWindow("Hellen Ekpo's Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(display_window);
    SDL_GL_MakeCurrent(display_window, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    program.Load(V_SHADER_PATH, F_SHADER_PATH);
    program1.Load(V_SHADER_PATH, F_SHADER_PATH);
    player_texture_id = load_texture(PLAYER_SPRITE2);
    player_texture_id1 = load_texture(PLAYER_SPRITE1);
    //Initialize our view, model, and projection matrices
    view_matrix = glm::mat4(1.0f);
    model_matrix = glm::mat4(1.0f);
    model_matrix1 = glm::mat4(1.0f);
    model_x = 0.1f;
    model_y = 0.0f;
    projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program1.SetViewMatrix(view_matrix);
    program.SetViewMatrix(view_matrix);
    program.SetProjectionMatrix(projection_matrix);
    program1.SetProjectionMatrix(projection_matrix);
    //set the color of the triangle
    program.SetColor(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);
    program1.SetColor(TRIANGLE1_RED, TRIANGLE1_BLUE, TRIANGLE1_GREEN, TRIANGLE1_OPACITY);
    glUseProgram(program.programID);
    
    //Old stuff
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            game_is_running = false;
        }

    }
}
void update()
{
    float ticks = (float) SDL_GetTicks() / 1000.0f;  // get the current number of ticks
       float delta_time = ticks - previous_ticks;       // the delta time is the difference from the last frame
       previous_ticks = ticks;

//    // Initialise our scale_vector and update the number of frames past
//    glm::vec3 scale_vector;
//    frame_counter += 1;
//
//    // Once we reach our limit, we switch directions
//    if (frame_counter >= MAX_FRAME)
//    {
//        is_growing = !is_growing;
//        frame_counter = 0;
//    }
//
//    // Decide if the matrix will be scaled up or scaled down
//    if (is_growing)
//    {
//        scale_vector = glm::vec3(GROWTH_FACTOR, GROWTH_FACTOR, 1.0f);
//    } else {
//        scale_vector = glm::vec3(SHRINK_FACTOR, SHRINK_FACTOR, 1.0f);
//    }
//
//    // Update model matrix
//    model_matrix = glm::scale(model_matrix, scale_vector);
    LOG(++frame_counter);
    //Step 1
    glm::vec3 scale_vector;
    glm::vec3 scale_vector1;
    //Step 2
    if (frame_counter >= MAX_FRAME) {
        is_growing = !is_growing;
        frame_counter = 0;
    }
    //Step 3
    scale_vector = glm::vec3(is_growing ? GROWTH_FACTOR : SHRINK_FACTOR,
                             is_growing ? GROWTH_FACTOR : SHRINK_FACTOR,
                             1.0f);

    
    //Step 4
    scale_vector1 = glm::vec3(GROWTH_FACTOR, GROWTH_FACTOR, 1.0);
    triangle_x += 0.2 * delta_time;
    triangle_rotate += 90.0 * delta_time;
    model_matrix = glm::mat4(1.0f);
    model_matrix1 = glm::mat4(1.0f);
    model_matrix = glm::scale(model_matrix, scale_vector1);
    model_matrix1 = glm::scale(model_matrix1, scale_vector1);
    model_matrix = glm::translate(model_matrix, glm::vec3(triangle_x, 0.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(triangle_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix1 = glm::translate(model_matrix1, glm::vec3(-triangle_x, 0.0f, 0.0f));
    model_matrix1 = glm::rotate(model_matrix1, glm::radians(-triangle_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    //print_matrix(model_matrix, 4);
}
void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id) {
    program.SetModelMatrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    

    //program1.SetModelMatrix(model_matrix1);
    //verticies
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    //texture
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(program.texCoordAttribute);
    program.SetModelMatrix(model_matrix);
    glBindTexture(GL_TEXTURE_2D, player_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    draw_object(model_matrix1, player_texture_id1);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
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
