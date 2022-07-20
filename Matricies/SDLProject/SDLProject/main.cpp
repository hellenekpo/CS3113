/*
#include <iostream>

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const float BG_RED = 0.8922f, BG_BLUE = 0.7654, BG_GREEN = 0.9059f;
const float BG_OPACITY = 1.0f;

SDL_Window* display_window;
bool game_is_running = true;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);  // Initialising
    
    display_window = SDL_CreateWindow("Hello, World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(display_window);

    SDL_GL_MakeCurrent(display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    SDL_Event event;
    while (game_is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                game_is_running = false;
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(display_window);
    }
    
    SDL_Quit();
    return 0;
}
 */



