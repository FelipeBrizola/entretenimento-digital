/*
 *  PlayState.h
 *  Example gameplay: ball-hits-sheep, with physics...
 *
 *  Created by Marcelo Cohen on R0515.
 *  Copyright 2015 PUCRS. All rights reserved.
 *
 */

#ifndef PLAY_BALL_PHYS_H_
#define PLAY_BALL_PHYS_H_

#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"
#include <tmx/MapLoader.h>
#include "Physics.h"

class PlayState : public cgf::GameState
{
public:

    void init();
    void cleanup();

    void pause();
    void resume();

    void handleEvents(cgf::Game* game);
    void update(cgf::Game* game);
    void draw(cgf::Game* game);

    void setup(cgf::Game* game);

    // Implement Singleton Pattern
    static PlayState* instance()
    {
        return &m_PlayState;
    }

protected:

    PlayState() {}

private:

    static PlayState m_PlayState;

    int x, y;
    float speed;
    sf::RenderWindow* screen;
    cgf::InputManager* im;
    tmx::MapLoader* map;
    cgf::Sprite ball;
    cgf::Sprite sheep;
    cgf::Sprite block;
    cgf::Sprite boom;

    sf::Font font;
    sf::Text text;

    bool firstTime;
    bool exploding;

    cgf::Physics* phys;
    b2Body* pball, *psheep;
    b2Body* pblock;
    enum { BALL_ID, SHEEP_ID, WALL_ID, GROUND_ID };

    // Centers the camera on the player position (if player is too close to the borders, stop)
    void centerMapOnPlayer();
};

#endif
