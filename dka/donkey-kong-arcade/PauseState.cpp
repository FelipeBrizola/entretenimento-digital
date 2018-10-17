#include <iostream>
#include "Game.h"
#include "InputManager.h"
#include "PauseState.h"

PauseState PauseState::m_PauseState;

using namespace std;

void PauseState::init()
{
    pauseSprite.load("data/img/paused.png"); // load menu state bitmap

    im = cgf::InputManager::instance();

    im->addKeyInput("resume", sf::Keyboard::R);

    cout << "PauseState Init Successful" << endl;
}

void PauseState::cleanup()
{
    cout << "PauseState Cleanup Successful" << endl;
}

void PauseState::pause()
{
    cout << "PauseState Paused" << endl;
}

void PauseState::resume()
{
    cout << "PauseState Resumed" << endl;
}

void PauseState::handleEvents(cgf::Game* game)
{
    sf::Event event;
    sf::RenderWindow* screen = game->getScreen();

    while (screen->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            game->quit();
    }

    if (im->testEvent("resume"))
        game->popState();

}

void PauseState::update(cgf::Game* game)
{
}

void PauseState::draw(cgf::Game *game)
{
    sf::RenderWindow* screen = game->getScreen();

    int screen_width = screen->getSize().x;
    int screen_height = screen->getSize().y;

    int x = screen_width * 0.5 - pauseSprite.getSize().x * 0.5;
    int y = screen_height * 0.5 - pauseSprite.getSize().y * 0.5;

    pauseSprite.setPosition(x,y);
    screen->draw(pauseSprite);
}

