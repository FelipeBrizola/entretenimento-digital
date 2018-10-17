#include <iostream>
#include "Player.h"

using namespace std;

Player Player::m_Player;

void Player::init()
{
    cout << "Player Init Successful" << endl;
}

int Player::getScore()
{
    return score;
}

void Player::addsScore(int score)
{
    this->score = this->score + score;
}

int Player::getLifePoints()
{
    return lifePoints;
}

void Player::removeLifePoint()
{
    lifePoints -= 1;
}

int Player::getLevel()
{
    return level;
}

void Player::levelUp()
{
    level += 1;
}

void Player::reset()
{
    lifePoints = 3;
    score = 0;
    level = 1;
}
