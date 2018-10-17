/*
 *  PlayState.cpp
 *  Example gameplay: ball-hits-sheep, with physics...
 *
 *  Created by Marcelo Cohen on R0515.
 *  Copyright 2015 PUCRS. All rights reserved.
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
    firstTime = true; // to set map position at first update
    map = new tmx::MapLoader("data/maps");
    map->Load("ballgame.tmx");
    speed = 100;   // speed to use

    sheep.loadXML("data/img/sheep.xml");
    sheep.loadAnimation("data/img/sheepanim.xml");
    sheep.setPosition(670,650);
    sheep.setScale(0.8,0.8);
    sheep.setAnimRate(10);
    sheep.setAnimation("walk");
    sheep.play();

    ball.load("data/img/football.png");
    ball.setPosition(30,750);
    ball.setScale(0.3,0.3);

    block.load("data/img/tallcolumn.png");
    block.setPosition(500,600);

    boom.loadXML("data/img/boom.xml");
    boom.loadAnimation("data/img/boomanim.xml");
    boom.setAnimRate(10);
    exploding = false;

    im = cgf::InputManager::instance();

    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("up", sf::Keyboard::Up);
    im->addKeyInput("down", sf::Keyboard::Down);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("zoomin", sf::Keyboard::Z);
    im->addKeyInput("zoomout", sf::Keyboard::X);
    im->addMouseInput("rightclick", sf::Mouse::Right);

    if (!font.loadFromFile("data/fonts/arial.ttf"))
    {
        cout << "Cannot load arial.ttf font!" << endl;
        exit(1);
    }

    phys = cgf::Physics::instance();
    phys->setGravity(30);
    phys->setConvFactor(30);

    pball = phys->newCircle(BALL_ID,  // id do objeto, definida num enum no .h
                &ball, // ponteiro para imagem ou sprite
                50,   // densidade (junto com o tamanho, define a massa do objeto)
                0.1,  // coeficiente de fricção
                0.5   // coeficiente de restituição
            );

    psheep = phys->newRect(SHEEP_ID, &sheep, 50, 0.1, 0.5);
    pblock = phys->newRect(WALL_ID, &block, 100, 0.9, 0.1);

    auto& layers = map->GetLayers();
    tmx::MapLayer& layer = layers[1];

    for(auto& object: layer.objects)
    {
        sf::FloatRect rect = object.GetAABB();
//        cout << "box: " << rect.left << "," << rect.top << " - " << rect.width << " x " << rect.height << endl;
        phys->newRect(GROUND_ID, rect.left, rect.top, rect.width, rect.height, 1, 0.5, 0.1, true);
    }

    firstTime = true;

    // select the font
    text.setFont(font);
    text.setString(L"Test!");
    text.setCharacterSize(24); // in pixels, not points!
    text.setFillColor(sf::Color::Red);
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);

    cout << "PlayState Init Successful" << endl;
}

void PlayState::cleanup()
{
//    delete map;
    cout << "PlayState Clean Successful" << endl;
}

void PlayState::pause()
{
    cout << "PlayState Paused" << endl;
}

void PlayState::resume()
{
    cout << "PlayState Resumed" << endl;
}

// Initial setup when the screen initialises
void PlayState::setup(cgf::Game* game)
{
    screen = game->getScreen();
    screen->setVerticalSyncEnabled(true);
    phys->setRenderTarget(*screen);
}

void PlayState::handleEvents(cgf::Game* game)
{
    sf::Event event;
    // First time here, get screen and define settings
    if(firstTime) {
        setup(game);
        firstTime = false;
    }
    sf::View view = screen->getView();

    while (screen->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            game->quit();
        if(event.type == sf::Event::KeyPressed)
            if(event.key.code == sf::Keyboard::S)
                game->toggleStats();
            else if(event.key.code == sf::Keyboard::G)
            {
                cout << phys->getGravity() << endl;
                if(phys->getGravity() == 0)
                    phys->setGravity(30);
                else
                    phys->setGravity(0);
            }
            else if(event.key.code == sf::Keyboard::Space)
                pball->ApplyLinearImpulse(b2Vec2(0,-1200), pball->GetWorldCenter(), true);
    }

    int dirx, diry;
    dirx = diry = 0;

    if(im->testEvent("left"))
        dirx = -1;
    else
    if(im->testEvent("right"))
        dirx = 1;

    if(im->testEvent("up"))
        diry = -1;
    else
    if(im->testEvent("down"))
        diry = 1;

    if(im->testEvent("quit") || im->testEvent("rightclick"))
        game->quit();

    if(im->testEvent("zoomin"))
    {
        view.zoom(1.01);
        screen->setView(view);
    }
    else if(im->testEvent("zoomout"))
    {
        view.zoom(0.99);
        screen->setView(view);
    }

    pball->ApplyLinearImpulse(b2Vec2(dirx*100,diry*100), pball->GetWorldCenter(), true);
}

void PlayState::update(cgf::Game* game)
{
    phys->step();

    b2Body* bptr;
    if((bptr=phys->haveContact(WALL_ID, SHEEP_ID)) != NULL && !exploding)
    {
        // Trata colisão (em bptr fica o ponteiro para o objeto cujo id é SHEEP_ID)
        cout << "Wall hit sheep!" << endl;
        cout << "Velocidade angular: " << pblock->GetAngularVelocity() << endl;
        phys->destroy(psheep);
        sheep.setVisible(false);
        boom.setPosition(sheep.getPosition().x-100,sheep.getPosition().y-100);
        boom.setVisible(true);
        boom.setAnimation("explode");
        boom.play();
        exploding = true;
        cout << "exploding" << endl;
    }

    if(exploding)
    {
        boom.update(game->getUpdateInterval());
        if(boom.isStopped())
        {
            sheep.setVisible(false);
            exploding = false;
            cout << "Exploded!" << endl;
        }
    }

    sheep.update(game->getUpdateInterval());

    centerMapOnPlayer();
}

void PlayState::centerMapOnPlayer()
{
    sf::View view = screen->getView();
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2f viewsize = view.getSize();
    viewsize.x /= 2;
    viewsize.y /= 2;
    sf::Vector2f pos = ball.getPosition();

//    cout << "vw: " << view.getSize().x << " " << view.getSize().y << endl;

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

//    cout << "pos: " << pos.x << " " << pos.y << endl;
//    cout << "pan: " << panX << " " << panY << endl;

    view.setCenter(sf::Vector2f(panX,panY));
    screen->setView(view);
}

void PlayState::draw(cgf::Game* game)
{
    screen->clear(sf::Color(0,0,0));

    map->Draw(*screen, 0);
    screen->draw(ball);
    screen->draw(sheep);
    screen->draw(block);
    if(exploding) screen->draw(boom);

//    phys->drawDebugData();

    screen->draw(text);
}
