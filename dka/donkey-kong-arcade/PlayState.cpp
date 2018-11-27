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
#include <string>

PlayState PlayState::m_PlayState;

using namespace std;

void PlayState::init()
{
    map = new tmx::MapLoader("data/maps");       // todos os mapas/tilesets serão lidos de data/maps
    map->Load("dungeon-tilesets2.tmx");

    cout << "PlayState Init Successful" << endl;

    player.load("data/img/Char19s.png");
	player.setPosition(10,100);

    playSprite2.load("data/img/Char01.png");
	playSprite2.setPosition(10,300);

    playSprite3.load("data/img/Char01.png");
	playSprite3.setPosition(50,300);

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


    playerStates[0] = "walk-right";
    playerStates[1] = "walk-left";
    playerStates[2] = "walk-up";
    playerStates[3] = "walk-down";
    playerStates[4] = "jump";
    currentState = LEFT;


    player.load("data/img/warrior.png",64,64,0,0,0,0,13,21,273);
	player.setPosition(712,480);
    player.loadAnimation("data/img/warrioranim.xml");
    player.setAnimation(playerStates[currentState]);
    player.setAnimRate(15);
    player.play();


	cout << "PlayState: Init" << endl;
}

void PlayState::cleanup()
{
    delete map;
    cout << "PlayState: Clean" << endl;
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

    if(im->testEvent("left"))
        dirx = -1;

    if(im->testEvent("right"))
        dirx = 1;

    if(im->testEvent("up"))
        diry = -1;

    if(im->testEvent("down"))
        diry = 1;

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
    map->Draw(*screen);         // mapa é fundo, precisa desenhar primeiro
    screen->draw(player);
    screen->draw(playSprite2);
    screen->draw(playSprite3);
}
