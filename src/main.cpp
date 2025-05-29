#include <raylib.h> 
#include "game.hpp"
#include <string>
#include <cstdlib> // For random star placement
#include <ctime>   // For seeding random generator

// Starfield variables
const int starCount = 100; // Number of stars
Vector2 stars[starCount];

// Initialize starfield
void InitStarfield(int screenWidth, int screenHeight) {
    for (int i = 0; i < starCount; i++) {
        stars[i] = { static_cast<float>(rand() % screenWidth), static_cast<float>(rand() % screenHeight) };
    }
}

// Update starfield (vertical movement)
void UpdateStarfield(int screenWidth, int screenHeight) {
    for (int i = 0; i < starCount; i++) {
        stars[i].y += 1.0f; // Move stars downward
        if (stars[i].y > screenHeight) {
            stars[i].y = 0; // Reset to top
            stars[i].x = static_cast<float>(rand() % screenWidth);
        }
    }
}

// Draw starfield
void DrawStarfield() {
    for (int i = 0; i < starCount; i++) {
        DrawPixelV(stars[i], WHITE);
    }
}

// Function to display the main menu with starfield background
void DisplayMainMenu(int screenWidth, int screenHeight) {
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ENTER)) {
            break; // Exit the main menu to start the game
        }

        // Update starfield for the main menu
        UpdateStarfield(screenWidth, screenHeight);

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw stars
        DrawStarfield();

        // Draw main menu text
        DrawText("SPACE SHOOTER", screenWidth / 2 - MeasureText("SPACE SHOOTER", 60) / 2, screenHeight / 2 - 70, 60, RED);
        DrawText("Press Enter to Continue", screenWidth / 2 - MeasureText("Press Enter to Continue", 20) / 2, screenHeight / 2 + 30, 20, GREEN);

        EndDrawing();
    }
}

std::string FormatWithLeadingZeros(int number, int width) {
    std::string numberText = std::to_string(number);
    int leadingZeros = width - numberText.length();
    return numberText = std::string(leadingZeros, '0') + numberText;
}

int main() {
    // Initialize random seed for starfield
    srand(static_cast<unsigned int>(time(0)));

    // Initialize the window
    int offset = 50;
    int windowWidth = 750;
    int windowHeight = 700;

    InitWindow(windowWidth + offset, windowHeight + 2 * offset, "C++ Space Invaders");

    // Initialize starfield
    InitStarfield(windowWidth + offset, windowHeight + 2 * offset);

    // Display the main menu before starting the game
    DisplayMainMenu(windowWidth + offset, windowHeight + 2 * offset);

    // Old main function code follows
    //Color grey = {29, 29, 27, 255};
    Color yellow = {243, 216, 63, 255};

    InitAudioDevice();

    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);
    Texture2D spaceshipImage = LoadTexture("Graphics/spaceship.png");

    SetTargetFPS(60);

    Game game;

    // Pause state
    bool isPaused = false;

    while (WindowShouldClose() == false) {
        UpdateMusicStream(game.music);
        
        // Toggle pause state when CTRL key is pressed
        if (IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL)) {
            isPaused = !isPaused; // Toggle the pause state
        }

        // Game logic only runs when not paused
        if (!isPaused) {
            game.HandleInput();
            game.Update();
            
            // Update starfield for the game screen
            UpdateStarfield(windowWidth + offset, windowHeight + 2 * offset);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw stars
        DrawStarfield();

        // Draw game elements
        DrawRectangleRoundedLines({10, 10, 780, 780}, 0.18f, 20, 2, yellow);
        DrawLineEx({25, 730}, {775, 730}, 3, yellow);

        if (game.run) {
            DrawTextEx(font, "LEVEL 01", {570, 740}, 34, 2, yellow);
        } else {
            DrawTextEx(font, "GAME OVER", {570, 740}, 34, 2, yellow);
        }

        // Display score and lives
        float x = 50.0;
        for (int i = 0; i < game.lives; i++) {
            DrawTextureV(spaceshipImage, {x, 745}, WHITE);
            x += 50;
        }

        DrawTextEx(font, "SCORE", {50, 15}, 34, 2, yellow);
        std::string scoreText = FormatWithLeadingZeros(game.score, 5);
        DrawTextEx(font, scoreText.c_str(), {50, 40}, 34, 2, yellow);

        DrawTextEx(font, "HIGH-SCORE", {570, 15}, 34, 2, yellow);
        std::string highscoreText = FormatWithLeadingZeros(game.highscore, 5);
        DrawTextEx(font, highscoreText.c_str(), {655, 40}, 34, 2, yellow);

        // Draw the pause message if the game is paused
        if (isPaused) {
            Vector2 pausedPosition = { windowWidth / 2 - MeasureText("PAUSED", 60) / 2, windowHeight / 2 + 30 }; // Slightly lower position
            Color pausedColor = LIME;  // LIME color for visibility
            Color shadowColor = DARKGRAY;  // Shadow for better contrast

            // Draw shadow first for contrast
            DrawText("PAUSED", pausedPosition.x + 2, pausedPosition.y + 2, 60, shadowColor);
            // Draw the "PAUSED" text with a bright color
            DrawText("PAUSED", pausedPosition.x, pausedPosition.y, 60, pausedColor);
        }

        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    CloseAudioDevice();
}
