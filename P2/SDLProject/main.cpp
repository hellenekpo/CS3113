#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>

#define LOG(argument) std::cout << argument << '\n'

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

float triangle_x = 0.0f;
float triangle_rotate = 0.0f;
const float GROWTH_FACTOR = 2.5f;
const float SHRINK_FACTOR = 0.99f;
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

const char PLAYER_SPRITE_FILEPATH[] = "pinkbar.png";
const char BALL_SPRITE_FILEPATH[] = "pink1.png";

const int MAX_FRAME = 40;
const float INIT_TRIANGLE_ANGLE = glm::radians(45.0); //opengl uses radians
const float ROT_ANGLEN = glm::radians(1.5f);
int frame_counter = 0;
bool is_growing = true;

SDL_Window* display_window;
bool game_is_running = true;

ShaderProgram program;
glm::mat4 view_matrix, model_matrix, projection_matrix, trans_matrix, other_model_matrix, ball_matrix;

float previous_ticks = 0.0f;

GLuint player_texture_id;
GLuint other_texture_id;
GLuint ball_texture_id;

glm::vec3 player_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 player_movement = glm::vec3(-4.75f, 0.0f, 0.0f);

glm::vec3 other_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 other_movement = glm::vec3(4.75f, 0.0f, 0.0f);
//glm::vec3 other_movement = glm::vec3(1.0f, 1.0f, 0.0f);
//to start the ping ball moving

glm::vec3 player_orientation = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 player_rotation    = glm::vec3(0.0f, 0.0f, 0.0f);

float player_speed = 1.0f;  // move 1 unit per second
float other_player_speed = 1.0f;

#define LOG(argument) std::cout << argument << '\n'

const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL  = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER   = 0;   // this value MUST be zero

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
    
    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);
    
    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    display_window = SDL_CreateWindow("Hello, Textures!",
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
    ball_matrix = glm::mat4(1.0f);
    model_matrix = glm::mat4(1.0f);
    other_model_matrix = glm::mat4(1.0f);
    other_model_matrix = glm::translate(other_model_matrix, glm::vec3(4.5f, 1.0f, 0.0f));
    //to start the movement
    other_position += other_movement;
    player_position += player_movement;
    
    view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.
    
    program.SetProjectionMatrix(projection_matrix);
    program.SetViewMatrix(view_matrix);
    
    glUseProgram(program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    player_texture_id = load_texture(PLAYER_SPRITE_FILEPATH);
    other_texture_id = load_texture(PLAYER_SPRITE_FILEPATH);
    ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    player_movement = glm::vec3(0.0f);
    other_movement = glm::vec3(0.0f);
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        //player_movement.x = 1.0f;
                        //other_movement.x = 1.0f;
                        break;
                        
                    case SDLK_q:
                        // Quit the game with a keystroke
                        game_is_running = false;
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    if (key_state[SDL_SCANCODE_UP])
    {
        if (other_position.y < 2.753) {
            other_movement.y = 1.0f;
        }
        else {
            other_movement.y = 0.0f;
        }
    
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        if (other_position.y > -2.753) {
            other_movement.y = -1.0f;
        }
        else {
            other_movement.y = 0.0f;
        }
    }
    
    if (key_state[SDL_SCANCODE_A] && player_position.y < 2.753) {
        if (player_position.y < 2.753) {
            player_movement.y = 1.0f;
        }
        else {
            player_movement.y = 0.0f;
        }
        std::cout << "player movement" << player_position.y << std::endl;
    }
    
    else if (key_state[SDL_SCANCODE_Z] && player_position.y > -2.753) {
        if (player_position.y > -2.753) {
            player_movement.y = -1.0f;
        }
        else {
            player_movement.y = 0.0f;
        }
        std::cout << "player movement" << player_position.y << std::endl;
    }
    
    // This makes sure that the player can't move faster diagonally
    if (glm::length(player_movement) > 1.0f)
    {
        player_movement = glm::normalize(player_movement);
    }
    if (glm::length(other_movement) > 1.0f) {
        other_movement = glm::normalize(other_movement);
    }
}


void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - previous_ticks; // the delta time is the difference from the last frame
    previous_ticks = ticks;

    // Add direction * units per second * elapsed time
    
    player_position += player_movement * player_speed * delta_time;
    other_position += other_movement * other_player_speed * delta_time;
    
    model_matrix = glm::mat4(1.0f);
    other_model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, player_position);
    other_model_matrix = glm::translate(other_model_matrix, other_position);
    
    //Step 2
    if (is_growing) {
        triangle_x += 0.9 * delta_time;
        triangle_rotate -= 90.0 * delta_time;
    }
    else {
        triangle_x -= 0.2 * delta_time;
        triangle_rotate += 90.0 * delta_time;
    }
    if (triangle_x >= 1.9 || triangle_x <= 0) {
        is_growing = !is_growing;
    }
    //Step 3

    
    //Step 4
    // if (other_position.x-0.3 <= ball_position.x <= other_position.x) {
    //     if (other_position.y-0.3 <= ball_position.y <= other_position.y+0.3) {
    //         ball_matrix = glm::translate(ball_matrix, glm::vec3(-triangle_x, 0.0f, 0.0f));
    //     }
    // }
    ball_matrix = glm::mat4(1.0f);
    ball_matrix = glm::translate(ball_matrix, glm::vec3(triangle_x, 0.0f, 0.0f));
    ball_matrix = glm::rotate(ball_matrix, glm::radians(triangle_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    ball_matrix = glm::rotate(ball_matrix, glm::radians(-triangle_rotate), glm::vec3(0.0f, 0.0f, 1.0f));

}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    program.SetModelMatrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Vertices
    float vertices[] = {
        -0.2f, -1.0f, 0.2f, -1.0f, 0.2f, 1.0f,  // triangle 1
        -0.2f, -1.0f, 0.2f, 1.0f, -0.2f, 1.0f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    // Bind texture
    draw_object(model_matrix, player_texture_id); // first paddle
    draw_object(other_model_matrix, other_texture_id); // second paddle
    draw_object(ball_matrix, ball_texture_id); // ping pong ball
    // We disable two attribute arrays now
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    SDL_GL_SwapWindow(display_window);
}

void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();
    
    while (game_is_running)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
