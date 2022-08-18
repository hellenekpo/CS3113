#include "LevelC.h"
#include "Utility.h"
const char TEXT_PATH2[] = "font1.png";
bool state_game2 = false;
bool win2 = false;
GLuint text_texture_id2;
#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

unsigned int LEVELC_DATA[] =
{
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0,
    6, 6, 6, 2, 0, 0,12,13,14, 0, 0, 12, 14, 0,
    6, 6, 6, 6, 2, 0, 0, 0, 0, 0,12,13, 14, 15,
    6, 6, 6, 6, 6, 2, 0, 0, 0,16, 1, 1 , 2, 15,
    6, 6, 6, 6, 6, 6, 2, 0,16, 4, 5, 5, 6, 15,
    6, 6, 6, 6, 6, 6, 6, 0, 4, 4, 5, 5, 6, 15
};

LevelC::~LevelC()
{
    delete [] this->state.enemies;
    delete    this->state.player;
    delete    this->state.map;
    Mix_FreeChunk(this->state.jump_sfx);
    Mix_FreeMusic(this->state.bgm);
}

void LevelC::initialise()
{
    state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("tile.png");
    this->state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 4.5, 4);
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing

    state.player = new Entity();
    state.player->set_entity_type(PLAYER);
    state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    state.player->set_movement(glm::vec3(0.0f));
    state.player->speed = 2.5f;
    state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    state.player->texture_id = Utility::load_texture("george_0.png");
    
    // Walking
    state.player->walking[state.player->LEFT]  = new int[4] { 1, 5, 9,  13 };
    state.player->walking[state.player->RIGHT] = new int[4] { 3, 7, 11, 15 };
    state.player->walking[state.player->UP]    = new int[4] { 2, 6, 10, 14 };
    state.player->walking[state.player->DOWN]  = new int[4] { 0, 4, 8,  12 };

    state.player->animation_indices = state.player->walking[state.player->RIGHT];  // start George looking left
    state.player->animation_frames = 4;
    state.player->animation_index  = 0;
    state.player->animation_time   = 0.0f;
    state.player->animation_cols   = 4;
    state.player->animation_rows   = 4;
    state.player->set_height(0.8f);
    state.player->set_width(0.8f);
    
    // Jumping
    state.player->jumping_power = 5.0f;
    
    /**
     Enemies' stuff */


    GLuint enemy_texture_id2 = Utility::load_texture("wormenemy.png");
    
    state.enemies = new Entity[this->ENEMY_COUNT];
    state.enemies[0].set_entity_type(ENEMY);
    state.enemies[0].set_ai_type(GUARD);
    state.enemies[0].set_ai_state(IDLE);
    state.enemies[0].texture_id = enemy_texture_id2;
    state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    state.enemies[0].set_movement(glm::vec3(0.0f));
    state.enemies[0].speed = 1.0f;
    state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    text_texture_id2 = Utility::load_texture(TEXT_PATH2);
    
    /**
     BGM and SFX
     */

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    state.bgm = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(state.bgm, -1);
    Mix_VolumeMusic(0.0f);
    
    state.jump_sfx = Mix_LoadWAV("bounce.wav");
}

void LevelC::update(float delta_time) { this->state.player->update(delta_time, state.player, state.enemies, this->ENEMY_COUNT, this->state.map);
    this->state.player->update(delta_time, state.player, state.enemies, this->ENEMY_COUNT, this->state.map);
    this->state.enemies->update(delta_time, state.player, state.player, this->ENEMY_COUNT, this->state.map);
    if (this->state.player->get_position().y < -10.0f) state.next_scene_id = 1;
    if (this->state.player->collided_with_enemy_bottom) {
        win2 = true;
        state_game2 = false;
        this->state.enemies->deactivate();

    }
     if ((this->state.player->collided_with_enemy_right || this->state.enemies->collided_with_player_left)
      ||(this->state.player->collided_with_enemy_left || this->state.enemies->collided_with_player_right ))  {
        state_game2 = true;
        win2 = false;
         lives -= 1;
    }
}
void LevelC::DrawText(ShaderProgram *program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position, int FONTBANK_SIZE)
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

void LevelC::render(ShaderProgram *program)
{
    this->state.map->render(program);
    this->DrawText(program, text_texture_id2, "Lives", .5f, 0.005f, glm::vec3(1.0f, -1.0f, 0.0f), 16);
    std::stringstream stream;
    stream << lives;
    std::string str;
    stream >> str;
    this->DrawText(program, text_texture_id2, str, .5f, 0.005f, glm::vec3(1.0f, -2.0f, 0.0f), 16);
   if (!win2) {
        this->state.enemies->render(program);
    }

        this->state.player->render(program);
    if (state_game2) {
        this->DrawText(program, text_texture_id2, "YOU LOSE", 1.0f, 0.005f, glm::vec3(1.0f, -5.0f, 0.0f), 16);

    }
    if (lives <= 0) {
        this->DrawText(program, text_texture_id2, "YOU LOSE", 1.0f, 0.005f, glm::vec3(1.0f, -5.0f, 0.0f), 16);
    }
    if (win2) {
        state.enemies[0].set_ai_type(GUARD);
        state.enemies[0].set_ai_state(IDLE);
        this->DrawText(program, text_texture_id2, "ENEMY DEFEATED", 0.5f, 0.005f, glm::vec3(1.0f, -5.0f, 0.0f), 16);
        this->DrawText(program, text_texture_id2, "YOU WIN", 1.0f, 0.005f, glm::vec3(1.0f, -6.0f, 0.0f), 16);

    }
}

