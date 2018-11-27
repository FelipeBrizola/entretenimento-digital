#include <iostream>
#include "Game.h"
#include "InputManager.h"
#include "MenuState.h"
#include "PlayState.h"

MenuState MenuState::m_MenuState;

using namespace std;

void MenuState::init()
{
    menuSprite.load("data/img/dk_logo.png");

    im = cgf::InputManager::instance();

    im->addKeyInput("start", sf::Keyboard::Return);
    im->addKeyInput("quit", sf::Keyboard::Escape);

    menuSoundBuffer.loadFromFile("data/audio/menu.wav");
    menuSound.setBuffer(menuSoundBuffer);

    menuSound.setLoop(true);
    menuSound.setVolume(100);
    menuSound.play();

    cout << "MenuState Init Successful" << endl;
}

void MenuState::cleanup()
{
    menuSound.stop();
    cout << "MenuState Cleanup Successful" << endl;
}

void MenuState::pause()
{
    menuSound.stop();
    cout << "MenuState Paused" << endl;
}

void MenuState::resume()
{
    menuSound.play();
    cout << "MenuState Resumed" << endl;
}

void MenuState::handleEvents(cgf::Game* game)
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

    if (im->testEvent("start"))
        game->changeState(PlayState::instance());
}

void MenuState::update(cgf::Game* game)
{
}

void MenuState::draw(cgf::Game *game)
{
    sf::RenderWindow* screen = game->getScreen();

    int screen_width = screen->getSize().x;
    int screen_height = screen->getSize().y;

    int x = screen_width*0.5 - menuSprite.getSize().x * 0.5;
    int y = screen_height*0.5 - menuSprite.getSize().y * 0.5;

    menuSprite.setPosition(x,y);
    screen->draw(menuSprite);
}
