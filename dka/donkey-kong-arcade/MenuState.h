#ifndef _MENU_STATE_H_
#define _MENU_STATE_H_

#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"

class MenuState : public cgf::GameState
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
    static MenuState* instance()
    {
        return &m_MenuState;
    }

    protected:

    MenuState() {}

    private:

    static MenuState m_MenuState;

    cgf::Sprite menuSprite;
    cgf::InputManager* im;

    sf::SoundBuffer menuSoundBuffer;
    sf::Sound menuSound;
};

#endif
