#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>

using namespace std;

class Player
{
    public:

    void init();
    int getScore();
    void addsScore(int score);
    int getLifePoints();
    void removeLifePoint();
    int getLevel();
    void levelUp();
    void reset();

    static Player* instance()
    {
        return &m_Player;
    }

    protected:

    Player() {}

    private:

    static Player m_Player;

    int score = 0;
    int lifePoints = 3;
    int level = 1;
    string name;
};

#endif
