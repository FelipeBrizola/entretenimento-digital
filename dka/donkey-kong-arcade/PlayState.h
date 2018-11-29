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
#include <tmx/MapLoader.h>
#include <vector>
#include "Barrel.h"
#include <stdlib.h>

class PlayState : public cgf::GameState
{
    public:

    void init();
    void cleanup();

    void pause();
    void resume();

    void wonGame(cgf::Game* game);
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

    static PlayState m_PlayState;


    enum { RIGHT=0, LEFT, UP, DOWN };
    std::string walkStates[8];
    int currentDir;
    int x, y;

    int dirx, diry;
    cgf::Sprite player;
    cgf::Sprite enemy;

    sf::RenderWindow* screen;
    cgf::InputManager* im;

    sf::SoundBuffer levelSoundBuffer;
    sf::Sound levelSound;
    sf::SoundBuffer winnerSoundBuffer;
    sf::Sound winnerSound;

    unsigned int score = 0;
    sf::Clock clock;
    sf::Time time;
    sf::Text timerText;
    sf::Text finishGameText;
    sf::Font gameFont;
    sf::Color color;

    int VICTORY_POSITION;
    bool isWinner, isLoser;

    tmx::MapLoader* map;

    std::vector<Barrel*> barrels;

    // Checks collision between a sprite and a map layer
    bool checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj);

    // get a cell GID from the map (x and y in world coords)
    sf::Uint16 getCellFromMap(uint8_t layernum, float x, float y);

    bool checkBarrelCollision(cgf::Sprite* player, cgf::Sprite barrel);

};

#endif
