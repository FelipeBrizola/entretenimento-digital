/*
 *  PlayState.cpp
 *  Normal "play" state
 *
 *  Created by Marcelo Cohen on 08/13.
 *  Copyright 2013 PUCRS. All rights reserved.
 *
 */

#include <iostream>
#include <cmath>
#include "Game.h"
#include "PlayState.h"
#include "InputManager.h"

PlayState PlayState::m_PlayState;

using namespace std;

void PlayState::init()
{
    levelSoundBuffer.loadFromFile("data/audio/intro1.wav");
    levelSound.setBuffer(levelSoundBuffer);

    levelSound.setVolume(100);
    levelSound.play();

    walkStates[0] = "right";
    walkStates[1] = "left";
    walkStates[2] = "up";
    walkStates[3] = "down";
    walkStates[4] = "right-stop";
    walkStates[5] = "left-stop";
    walkStates[6] = "up-stop";
    walkStates[7] = "down-stop";
    //player.load("data/img/Char19s.png");
	//player.setPosition(10,100);

    level.load("data/maps/level.png");
	level.setPosition(0,0);

    dirx = 0; // sprite direction: right (1), left (-1)
    diry = 0; // down (1), up (-1)

    im = cgf::InputManager::instance();

    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("up", sf::Keyboard::Up);
    im->addKeyInput("down", sf::Keyboard::Down);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("stats", sf::Keyboard::S);
    im->addMouseInput("rightclick", sf::Mouse::Right);
    im->addKeyInput("dead", sf::Keyboard::D);

    player.load("data/player/mario.png",16,16,24,24,0,0,8,4,32);
    player.loadAnimation("data/player/test.xml");
    //player.setAnimation('wal');
    //sprite.setPosition(96,172);
    player.setPosition(30,30);
    player.setAnimRate(30);
    player.setScale(2,2);
    player.play();

    dirx = 0; // sprite dir: right (1), left (-1)
    diry = 0; // down (1), up (-1)

    currentDir = DOWN;

	cout << "PlayState: Init" << endl;
}

void PlayState::cleanup()
{
	cout << "PlayState: Clean" << endl;
}

void PlayState::pause()
{
	cout << "PlayState: Paused" << endl;
}

void PlayState::resume()
{
	cout << "PlayState: Resumed" << endl;
}

void PlayState::handleEvents(cgf::Game* game)
{
    sf::Event event;

    while (screen->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            game->quit();
    }

    dirx = diry = 0;

    int newDir = currentDir;

    if(im->testEvent("left")) {
        dirx = -1;
        newDir = LEFT;
    }

    if(im->testEvent("right")) {
        dirx = 1;
        newDir = RIGHT;
    }

    if(im->testEvent("up")){
        diry = -1;
        newDir = UP;
    }

    if(im->testEvent("down")) {
        diry = 1;
        newDir = DOWN;
    }

    if(dirx == 0 && diry == 0) {
        if(currentDir < 4)
            currentDir+=4;
        player.setAnimation(walkStates[currentDir]);
        player.pause();
    }
    else {
        if(currentDir != newDir) {
            player.setAnimation(walkStates[newDir]);
            currentDir = newDir;
            player.play();
        }
    }

    if(im->testEvent("dead")) {
        player.setAnimation("death");
        player.play();
    }

    if(im->testEvent("quit") || im->testEvent("rightclick"))
        game->quit();

    if(im->testEvent("stats"))
        game->toggleStats();
}

void PlayState::update(cgf::Game* game)
{
    float x = player.getPosition().x;
    float y = player.getPosition().y;
    x += dirx*5;
    y += diry*5;
    player.setPosition(x,y);
    player.update(game->getUpdateInterval());
}

void PlayState::draw(cgf::Game* game)
{
    screen = game->getScreen();
    screen->draw(level);
    screen->draw(player);
}
