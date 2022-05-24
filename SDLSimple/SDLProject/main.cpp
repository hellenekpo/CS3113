//Getting all the libraries
#include <iostream>
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPE 1
#include <SDL.h>
#include <SDL_opengl.h>

//Constants
const int WINDOWS_WIDTH = 640;
const int WINDOWS_HEIGHT = 480;

const float BG_RED = 0.1922F, BG_BLUE = 0.549f, BG_GREEN = 0.9059f;
const float BG_OPACITY = 1.0f;

//Variables
SDL_Window* display_window;
bool game_is_running = true;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO); //Initializes our video window
    
    display_window = SDL_CreateWindow("Hello OpenGL !",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      WINDOWS_WIDTH,
                                      WINDOWS_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(display_window);
    SDL_GL_MakeCurrent(display_window, context);
#ifdef _WINDOWS
    glewInit();
#endif
    //clearing our screen to our desired color
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    
    //Game loop
    SDL_Event event;
    while (game_is_running) {
        // game will go here
        while (SDL_PollEvent(&event)) {
            //the only type of events we are expecting thus far are simple ones that
            //close the program, obviously this will change soon
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                game_is_running = false;
            }
            // game will go here
        }
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(display_window);
    
    }
    SDL_Quit();
    return 0;
}
