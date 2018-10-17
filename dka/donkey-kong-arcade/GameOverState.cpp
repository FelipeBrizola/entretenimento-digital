#include <iostream>
#include "GameOverState.h"
#include "Game.h"
#include "InputManager.h"
#include "MenuState.h"
#include "Player.h"

GameOverState GameOverState::m_GameOverState;

using namespace std;

void GameOverState::init()
{
    gameoverSprite.load("data/img/gameover.png"); // load game over image

    im = cgf::InputManager::instance();

    im->addKeyInput("restart", sf::Keyboard::R);
    im->addKeyInput("quit", sf::Keyboard::Escape);

    cout << "GameOverState Init Successful" << endl;
}

void GameOverState::cleanup()
{
    cout << "GameOverState Cleanup Successful" << endl;
}

void GameOverState::pause()
{
    cout << "GameOverState Paused" << endl;
}

void GameOverState::resume()
{
    cout << "GameOverState Resumed" << endl;
}

void GameOverState::handleEvents(cgf::Game* game)
{
    sf::Event event;
    sf::RenderWindow* screen = game->getScreen();

    while (screen->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            game->quit();
    }

    if (im->testEvent("quit"))
        game->quit();

    if (im->testEvent("restart"))
    {
        Player::instance()->reset();
        game->changeState(MenuState::instance());
    }
}

void GameOverState::update(cgf::Game* game)
{
}

void GameOverState::draw(cgf::Game *game)
{
    sf::RenderWindow* screen = game->getScreen();

    int screen_width = screen->getSize().x;
    int screen_height = screen->getSize().y;

    int x = screen_width*0.5 - gameoverSprite.getSize().x * 0.5;
    int y = screen_height*0.5 - gameoverSprite.getSize().y * 0.5;

    gameoverSprite.setPosition(x,y);
    screen->draw(gameoverSprite);
}
