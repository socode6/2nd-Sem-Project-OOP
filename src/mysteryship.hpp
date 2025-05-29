#pragma once
#include <raylib.h>

class MysteryShip {
    public:
        MysteryShip();
        ~MysteryShip();
        void Update();
        void Draw();
        void Spawn();
        Rectangle getRect();
        bool alive;
        bool hit; // Flag to track if the mystery ship has been hit
    private:
        Vector2 position;
        Texture2D image;
        int speed;
};
