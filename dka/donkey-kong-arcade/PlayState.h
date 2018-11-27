/*
 *  PlayState.h
 *  Normal "play" state
 *
 *  Created by Marcelo Cohen on 08/13.
 *  Copyright 2013 PUCRS. All rights reserved.
 *
 */

#ifndef PLAY_STATE_H_
#define PLAY_STATE_H_

#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"
#include <string>
#include <tmx/MapLoader.h>

using namespace std;

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

    // Implement Singleton Pattern
    static PlayState* instance()
    {
        return &m_PlayState;
    }

    protected:

    PlayState() {}

    private:

    enum { RIGHT = 0, LEFT, UP, DOWN, JUMP };

    static PlayState m_PlayState;

    string playerStates[5];
    int currentState;

    tmx::MapLoader* map;

    int dirx, diry;
    cgf::Sprite player;
    cgf::Sprite playSprite2;
    cgf::Sprite playSprite3;
    sf::RenderWindow* screen;
    cgf::InputManager* im;
};

#endif
