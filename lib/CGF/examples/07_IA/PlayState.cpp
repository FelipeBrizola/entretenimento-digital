/*
 *  PlayState.cpp
 *  Testbed for AI experiments
 *
 *  Created by Marcelo Cohen on 05/11.
 *  Copyright 2011 PUCRS. All rights reserved.
 *
 */

#include <iostream>
#include <cmath>
#include "Game.h"
#include "PlayState.h"
#include "VectorUtils.h"

PlayState PlayState::m_PlayState;

using namespace std;

const string PlayState::modes[] = { "Chase", "Arrive", "Pursuit", "Flee", "Evade" };

void PlayState::init()
{
    map = new tmx::MapLoader("data/maps");
    map->Load("dungeon2layers.tmx");

    playerK.sprite = &player;
    playerK.vel.x = playerK.vel.y = 0;   // current player speed
    speed = 100;   // speed to use

    player.loadXML("data/img/hunter.xml");
    player.setPosition(50,100);
    player.loadAnimation("data/img/hunteranim.xml");
    player.setAnimRate(15);
    playerK.pos.x = 50;
    playerK.pos.y = 100;

    ghost.load("data/img/Char14.png");
    ghost.setPosition(100,300);
    ghost.setScale(sf::Vector2f(2,2));

    enemyK.sprite = &ghost;
    enemyK.pos.x = 100;
    enemyK.pos.y = 300;
    enemyK.maxSpeed = 80;

    steerMode = CHASE_BEHAVIOR; // default: chase player

    font.loadFromFile("data/fonts/arial.ttf");
    // select the font
    text.setFont(font);
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::Black);
    //text.setStyle(sf::Text::Bold | sf::Text::Underlined);

    firstTime = true; // to set map position at first update

    showTrails = false;

    im = cgf::InputManager::instance();
    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("up", sf::Keyboard::Up);
    im->addKeyInput("down", sf::Keyboard::Down);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("zoomin", sf::Keyboard::Z);
    im->addKeyInput("zoomout", sf::Keyboard::X);
    im->addMouseInput("rightclick", sf::Mouse::Right);

	cout << "PlayState Init Successful" << endl;
}

void PlayState::cleanup()
{
    delete map;
    //  delete playSprite2;
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
}

void PlayState::handleEvents(cgf::Game* game)
{
    sf::Event event;
    if(firstTime) {
        setup(game);
        firstTime = false;
    }
    sf::View view = screen->getView();

    while (screen->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            game->quit();
        if(event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape)
                game->quit();
            else if (event.key.code == sf::Keyboard::S)
                game->toggleStats();
            else if (event.key.code == sf::Keyboard::B) {
                steerMode++;
                if (steerMode > EVADE_BEHAVIOR)
                    steerMode = CHASE_BEHAVIOR;
            }
            else if (event.key.code == sf::Keyboard::T)
                showTrails = !showTrails;
        }
    }

    int dirx, diry;
    dirx = diry = 0;

    if(im->testEvent("left")) {
        if(playerK.vel.x >= 0) {
            player.setAnimation("walk-left");
            player.play();
        }
        dirx = -1;
    }
    else
    if(im->testEvent("right")) {
        if(playerK.vel.x <= 0) {
            player.setAnimation("walk-right");
            player.play();
        }
        dirx = 1;
    }

    if(im->testEvent("up")) {
        if(playerK.vel.y >= 0) {
            player.setAnimation("walk-up");
            player.play();
        }
        diry = -1;
    }

    if(im->testEvent("down")) {
        if(playerK.vel.y <= 0) {
            player.setAnimation("walk-down");
            player.play();
        }
        diry = 1;
    }

    if(!dirx && !diry) // parado?
    {
        player.setCurrentFrame(0);
        player.pause();
    }

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

    //player.setXspeed(dirx*100);
    //player.setYspeed(diry*100);

    playerK.vel.x = dirx*speed;
    playerK.vel.y = diry*speed;
}

// Steering Behavior: chase target
sf::Vector3f PlayState::chase(Kinematic& vehicle, sf::Vector3f& target)
{
    sf::Vector3f desiredVel = target - vehicle.pos;
    vecutils::normalize(desiredVel);
    desiredVel *= vehicle.maxSpeed;
    return desiredVel - vehicle.vel;
}

// Steering Behavior: arrive at target
sf::Vector3f PlayState::arrive(Kinematic& vehicle, sf::Vector3f& target, float decel)
{
    sf::Vector3f toTarget = target - vehicle.pos;
    float d = vecutils::length(toTarget);
    if(d > 0)
    {
        // Calculate the speed required to reach the target given the desired
        // deceleration
        float speed = d / decel;

        // Make sure the velocity does not exceed the max
        speed = min(speed, vehicle.maxSpeed);

        // From here proceed just like chase, except we don't need to normalize
        // the toTarget vector because we have already gone to the trouble
        // of calculating its length: d
        sf::Vector3f desiredVel = toTarget * speed / d;
        return desiredVel - vehicle.vel;
    }
    return sf::Vector3f(0,0,0);
}

// Steering Behavior: flee from target
sf::Vector3f PlayState::flee(Kinematic& vehicle, sf::Vector3f& target, float panicDistance)
{
    float panicDistance2 = panicDistance * panicDistance;
    if(vecutils::distanceSquared(enemyK.pos, target) > panicDistance2)
        return sf::Vector3f(0,0,0);
    sf::Vector3f desiredVel = vehicle.pos - target;
    vecutils::normalize(desiredVel);
    desiredVel *= vehicle.maxSpeed;
    return desiredVel - vehicle.vel;
}

// Steering Behavior: pursuit target
sf::Vector3f PlayState::pursuit(Kinematic& vehicle, Kinematic& target)
{
    sf::Vector3f toEvader = target.pos - vehicle.pos;
    double relativeHeading = vecutils::dotProduct(vehicle.heading, target.heading);
    // If target is facing us, go chase it
    if(vecutils::dotProduct(toEvader, vehicle.heading) > 0 && relativeHeading < -0.95) // acos(0.95) = 18 graus
        return arrive(vehicle, target.pos);

    // Not facing, so let's predict where the target will be

    // The look-ahead time is proportional to the distance between the target
    // and the enemy, and is inversely proportional to the sum of the
    // agents' velocities

    float vel = vecutils::length(target.vel);
    float lookAheadTime = vecutils::length(toEvader) / (vehicle.maxSpeed + vel);

    // Now chase to the predicted future position of the target

    sf::Vector3f predicted(target.pos + target.vel * lookAheadTime);
    return arrive(vehicle, predicted);
}

// Steering Behavior: evade target
sf::Vector3f PlayState::evade(Kinematic& vehicle, Kinematic& target)
{
    sf::Vector3f toPursuer = target.pos - vehicle.pos;

    // The look-ahead time is proportional to the distance between the pursuer
    // and the vehicle, and is inversely proportional to the sum of the
    // agents' velocities

    float vel = vecutils::length(target.vel);
    float lookAheadTime = vecutils::length(toPursuer) / (vehicle.maxSpeed + vel);

    // Now chase to the predicted future position of the target

    sf::Vector3f predicted(target.pos + target.vel * lookAheadTime);
    return flee(vehicle, predicted);
}

void PlayState::update(cgf::Game* game)
{
    // First time in update, set initial camera pos
    if(firstTime) {
        screen = game->getScreen();
        firstTime = false;
    }

    checkCollision(1, game, playerK);
    centerMapOnPlayer();

#define STEERING
#ifdef STEERING
    // Apply steering behavior(s)

    //sf::Vector3f steeringForce = flee(enemyK, playerK,100);
    //sf::Vector3f steeringForce = pursuit(enemyK, playerK);
    sf::Vector3f steeringForce;

    switch(steerMode) {
        case CHASE_BEHAVIOR:
            steeringForce = chase(enemyK, playerK.pos);
            break;
        case ARRIVE_BEHAVIOR:
            steeringForce = arrive(enemyK, playerK.pos, 0.3); // 0.3 - rapido ... 1 - lento
            break;
        case PURSUIT_BEHAVIOR:
            steeringForce = pursuit(enemyK, playerK);
            break;
        case FLEE_BEHAVIOR:
            steeringForce = flee(enemyK, playerK.pos, 100);
            break;
        case EVADE_BEHAVIOR:
            steeringForce = evade(enemyK, playerK);
    }
    sf::Vector3f accel = steeringForce/1.f; // mass;

    enemyK.vel += accel; // * deltaTime

    // Can't exceed max speed
    if(vecutils::lengthSquared(enemyK.vel) > enemyK.maxSpeed*enemyK.maxSpeed) {
        vecutils::normalize(enemyK.vel);
        enemyK.vel *= enemyK.maxSpeed;
    }

    // Only update heading if speed is above minimum threshold
    if(vecutils::lengthSquared(enemyK.vel) > 0.00000001) {
        enemyK.heading = enemyK.vel / vecutils::length(enemyK.vel);
    }
#else
    // Basic chase

    enemyK.vel.set(0,0,0);
    if(enemyK.pos.X < playerK.pos.X)
        enemyK.vel.X = 2;
    else if(enemyK.pos.X > playerK.pos.X)
        enemyK.vel.X = -2;
    if(enemyK.pos.Y < playerK.pos.Y)
        enemyK.vel.Y = 2;
    else if(enemyK.pos.Y > playerK.pos.Y)
        enemyK.vel.Y = -2;
#endif
    checkCollision(1, game, enemyK);

    if(trail.size()>30)
        trail.pop_back();
    trail.push_front(enemyK.pos);
}

void PlayState::centerMapOnPlayer()
{
    sf::View view = screen->getView();
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2f viewsize = view.getSize();
    viewsize.x /= 2;
    viewsize.y /= 2;
    sf::Vector2f pos = player.getPosition();

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

bool PlayState::checkCollision(uint8_t layer, cgf::Game* game, Kinematic& obj)
{
    int i, x1, x2, y1, y2;
    bool bump = false;

    // Get the limits of the map
    sf::Vector2u mapsize = map->GetMapSize();
    // Get the width and height of a single tile
    sf::Vector2u tilesize = map->GetMapTileSize();

    mapsize.x /= tilesize.x;
    mapsize.y /= tilesize.y;
    mapsize.x--;
    mapsize.y--;

    // Get the height and width of the object (in this case, 100% of a tile)
    sf::Vector2u objsize = obj.sprite->getSize();
    objsize.x *= obj.sprite->getScale().x;
    objsize.y *= obj.sprite->getScale().y;

    float px = obj.pos.x; //->getPosition().x;
    float py = obj.pos.y; //->getPosition().y;

    double deltaTime = game->getUpdateInterval();

    sf::Vector2f offset(obj.vel.x/1000 * deltaTime, obj.vel.y/1000 * deltaTime);

    float vx = offset.x;
    float vy = offset.y;

    //cout << "px,py: " << px << " " << py << endl;

    //cout << "tilesize " << tilesize.x << " " << tilesize.y << endl;
    //cout << "mapsize " << mapsize.x << " " << mapsize.y << endl;

    // Test the horizontal movement first
    i = objsize.y > tilesize.y ? tilesize.y : objsize.y;

    for (;;)
    {
        x1 = (px + vx) / tilesize.x;
        x2 = (px + vx + objsize.x - 1) / tilesize.x;

        y1 = (py) / tilesize.y;
        y2 = (py + i - 1) / tilesize.y;

        if (x1 >= 0 && x2 < mapsize.x && y1 >= 0 && y2 < mapsize.y)
        {
            if (vx > 0)
            {
                // Trying to move right

                int upRight   = getCellFromMap(layer, x2*tilesize.x, y1*tilesize.y);
                int downRight = getCellFromMap(layer, x2*tilesize.x, y2*tilesize.y);
                if (upRight || downRight)
                {
                    // Place the player as close to the solid tile as possible
                    px = x2 * tilesize.x;
                    px -= objsize.x;// + 1;
                    vx = 0;
                    bump = true;
                }
            }

            else if (vx < 0)
            {
                // Trying to move left

                int upLeft   = getCellFromMap(layer, x1*tilesize.x, y1*tilesize.y);
                int downLeft = getCellFromMap(layer, x1*tilesize.x, y2*tilesize.y);
                if (upLeft || downLeft)
                {
                    // Place the player as close to the solid tile as possible
                    px = (x1+1) * tilesize.x;
                    vx = 0;
                    bump = true;
                }
            }
        }

        if (i == objsize.y) // Checked player height with all tiles ?
        {
            break;
        }

        i += tilesize.y; // done, check next tile upwards

        if (i > objsize.y)
        {
            i = objsize.y;
        }
    }

    // Now test the vertical movement

    i = objsize.x > tilesize.x ? tilesize.x : objsize.x;

    for (;;)
    {
        x1 = (px / tilesize.x);
        x2 = ((px + i-1) / tilesize.x);

        y1 = ((py + vy) / tilesize.y);
        y2 = ((py + vy + objsize.y-1) / tilesize.y);

        if (x1 >= 0 && x2 < mapsize.x && y1 >= 0 && y2 < mapsize.y)
        {
            if (vy > 0)
            {
                // Trying to move down
                int downLeft  = getCellFromMap(layer, x1*tilesize.x, y2*tilesize.y);
                int downRight = getCellFromMap(layer, x2*tilesize.x, y2*tilesize.y);
                if (downLeft || downRight)
                {
                    // Place the player as close to the solid tile as possible
                    py = y2 * tilesize.y;
                    py -= objsize.y;
                    vy = 0;
                    bump = true;
                }
            }

            else if (vy < 0)
            {
                // Trying to move up

                int upLeft  = getCellFromMap(layer, x1*tilesize.x, y1*tilesize.y);
                int upRight = getCellFromMap(layer, x2*tilesize.x, y1*tilesize.y);
                if (upLeft || upRight)
                {
                    // Place the player as close to the solid tile as possible
                    py = (y1 + 1) * tilesize.y;
                    vy = 0;
                    bump = true;
                }
            }
        }

        if (i == objsize.x)
        {
            break;
        }

        i += tilesize.x; // done, check next tile to the right

        if (i > objsize.x)
        {
            i = objsize.x;
        }
    }

    // Now apply the movement and animation

    obj.pos.x = px+vx;
    obj.pos.y = py+vy;

    // Check collision with edges of map
    if (obj.pos.x < 0)
        obj.pos.x = 0;
    else if (obj.pos.x + objsize.x >= mapsize.x * tilesize.x)
        obj.pos.x = mapsize.x*tilesize.x - objsize.x - 1;

    if(obj.pos.y < 0)
        obj.pos.y = 0;
    else if(obj.pos.y + objsize.y >= mapsize.y * tilesize.y)
        obj.pos.y = mapsize.y*tilesize.y - objsize.y - 1;

    obj.sprite->setPosition(obj.pos.x, obj.pos.y);
    obj.sprite->update(deltaTime, false); // only update animation

    return bump;
}

sf::Uint16 PlayState::getCellFromMap(uint8_t layernum, float x, float y)
{
    auto& layers = map->GetLayers();
    tmx::MapLayer& layer = layers[layernum];
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2u tilesize = map->GetMapTileSize();
    mapsize.x /= tilesize.x;
    mapsize.y /= tilesize.y;
    int col = floor(x / tilesize.x);
    int row = floor(y / tilesize.y);
    return layer.tiles[row*mapsize.x + col].gid;
}

void PlayState::draw(cgf::Game* game)
{
    //sf::View view = screen->getView();

    screen->clear(sf::Color(0,0,0));

    map->Draw(*screen, 0);
    screen->draw(player);

    if(showTrails)
    {
        sf::Color transp = sf::Color::White;
        transp.a = 20;
        ghost.setColor(transp);
        for(auto pos: trail)
        {
            ghost.setPosition(pos.x, pos.y);
            screen->draw(ghost);
        }
        ghost.setColor(sf::Color::White);
    }

    ghost.setPosition(enemyK.pos.x, enemyK.pos.y);
    screen->draw(ghost);

    text.setPosition(enemyK.pos.x, enemyK.pos.y-14);
    text.setString(modes[steerMode]);

    screen->draw(text);
}
