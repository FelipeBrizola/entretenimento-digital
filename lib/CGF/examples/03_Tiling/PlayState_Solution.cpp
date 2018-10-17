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
#include "PlayState_Solution.h"
#include "InputManager.h"

PlayState PlayState::m_PlayState;

using namespace std;

void PlayState::init()
{
    if (!font.loadFromFile("data/fonts/arial.ttf")) {
        cout << "Cannot load arial.ttf font!" << endl;
        exit(1);
    }
    text.setFont(font);
    text.setString(L"Testing text output in SFML");
    text.setCharacterSize(24); // in pixels
    text.setFillColor(sf::Color::Yellow);
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);

    map = new tmx::MapLoader("data/maps");       // all maps/tiles will be read from data/maps
    // map->AddSearchPath("data/maps/tilesets"); // e.g.: adding more search paths for tilesets
    map->Load("dungeon-tilesets2.tmx");

    walkStates[0] = "walk-right";
    walkStates[1] = "walk-left";
    walkStates[2] = "walk-up";
    walkStates[3] = "walk-down";
    currentDir = RIGHT;
    player.load("data/img/warrior.png",64,64,0,0,0,0,13,21,273);
    player.setPosition(10,100);
    player.loadAnimation("data/img/warrioranim.xml");
    player.setAnimation(walkStates[currentDir]);
    player.setAnimRate(15);
    player.play();

    dirx = 0; // sprite dir: right (1), left (-1)
    diry = 0; // down (1), up (-1)

    im = cgf::InputManager::instance();

    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("up", sf::Keyboard::Up);
    im->addKeyInput("down", sf::Keyboard::Down);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("stats", sf::Keyboard::S);
    im->addMouseInput("rightclick", sf::Mouse::Right);

    // Camera control
    im->addKeyInput("zoomin", sf::Keyboard::Z);
    im->addKeyInput("zoomout", sf::Keyboard::X);

    cout << "PlayState: Init" << endl;
}

void PlayState::cleanup()
{
    delete map;
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
    screen = game->getScreen();
    sf::View view = screen->getView(); // gets the view
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

    if(im->testEvent("up")) {
        diry = -1;
        newDir = UP;
    }

    if(im->testEvent("down")) {
        diry = 1;
        newDir = DOWN;
    }

    if(im->testEvent("quit") || im->testEvent("rightclick"))
        game->quit();

    if(im->testEvent("stats"))
        game->toggleStats();

    if(im->testEvent("zoomin")) {
        view.zoom(1.01);
        screen->setView(view);
    }
    else if(im->testEvent("zoomout")) {
        view.zoom(0.99);
        screen->setView(view);
    }

    if(dirx == 0 && diry == 0) {
        player.pause();
    }
    else {
        if(currentDir != newDir) {
            player.setAnimation(walkStates[newDir]);
            currentDir = newDir;
        }
        player.play();
    }

    player.setXspeed(100*dirx);
    player.setYspeed(100*diry);
}

void PlayState::update(cgf::Game* game)
{
    player.update(game->getUpdateInterval());
}

void PlayState::draw(cgf::Game* game)
{
    screen = game->getScreen();
    map->Draw(*screen);          // draw all layers
//    map->Draw(*screen, 1);     // draw only the second layer
    screen->draw(player);
    screen->draw(text);
    centerMapOnPlayer();
}

void PlayState::centerMapOnPlayer()
{
    sf::View view = screen->getView();
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2f viewsize = view.getSize();
    viewsize.x /= 2;
    viewsize.y /= 2;
    sf::Vector2f pos = player.getPosition();

    float panX = viewsize.x; // minimum pan
    if(pos.x >= viewsize.x)
        panX = pos.x;

    if(panX >= mapsize.x - viewsize.x)
        panX = mapsize.x - viewsize.x;

    float panY = viewsize.y; // minimum pan
    if(pos.y >= viewsize.y)
        panY = pos.y;

    if(panY >= mapsize.y - viewsize.y)
        panY = mapsize.y - viewsize.y;

    view.setCenter(sf::Vector2f(panX,panY));
    screen->setView(view);
}
