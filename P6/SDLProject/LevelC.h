

#include "Scene.h"

class LevelC : public Scene {
public:
    int ENEMY_COUNT = 2;
    int points;
    int time2 = 800;
    ~LevelC();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    void DrawText(ShaderProgram *program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position, int FONTBANK_SIZE);
};

