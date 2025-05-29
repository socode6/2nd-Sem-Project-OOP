#include "mysteryship.hpp"

MysteryShip::MysteryShip()
{
    image = LoadTexture("Graphics/mystery.png");
    alive = false;
    hit = false; // Initialize hit flag to false
}

MysteryShip::~MysteryShip() {
    UnloadTexture(image);
}

void MysteryShip::Spawn() {
    if (hit) return; // Don't spawn if the ship has been hit

    position.y = 90;
    int side = GetRandomValue(0, 1);

    if(side == 0) {
        position.x = 25;
        speed = 3;
    } else {
        position.x = GetScreenWidth() - image.width - 25;
        speed = -3;
    }
    alive = true;
    hit = false; // Reset hit flag when respawning
}

Rectangle MysteryShip::getRect()
{
    if(alive){
        return {position.x, position.y, float(image.width), float(image.height)};
    } else {
        return {position.x, position.y, 0, 0};
    }
}

void MysteryShip::Update() {
    if(alive) {
        position.x += speed;
        if(position.x > GetScreenWidth() - image.width -25 || position.x < 25) {
            alive = false;
        }
    }
}

void MysteryShip::Draw() {
    if(alive) {
        DrawTextureV(image, position, WHITE);
    }
}
