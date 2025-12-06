#include <iostream>
#include <raylib.h>
#include <cmath>

using namespace std;

int player_score = 0;
int cpu_score = 0;
const int WINNING_SCORE = 5;

enum GameState {
    START_SCREEN,
    PLAYING,
    INTERMISSION,
    GAME_OVER
};

GameState current_state = START_SCREEN;

class Ball{
    public:
    float x, y;
    float speed_x, speed_y; // Changed to float for more precise speed control
    int radius;

    const float INITIAL_SPEED = 7.0f;
    const float SPEED_INCREASE_FACTOR = 0.2f; // How much speed increases per hit
    
  

void Draw() {
    DrawCircle(x, y, radius, WHITE);
}

void Update() {
    x += speed_x;
    y += speed_y; 

    if(y + radius >= GetScreenHeight() || y - radius <= 0) {
        speed_y *= -1;
    }

    if(x + radius >= GetScreenWidth()) { //Cpu wins
        cpu_score++;
        ResetBall();
        if (cpu_score >= WINNING_SCORE){
            current_state = GAME_OVER;
        }
        else {
            current_state = INTERMISSION;
        }
    }
    
    if(x - radius <= 0) { //player wins
        player_score++;
        ResetBall();
        if (player_score >= WINNING_SCORE) {
            current_state = GAME_OVER;
        }
        else {
            current_state = INTERMISSION;
        }
    }
}
void IncreaseSpeed() {
    // Increase the magnitude of speed_x and speed_y
    if (speed_x > 0) {
        speed_x += SPEED_INCREASE_FACTOR;
    } else {
        speed_x -= SPEED_INCREASE_FACTOR;
    }

    if (speed_y > 0) {
        speed_y += SPEED_INCREASE_FACTOR;
    } else {
        speed_y -= SPEED_INCREASE_FACTOR;
    }
}

void ResetBall() {
    x = GetScreenWidth()/2;
    y = GetScreenHeight()/2;

    // Reset speeds to initial value
    speed_x = INITIAL_SPEED;
    speed_y = INITIAL_SPEED;

    int speed_choices[2] = {-1, 1};
    speed_x *= speed_choices[GetRandomValue(0,1)];
    speed_y *= speed_choices[GetRandomValue(0,1)];
}
};

class Paddle {
protected:
void LimitMovement() {
     if(y <= 0){
        y = 0;
    }
    if(y + height >= GetScreenHeight()) {
        y = GetScreenHeight() - height;
    }
}
    public:
    float x, y;
    float width, height;
    float speed;

    // Paddle Speed Constants
    const float INITIAL_PADDLE_SPEED = 6.0f;
    const float PADDLE_SPEED_INCREASE = 0.4f; // Paddle speed increases by 0.3 on every hit

void Draw() {
     DrawRectangle(x, y, width, height, WHITE);

}

void Update() {
    if(IsKeyDown(KEY_UP)) {
        y = y - speed;
    }
    if(IsKeyDown(KEY_DOWN)) {
        y = y + speed;
    }

   LimitMovement();
}
//paddle increase speed
void IncreaseSpeed() {
    speed += PADDLE_SPEED_INCREASE;
}
void ResetSpeed() {
    speed = INITIAL_PADDLE_SPEED;
}

};

class CpuPaddle: public Paddle{
    public:

void Update(int ball_y) {

    if(y + height/2 > ball_y) {
        y = y - speed;
    }
    if(y + height/2 <= ball_y) {
        y = y + speed;
    }
    LimitMovement();
}
};

Ball ball;
Paddle player;
CpuPaddle cpu;

// --- Function Prototypes ---
void DrawBackground();
void DrawScreens();
void GameUpdate();
void GameDraw();
void GameSetup(int screen_width, int screen_height);
void ResetSpeedsAndPositions(); // NEW helper to reset speeds and positions after score
void FullGameReset(); // Used for START_SCREEN and GAME_OVER

int main () {

    const int screen_width = 1280;
    const int screen_height = 850; 
    InitWindow(screen_width, screen_height, "Pong Game!");
    SetTargetFPS(60);

   GameSetup(screen_width, screen_height);

    while(WindowShouldClose() == false) {
        BeginDrawing();
        
        switch (current_state) {
            case START_SCREEN:
                DrawScreens();
                break;
            case INTERMISSION:
                GameUpdate();
                GameDraw(); //pause updates
                break;
            case PLAYING:
                GameUpdate();
                GameDraw();
                break;
            case GAME_OVER:
                DrawScreens();
                break;
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

// --- Function Definitions ---

void GameSetup(int screen_width, int screen_height) {
    ball.radius = 20;
    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    
    FullGameReset(); // Set initial positions and speeds
}

void ResetSpeedsAndPositions() {
    // Reset positions
    ball.x = GetScreenWidth()/2;
    ball.y = GetScreenHeight()/2;
    player.y = GetScreenHeight()/2 - player.height/2; 
    cpu.y = GetScreenHeight()/2 - cpu.height/2;

    // Reset speeds
    ball.ResetBall(); 
    player.ResetSpeed();
    cpu.ResetSpeed();
}

void FullGameReset() {
    player_score = 0;
    cpu_score = 0;
    ResetSpeedsAndPositions();
}

void DrawBackground() {
    const int screen_width = GetScreenWidth();
    const int screen_height = GetScreenHeight();
    
    // Left side: Dark Blue
    DrawRectangle(0, 0, screen_width / 2, screen_height, DARKBLUE);
    
    // Right side: Light Blue
    DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, SKYBLUE);

    // Center Line
    DrawLine(screen_width/2, 0, screen_width / 2, screen_height, WHITE);
}

void GameUpdate() {
    // Updating
    ball.Update();
    player.Update();
    cpu.Update(ball.y);

    // checking collisions
    if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height})) {
        ball.speed_x *= -1;
        ball.IncreaseSpeed(); 
        player.IncreaseSpeed(); // Player paddle speed increases
    }

    if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width,cpu.height})) {
        ball.speed_x *= -1;
        ball.IncreaseSpeed(); 
        cpu.IncreaseSpeed(); // CPU paddle speed increases
    }
}

void GameDraw() {
    const int screen_width = GetScreenWidth();
    
    DrawBackground();

    // Drawing Game Objects
    ball.Draw();
    cpu.Draw();
    player.Draw();
    
    // Draw Scores 
    DrawText(TextFormat("%i", cpu_score), screen_width/4 - 20, 20, 80, WHITE);
    DrawText(TextFormat("%i", player_score), 3 * screen_width/4 - 20, 20, 80, BLACK);
}

void DrawScreens() {
    const int screen_width = GetScreenWidth();
    const int screen_height = GetScreenHeight();
    const int FONT_SIZE = 80;
    const int SMALL_FONT = 40;
    
    if (current_state == START_SCREEN) {
        ClearBackground(BLACK);
        const char* title = "PONG Game!";
        const char* instruction = "Press [SPACE] to Start!";
        
        int title_width = MeasureText(title, FONT_SIZE);
        int instruction_width = MeasureText(instruction, SMALL_FONT);

        DrawText(title, screen_width/2 - title_width/2, screen_height/3, FONT_SIZE, YELLOW);
        DrawText(instruction, screen_width/2 - instruction_width/2, screen_height/3 + FONT_SIZE + 40, SMALL_FONT, WHITE);

        if (IsKeyPressed(KEY_SPACE)) {
            FullGameReset();
            current_state = PLAYING;
        }

    } else if (current_state == INTERMISSION) {
        const char* instruction = "Point Scored! Press [SPACE] to Continue";
        const char* current_score = TextFormat("Current Score: %i - %i", cpu_score, player_score);

        int instruction_width = MeasureText(instruction, SMALL_FONT);
        int score_width = MeasureText(current_score, FONT_SIZE);

        // Draw a translucent overlay to highlight the pause
        DrawRectangle(0, 0, screen_width, screen_height, Fade(BLACK, 0.7f));
        
        // Draw centered score
        DrawText(current_score, screen_width/2 - score_width/2, screen_height/2 - FONT_SIZE, FONT_SIZE, WHITE);
        
        // Draw continue instruction
        DrawText(instruction, screen_width/2 - instruction_width/2, screen_height/2 + FONT_SIZE/2, SMALL_FONT, YELLOW);

        // Reset speeds and positions when the intermission starts (and every time the screen is drawn)
        ResetSpeedsAndPositions();

        if (IsKeyPressed(KEY_SPACE)) {
            current_state = PLAYING;
        }

    } else if (current_state == GAME_OVER) {
        ClearBackground(BLACK);
        const char* winner_text;
        Color winner_color;

        if (player_score >= WINNING_SCORE) {
            winner_text = "PLAYER WINS!";
            winner_color = GREEN;
        } else {
            winner_text = "CPU WINS!";
            winner_color = RED;
        }
        
        const char* final_score = TextFormat("Final Score: %i - %i", cpu_score, player_score);
        const char* instruction = "Press [SPACE] to Play Again";
        
        int winner_width = MeasureText(winner_text, FONT_SIZE);
        int score_width = MeasureText(final_score, SMALL_FONT);
        int instruction_width = MeasureText(instruction, SMALL_FONT);

        DrawText(winner_text, screen_width/2 - winner_width/2, screen_height/3, FONT_SIZE, winner_color);
        DrawText(final_score, screen_width/2 - score_width/2, screen_height/3 + FONT_SIZE + 20, SMALL_FONT, WHITE);
        DrawText(instruction, screen_width/2 - instruction_width/2, screen_height/3 + FONT_SIZE + 100, SMALL_FONT, WHITE);
        
        if (IsKeyPressed(KEY_SPACE)) {
            FullGameReset();
            current_state = PLAYING;
        }
    }
}