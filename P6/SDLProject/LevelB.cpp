#include "LevelB.h"
#include "Utility.h"
GLuint text_texture_id1;
const char TEXT_PATH1[] = "font1.png";
bool state_game1 = false;
bool win1 = false;
#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8
#include <sstream>

unsigned int LEVELB_DATA[] =
{
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,16, 7,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,16, 4, 7,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0,16, 4, 4, 7,
    6, 0, 0, 0, 0, 0, 0, 0, 0,16, 4, 4, 4, 7,
    6,16, 1, 1, 1, 1, 2, 0,16, 4, 4, 4, 4, 7,
    6, 4, 5, 5, 5, 5, 6, 0, 4, 4, 4, 4, 4, 7
};

LevelB::~LevelB()
{
    delete [] this->state.enemies;
    delete    this->state.player;
    delete    this->state.map;
    Mix_FreeChunk(this->state.jump_sfx);
    Mix_FreeMusic(this->state.bgm);
}

void LevelB::initialise()
{
    state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("tile.png");
    this->state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 4.5, 4);
    
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
    GLuint enemy_texture_id = Utility::load_texture("zombie.png");
    
    state.enemies = new Entity[this->ENEMY_COUNT];
    state.enemies[0].set_entity_type(ENEMY);
    state.enemies[0].set_ai_type(GUARD);
    state.enemies[0].set_ai_state(IDLE);
    state.enemies[0].texture_id = enemy_texture_id;
    state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    state.enemies[0].set_movement(glm::vec3(0.0f));
    state.enemies[0].speed = 1.0f;
    state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    state.enemies[1].set_entity_type(ENEMY);
    state.enemies[1].set_ai_type(GUARD);
    state.enemies[1].set_ai_state(IDLE);
    state.enemies[1].texture_id = enemy_texture_id;
    state.enemies[1].set_position(glm::vec3(7.0f, 0.0f, 0.0f));
    state.enemies[1].set_movement(glm::vec3(0.0f));
    state.enemies[1].speed = 1.0f;
    state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    state.enemies[2].set_entity_type(ENEMY);
    state.enemies[2].set_ai_type(GUARD);
    state.enemies[2].set_ai_state(IDLE);
    state.enemies[2].texture_id = enemy_texture_id;
    state.enemies[2].set_position(glm::vec3(9.0f, 0.0f, 0.0f));
    state.enemies[2].set_movement(glm::vec3(0.0f));
    state.enemies[2].speed = 1.0f;
    state.enemies[2].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    state.enemies[3].set_entity_type(ENEMY);
    state.enemies[3].set_ai_type(GUARD);
    state.enemies[3].set_ai_state(IDLE);
    state.enemies[3].texture_id = enemy_texture_id;
    state.enemies[3].set_position(glm::vec3(10.0f, 0.0f, 0.0f));
    state.enemies[3].set_movement(glm::vec3(0.0f));
    state.enemies[3].speed = 1.0f;
    state.enemies[3].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    text_texture_id1 = Utility::load_texture(TEXT_PATH1);
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    state.bgm = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(state.bgm, -1);
    Mix_VolumeMusic(0.0f);
    
    state.jump_sfx = Mix_LoadWAV("bounce.wav");
}

void LevelB::update(float delta_time) {
    time2 = time2 - delta_time;
    this->state.player->update(delta_time, state.player, state.enemies, this->ENEMY_COUNT, this->state.map);
    this->state.enemies[0].update(delta_time, state.player, state.player, this->ENEMY_COUNT, this->state.map);
    this->state.enemies[1].update(delta_time, state.player, state.player, this->ENEMY_COUNT, this->state.map);
    this->state.enemies[2].update(delta_time, state.player, state.player, this->ENEMY_COUNT, this->state.map);
    this->state.enemies[3].update(delta_time, state.player, state.player, this->ENEMY_COUNT, this->state.map);
    if (this->state.player->get_position().y < -10.0f) state.next_scene_id = 1;
    if (this->state.player->collided_with_enemy_bottom) {
        points += 3;
        std::cout << this->state.player->check_collision_y(this->state.enemies, 4) << std::endl;
        if (this->state.player->check_collision_y(this->state.enemies, 4) == 1) {
            this->state.enemies[1].deactivate();
        }
        else if (this->state.player->check_collision_y(this->state.enemies, 4) == 2) {
            this->state.enemies[2].deactivate();
        }
        else if (this->state.player->check_collision_y(this->state.enemies, 4) == 3) {
            this->state.enemies[3].deactivate();
        }
        else if (this->state.player->check_collision_y(this->state.enemies, 4) == 0) {
            this->state.enemies[0].deactivate();
        }

    }
}

void LevelB::DrawText(ShaderProgram *program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position, int FONTBANK_SIZE)
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

void LevelB::render(ShaderProgram *program)
{
    
    this->state.map->render(program);
    this->state.enemies[0].render(program);
    this->state.enemies[1].render(program);
    this->state.enemies[2].render(program);
    this->state.enemies[3].render(program);
    this->DrawText(program, text_texture_id1, "Points", .5f, 0.005f, glm::vec3(1.0f, -0.5f, 0.0f), 16);
    this->DrawText(program, text_texture_id1, "Time", .5f, 0.005f, glm::vec3(3.0f, -0.5f, 0.0f), 16);
    std::stringstream stream;
    std::stringstream stream1;
    stream << points;
    stream1 << time2;
    std::string str;
    std::string str1;
    stream >> str;
    stream1 >> str1;
    this->DrawText(program, text_texture_id1, str, .5f, 0.005f, glm::vec3(1.0f, -1.5f, 0.0f), 16);
    this->DrawText(program, text_texture_id1, str1, .5f, 0.005f, glm::vec3(3.0f, -1.5f, 0.0f), 16);

    if (time2 <= 0 && !win1) {
        this->DrawText(program, text_texture_id1, "YOU LOSE", 1.0f, 0.005f, glm::vec3(1.0f, -5.0f, 0.0f), 16);
    }

         this->state.player->render(program);
        //this->DrawText(program, text_texture_id, "YOU LOSE", 1.0f, 0.005f, glm::vec3(1.0f, -5.0f, 0.0f), 16);
    if (!state.enemies[0].is_active && !state.enemies[1].is_active && !state.enemies[2].is_active && !state.enemies[3].is_active && time2 > 0) {
        win1 = true;
    }
    if (win1) {
        this->DrawText(program, text_texture_id1, "ENEMIES DEFEATED", 0.5f, 0.005f, glm::vec3(1.0f, -5.0f, 0.0f), 16);
        this->DrawText(program, text_texture_id1, "MOVE TO THE RIGHT FOR LEVEL 3", 0.5f, 0.005f, glm::vec3(1.0f, -6.0f, 0.0f), 16);

    }

}
