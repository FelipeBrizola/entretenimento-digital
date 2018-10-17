#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include "string"
#include "GameState.h"
#include "Sprite.h"
#include "Game.h"
#include "InputManager.h"

using namespace std;

class GameOverState : public cgf::GameState
{
public:

    void init();
    void cleanup();

    void pause();
    void resume();

    void setup(cgf::Game* game);
    void handleEvents(cgf::Game* game);
    void update(cgf::Game* game);
    void draw(cgf::Game* game);

    // Implement Singleton Pattern
    static GameOverState* instance()
    {
        return &m_GameOverState;
    }

    protected:

    GameOverState() {}

    private:

    static GameOverState m_GameOverState;

    cgf::Sprite gameoverSprite;
    cgf::InputManager* im;
};

#endif // GAMEOVERSTATE_H
