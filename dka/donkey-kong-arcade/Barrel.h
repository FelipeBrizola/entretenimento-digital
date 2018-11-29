#ifndef BARREL_H
#define BARREL_H

#include "Sprite.h"

class Barrel
{
    public:

    void moving();
    cgf::Sprite getSprite();

    Barrel(int x, int y);
    virtual ~Barrel();

    protected:

    private:

    cgf::Sprite sprite;
};

#endif // BARREL_H


