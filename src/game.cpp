#include "game.hpp"
#include <iostream>
#include <fstream>

Game::Game()
{
    music = LoadMusicStream("Sounds/music.ogg");
    explosionSound = LoadSound("Sounds/explosion.ogg");
    PlayMusicStream(music);
    InitGame();
}

Game::~Game() {
    Alien::UnloadImages();
    UnloadMusicStream(music);
    UnloadSound(explosionSound);
}

void Game::Update() {
    if(run) {
        double currentTime = GetTime();
        
        // Spawn mystery ship if it's time
        if(currentTime - timeLastSpawn > mysteryShipSpawnInterval) {
            mysteryship.Spawn();
            timeLastSpawn = GetTime();
            mysteryShipSpawnInterval = GetRandomValue(10, 20);
        }

        // Update spaceship lasers
        for(auto& laser: spaceship.lasers) {
            laser.Update();
        }

        // Move aliens and alien lasers
        MoveAliens();
        AlienShootLaser();
        for(auto& laser: alienLasers) {
            laser.Update();
        }

        // Clean up inactive lasers
        DeleteInactiveLasers();

        // Update mysteryship
        mysteryship.Update();

        // Check for collisions
        CheckForCollisions();

        // Check if player wins or loses
        if (aliens.empty() && !mysteryship.alive) {
            // All aliens and mysteryship are destroyed, player wins
            run = false; // stop the game
            DrawWinner(); // Show winner message
        } else if (lives <= 0) {
            // Player lost, game over
            run = false;
            DrawGameOver(); // Show game over message
        }
    } else {
        if (IsKeyDown(KEY_ENTER)) {
            Reset(); // Reset the game
            InitGame(); // Reinitialize game state
        }
    }
}

void Game::Draw() {
    // Draw game elements
    spaceship.Draw();
    for(auto& laser: spaceship.lasers) {
        laser.Draw();
    }
    for(auto& obstacle: obstacles) {
        obstacle.Draw();
    }
    for(auto& alien: aliens) {
        alien.Draw();
    }
    for(auto& laser: alienLasers) {
        laser.Draw();
    }
    mysteryship.Draw();

    // If game is over or player wins, display the appropriate message
    if (!run) {
        if (lives <= 0) {
            DrawGameOver();
        } else if (aliens.empty() && !mysteryship.alive) {
            DrawWinner();
        }
    }
}


// Draw the Game Over message
// Draw the Game Over message with text outline
void Game::DrawGameOver() {
    int fontSize = 80;  // Increased font size for better visibility
    int yOffset = GetScreenHeight() / 2 - fontSize;  // Starting y position for "Game Over!"
    
    // Game Over text with outline (shadow effect)
    DrawTextEx(GetFontDefault(), "Game Over!", {GetScreenWidth() / 2 - MeasureText("Game Over!", fontSize) / 2, float(yOffset)}, fontSize, 2, DARKGRAY); // Shadow effect
    DrawTextEx(GetFontDefault(), "Game Over!", {GetScreenWidth() / 2 - MeasureText("Game Over!", fontSize) / 2, float(yOffset)}, fontSize, 2, BLUE); // Actual text

    // Press ENTER to restart
    DrawTextEx(GetFontDefault(), "Press ENTER to restart", {GetScreenWidth() / 2 - MeasureText("Press ENTER to restart", fontSize / 2) / 2, float(yOffset + fontSize + 10)}, fontSize / 2, 2, GREEN);
}

// Draw the Winner message with text outline
void Game::DrawWinner() {
    int fontSize = 80;  // Increased font size for better visibility
    int yOffset = GetScreenHeight() / 2 - fontSize;  // Starting y position for winner message

    // Winner message with outline (shadow effect)
    DrawTextEx(GetFontDefault(), "Winner!", {GetScreenWidth() / 2 - MeasureText("Winner!", fontSize) / 2, float(yOffset)}, fontSize, 2, DARKGRAY); // Shadow effect
    DrawTextEx(GetFontDefault(), "Winner!", {GetScreenWidth() / 2 - MeasureText("Winner!", fontSize) / 2, float(yOffset)}, fontSize, 2, GREEN); // Actual text

    // All enemies destroyed message
    DrawTextEx(GetFontDefault(), "All enemies destroyed!", {GetScreenWidth() / 2 - MeasureText("All enemies destroyed!", fontSize / 2) / 2, float(yOffset + fontSize + 10)}, fontSize / 2, 2, BLUE);
}


void Game::HandleInput() {
    if(run){
        if(IsKeyDown(KEY_LEFT)) {
            spaceship.MoveLeft();
        } else if (IsKeyDown(KEY_RIGHT)){
            spaceship.MoveRight();
        } else if (IsKeyDown(KEY_SPACE)) {
            spaceship.FireLaser();
        }
    }
}

void Game::DeleteInactiveLasers()
{
    for(auto it = spaceship.lasers.begin(); it != spaceship.lasers.end();){
        if(!it -> active) {
            it = spaceship.lasers.erase(it);
        } else {
            ++ it;
        }
    }

    for(auto it = alienLasers.begin(); it != alienLasers.end();){
        if(!it -> active) {
            it = alienLasers.erase(it);
        } else {
            ++ it;
        }
    }
}

std::vector<Obstacle> Game::CreateObstacles()
{
    int obstacleWidth = Obstacle::grid[0].size() * 3;
    float gap = (GetScreenWidth() - (4 * obstacleWidth))/5;

    for(int i = 0; i < 4; i++) {
        float offsetX = (i + 1) * gap + i * obstacleWidth;
        obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight() - 200)}));
    }
    return obstacles;
}

std::vector<Alien> Game::CreateAliens()
{
    std::vector<Alien> aliens;
    for(int row = 0; row < 5; row++) {
        for(int column = 0; column < 11; column++) {

            int alienType;
            if(row == 0) {
                alienType = 3;
            } else if (row == 1 || row == 2) {
                alienType = 2;
            } else {
                alienType = 1;
            }

            float x = 75 + column * 55;
            float y = 110 + row * 55;
            aliens.push_back(Alien(alienType, {x, y}));
        }
    }
    return aliens;
}

void Game::MoveAliens() {
    for(auto& alien: aliens) {
        if(alien.position.x + alien.alienImages[alien.type - 1].width > GetScreenWidth() - 25) {
            aliensDirection = -1;
            MoveDownAliens(4);
        }
        if(alien.position.x < 25) {
            aliensDirection = 1;
            MoveDownAliens(4);
        }

        alien.Update(aliensDirection);
    }
}

void Game::MoveDownAliens(int distance)
{
    for(auto& alien: aliens) {
        alien.position.y += distance;
    }
}

void Game::AlienShootLaser()
{
    double currentTime = GetTime();
    if(currentTime - timeLastAlienFired >= alienLaserShootInterval && !aliens.empty()) {
        int randomIndex = GetRandomValue(0, aliens.size() - 1);
        Alien& alien = aliens[randomIndex];
        alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type -1].width/2, 
                                    alien.position.y + alien.alienImages[alien.type - 1].height}, 6));
        timeLastAlienFired = GetTime();
    }
}

void Game::CheckForCollisions()
{
    //Spaceship Lasers

    for(auto& laser: spaceship.lasers) {
        auto it = aliens.begin();
        while(it != aliens.end()){
            if(CheckCollisionRecs(it -> getRect(), laser.getRect()))
            {
                PlaySound(explosionSound);
                if(it -> type == 1) {
                    score += 100;
                } else if (it -> type == 2) {
                    score += 200;
                } else if(it -> type == 3) {
                    score += 300;
                }
                checkForHighscore();

                it = aliens.erase(it);
                laser.active = false;
            } else {
                ++it;
            }
        }

        for(auto& obstacle: obstacles){
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()){
                if(CheckCollisionRecs(it -> getRect(), laser.getRect())){
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }
        }

        // Check if laser hits the mystery ship
        if(CheckCollisionRecs(mysteryship.getRect(), laser.getRect())){
            mysteryship.alive = false;
            mysteryship.hit = true; // Set hit flag when laser hits mystery ship
            laser.active = false;
            score += 500;
            checkForHighscore();
            PlaySound(explosionSound);
        }
    }

    // Alien Lasers

    for(auto& laser: alienLasers) {
        if(CheckCollisionRecs(laser.getRect(), spaceship.getRect())){
            laser.active = false;
            lives --;
            if(lives == 0) {
                GameOver();
            }
        }

        for(auto& obstacle: obstacles){
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()){
                if(CheckCollisionRecs(it -> getRect(), laser.getRect())){
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }
        }
    }

    // Alien Collision with Obstacle

    for(auto& alien: aliens) {
        for(auto& obstacle: obstacles) {
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()) {
                if(CheckCollisionRecs(it -> getRect(), alien.getRect())) {
                    it = obstacle.blocks.erase(it);
                } else {
                    it ++;
                }
            }
        }

        if(CheckCollisionRecs(alien.getRect(), spaceship.getRect())) {
            GameOver();
        }
    }
}

void Game::GameOver()
{
    run = false;
}

void Game::InitGame()
{
    obstacles = CreateObstacles();
    aliens = CreateAliens();
    aliensDirection = 1;
    timeLastAlienFired = 0.0;
    timeLastSpawn = 0.0;
    lives = 3;
    score = 0;
    highscore = loadHighscoreFromFile();
    run = true;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
}

void Game::checkForHighscore()
{
    if(score > highscore) {
        highscore = score;
        saveHighscoreToFile(highscore);
    }
}

void Game::saveHighscoreToFile(int highscore)
{
    std::ofstream highscoreFile("highscore.txt");
    if(highscoreFile.is_open()) {
        highscoreFile << highscore;
        highscoreFile.close();
    } else {
        std::cerr << "Failed to save highscore to file" << std::endl;
    }
}

int Game::loadHighscoreFromFile() {
    int loadedHighscore = 0;
    std::ifstream highscoreFile("highscore.txt");
    if(highscoreFile.is_open()) {
        highscoreFile >> loadedHighscore;
        highscoreFile.close();
    } else {
        std::cerr << "Failed to load highscore from file." << std::endl;
    }
    return loadedHighscore;
}

void Game::Reset() {
    spaceship.Reset();
    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
}
