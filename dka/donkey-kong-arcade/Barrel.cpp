#include "Barrel.h"

Barrel::Barrel(int x, int y)
{
    sprite.load("data/img/barril.png");
    sprite.setPosition(x, y);
}

Barrel::~Barrel()
{
    //dtor
}

void Barrel::moving()
{
    this->sprite.setPosition(this->sprite.getPosition().x, this->sprite.getPosition().y + 1);
}

cgf::Sprite Barrel::getSprite ()
{
    return this->sprite;
}
