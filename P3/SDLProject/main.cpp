#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define PLATFORM_COUNT 4
#define ROCK_COUNT 2
float start_ticks = 0.0f;
bool one = false;
bool win;
bool stop;
float curr_x_accel = 0.0f, curr_y_acceleration = 0.0f;
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <unistd.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <string>

#include <ctime> // time header

float curr_state = 0;
bool switching = true;
/**
 STRUCTS AND ENUMS
 */
struct GameState
{
    Entity* player;
    Entity* platforms;
    Entity* losing_platforms;
};

#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;

/**
 CONSTANTS
 */
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;
float delta_time;
float ticks;
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;
const char SPRITESHEET_FILEPATH[] = "spaceships.png";
const char PLATFORM_FILEPATH[]    = "platform.png";
const char ROCK_PATH[] = "rock.png";
const char TEXT_PATH[] = "font1.png";
GLuint text_texture_id;
const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL  = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER   = 0;   // this value MUST be zero

/**
 VARIABLES
 */
GameState state;

SDL_Window* display_window;
bool game_is_running = true;

ShaderProgram program;
glm::mat4 view_matrix, projection_matrix;

float previous_ticks = 0.0f;
float accumulator = 0.0f;

/**
 GENERAL FUNCTIONS
 */


GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // STEP 3: Setting our texture filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // STEP 4: Setting our texture wrapping modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // the last argument can change depending on what you are looking for
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // STEP 5: Releasing our file from memory and returning our texture id
    stbi_image_free(image);
    
    return textureID;
}

void DrawText(ShaderProgram *program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position, int FONTBANK_SIZE)
{

    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for each character
    // Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
        //    relative to the whole sentence)
        int spritesheet_index = (int) text[i];  // ascii value of character
        float offset = (screen_size + spacing) * i;
        
        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float) (spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float) (spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
        });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
        });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    
    program->SetModelMatrix(model_matrix);
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    display_window = SDL_CreateWindow("Hello, Entities!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(display_window);
    SDL_GL_MakeCurrent(display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    program.Load(V_SHADER_PATH, F_SHADER_PATH);
    
    view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.
    
    program.SetProjectionMatrix(projection_matrix);
    program.SetViewMatrix(view_matrix);
    
    glUseProgram(program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    state.player = new Entity();
    state.player->set_position(glm::vec3(0.0f));
    state.player->set_movement(glm::vec3(0.0f));
    state.player->speed = 1.0f;
    state.player->set_acceleration(glm::vec3(0.0f, -0.01f, 0.0f));
    state.player->texture_id = load_texture(SPRITESHEET_FILEPATH);
    
    state.player->walking[state.player->LEFT]  = new int[4] { 1, 5, 9,  13 };
    state.player->walking[state.player->RIGHT] = new int[4] { 14, 11, 7, 0 };
    state.player->walking[state.player->UP]    = new int[4] { 2, 6, 10, 14 };
    state.player->walking[state.player->DOWN]  = new int[4] { 0, 4, 8,  12 };

    state.player->animation_indices = state.player->walking[state.player->LEFT];  // start George looking left
    state.player->animation_frames = 4;
    state.player->animation_index  = 0;
    state.player->animation_time   = 0.0f;
    state.player->animation_cols   = 7;
    state.player->animation_rows   = 4;
    state.player->set_height(0.80);
    state.player->set_width(0.80);
    
    GLuint platform_texture_id = load_texture(PLATFORM_FILEPATH);
    
    state.platforms = new Entity[PLATFORM_COUNT];
    
    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        state.platforms[i].texture_id = platform_texture_id;
        state.platforms[i].set_position(glm::vec3(i+1 - 1.0f, -3.0f, 0.0f));
        state.platforms[i].update(0.0f, NULL, 0);
    }
    
    GLuint rock_texture_id = load_texture(ROCK_PATH);
    state.losing_platforms = new Entity[ROCK_COUNT];
    for (int i = 0; i < ROCK_COUNT; i++) {
        state.losing_platforms[i].texture_id = rock_texture_id;
        state.losing_platforms[i].set_position(glm::vec3(i-4.0f, -3.0f, 0.0f));
        state.losing_platforms[i].update(0.0f, NULL, 0);
    }
    text_texture_id = load_texture(TEXT_PATH);
    std::cout << "the initial" << text_texture_id << std::endl;
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    state.player->set_movement(glm::vec3(0.0f));

    if (ticks >= start_ticks+1.0f) {
        state.player->set_acceleration(glm::vec3(0.0f, -0.49f, 0.0f));
    }
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            game_is_running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_q:
                    game_is_running = false;
                    break;
                case SDLK_LEFT:
                    curr_x_accel = -50.0f;
                    start_ticks = ticks;
                    state.player->set_acceleration(glm::vec3(curr_x_accel, -0.49f, 0.0f));
                    
                    break;
                case SDLK_RIGHT:
                    curr_x_accel = 50.0f;
                    start_ticks = ticks;
                    state.player->set_acceleration(glm::vec3(curr_x_accel, -0.49f, 0.0f));
                    break;
                case SDLK_UP:
                    state.player->set_acceleration(glm::vec3(curr_x_accel, 2.00f, 0.0f));
                    break;
                default:
                    break;
            }
            
        }
        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                case SDLK_q:
                    game_is_running = false;
                    break;
                case SDLK_LEFT:
                   // state.player->set_acceleration(glm::vec3(0.0f, -0.49f, 0.0f));
                    break;
                case SDLK_RIGHT:
                   // state.player->set_acceleration(glm::vec3(00.0f, -0.49f, 0.0f));
                    break;
                case SDLK_UP:
                  //  state.player->set_acceleration(glm::vec3(curr_x_accel, 0.00f, 0.0f));
                    break;
                default:
                    break;
            }

            
        }

    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        state.player->movement.x = -1.0f;
        state.player->animation_indices = state.player->walking[state.player->LEFT];

    
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        state.player->movement.x = 1.0f;
        state.player->animation_indices = state.player->walking[state.player->RIGHT];
    }
    
    if (key_state[SDL_SCANCODE_UP])
    {
        state.player->movement.y = 1.0f;
        state.player->animation_indices = state.player->walking[state.player->UP];
    }
//    else if (key_state[SDL_SCANCODE_DOWN])
//    {
//        state.player->movement.y = -1.0f;
//        state.player->animation_indices = state.player->walking[state.player->DOWN];
//    }
    
    // This makes sure that the player can't move faster diagonally
    if (glm::length(state.player->movement) > 1.0f)
    {
        state.player->movement = glm::normalize(state.player->movement);
    }
}

void update()
{
    ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    delta_time = ticks - previous_ticks;

    previous_ticks = ticks;
    delta_time += accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        accumulator = delta_time;
        return;
    }
    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        if (state.player->check_collision(&state.platforms[i])) {
        
       std::cout << "MISSION SUCCESSFUL!!!" << std::endl;
            win = true;
        }
        
        
    }
    for (int i = 0; i < ROCK_COUNT; ++i) {
        if (state.player->check_collision(&state.losing_platforms[i])) {
            std::cout << "MISSION FAILED!!" << std::endl;
            win = false;
        }
    }
    if (one) {
        state.player->update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        one = false;
    }
    else {
        state.player->update(FIXED_TIMESTEP, state.losing_platforms, ROCK_COUNT);
        one = true;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        delta_time -= FIXED_TIMESTEP;
    }
    
    accumulator = delta_time;
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    state.player->render(&program);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) state.platforms[i].render(&program);
    for (int i = 0; i < ROCK_COUNT; i++) state.losing_platforms[i].render(&program);
    
    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        if (state.player->check_collision(&state.platforms[i])) {
            DrawText(&program, text_texture_id, "YOU WIN", 1.0f, 0.000005f, glm::vec3(-3.0f, 0.0f, 0.0f), 16);
        }
        
        
    }
    for (int i = 0; i < ROCK_COUNT; ++i) {
        if (state.player->check_collision(&state.losing_platforms[i])) {
            DrawText(&program, text_texture_id, "YOU LOSE", 1.0f, 0.005f, glm::vec3(-3.0f, 0.0f, 0.0f), 16);
        }
    }
    
    SDL_GL_SwapWindow(display_window);
}

void shutdown()
{
    SDL_Quit();
}

/**
 DRIVER GAME LOOP
 */
int main(int argc, char* argv[])
{
    initialise();
    
    
    string hellen = "andre";
    cout << hellen << endl;
    
    while (game_is_running)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
