/*
 *  PlayState.cpp
 *  Normal "play" state
 *
 *  Created by Marcelo Cohen on 08/13.
 *  Copyright 2013 PUCRS. All rights reserved.
 *
 */

#include <iostream>
#include <cmath>
#include "Game.h"
#include "PlayState.h"
#include "InputManager.h"

PlayState PlayState::m_PlayState;

using namespace std;

void PlayState::init()
{
    if (!gameFont.loadFromFile("data/fonts/arial.ttf"))
        std::cerr << "No font file found!" << std::endl;

    levelSoundBuffer.loadFromFile("data/audio/intro1.wav");
    levelSound.setBuffer(levelSoundBuffer);
    levelSound.setLoop(false);
    levelSound.setVolume(100);
    levelSound.play();

    winnerSoundBuffer.loadFromFile("data/audio/kaboom.wav");
    winnerSound.setBuffer(winnerSoundBuffer);
    winnerSound.setLoop(false);
    winnerSound.setVolume(100);

    walkStates[0] = "right";
    walkStates[1] = "left";
    walkStates[2] = "up";
    walkStates[3] = "down";
    walkStates[4] = "right-stop";
    walkStates[5] = "left-stop";
    walkStates[6] = "up-stop";
    walkStates[7] = "down-stop";
    map = new tmx::MapLoader("data/maps");
    map->Load("mapa.tmx");

    dirx = 0; // sprite direction: right (1), left (-1)
    diry = 0; // down (1), up (-1)

    im = cgf::InputManager::instance();

    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("up", sf::Keyboard::Up);
    im->addKeyInput("down", sf::Keyboard::Down);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("stats", sf::Keyboard::S);
    im->addMouseInput("rightclick", sf::Mouse::Right);
    im->addKeyInput("reload", sf::Keyboard::R);

    player.load("data/player/mario.png",16,16,24,24,0,0,8,4,32);
    player.loadAnimation("data/player/mario.xml");

    princess.load("data/img/princesa.png");

    princess.setPosition(420, 60);
    princess.setScale(0.2, 0.2);


    enemy.load("data/img/enemies.png",42,42,8,8,0,7,6,3,18);
    enemy.loadAnimation("data/img/enemies.xml");
    enemy.setAnimation("move");
    enemy.setPosition(350,50);
    enemy.setScale(1.5,1.5);
    enemy.setAnimRate(5);
    enemy.play();

    player.setAnimation("right");
    player.setPosition(100,700);
    player.setAnimRate(30);
    player.setScale(1.5,1.5);
    player.play();

    dirx = 0; // sprite dir: right (1), left (-1)
    diry = 0; // down (1), up (-1)

    currentDir = DOWN;

    std::string scoreStr = std::to_string(score);
    timerText.setFont(gameFont);
    timerText.setString(scoreStr + " seconds");
    timerText.setPosition(690, 10);
    timerText.setColor(color.Magenta);
    timerText.setCharacterSize(20);


    finishGameText.setFont(gameFont);
    finishGameText.setPosition(250, 350);
    finishGameText.setCharacterSize(80);

    VICTORY_POSITION = 72;
    isWinner = false;
    isLoser = false;

	cout << "PlayState: Init" << endl;
}

void PlayState::wonGame(cgf::Game* game) {

    isWinner = true;
    enemy.setAnimation("die");
    levelSound.stop();
    winnerSound.play();
}

void PlayState::loseGame(cgf::Game* game) {

    isLoser = true;
    levelSound.stop();
    winnerSound.play();
}


bool PlayState::checkBarrelCollision(cgf::Sprite* player, cgf::Sprite barrel)
{
   return player->bboxCollision(barrel);
}

void PlayState::cleanup()
{
    delete map;
    barrels.clear();
	cout << "PlayState: Clean" << endl;
}

void PlayState::pause()
{
	cout << "PlayState: Paused" << endl;
}

void PlayState::resume()
{
	cout << "PlayState: Resumed" << endl;
}

void PlayState::handleEvents(cgf::Game* game)
{
    screen = game->getScreen();
    sf::View view = screen->getView();
    sf::Event event;

    while (screen->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            game->quit();
    }

    if (im->testEvent("reload"))
       game->changeState(MenuState::instance());

    if (isLoser || isWinner)
       return;

    dirx = diry = 0;

    int newDir = currentDir;

    if(im->testEvent("left")) {
        dirx = -1;
        newDir = LEFT;
    }

    if(im->testEvent("right")) {
        dirx = 1;
        newDir = RIGHT;
    }

    if(im->testEvent("up")){
        diry = -1;
        newDir = UP;
    }

    if(im->testEvent("down")) {
        diry = 1;
        newDir = DOWN;
    }
    if(dirx == 0 && diry == 0) {
        player.pause();
    }
    else {
        if(currentDir != newDir) {
            player.setAnimation(walkStates[newDir]);
            currentDir = newDir;
        }
        player.play();
    }

    player.setXspeed(100*dirx);
    player.setYspeed(100*diry);

    if(im->testEvent("quit") || im->testEvent("rightclick"))
        game->quit();

    if(im->testEvent("stats"))
        game->toggleStats();
}

void PlayState::update(cgf::Game* game)
{
    if (isWinner || isLoser)
        return;

    screen = game->getScreen();

    checkCollision(1, game, &player);

    enemy.update(game->getUpdateInterval());

    for (int i = 0; i < barrels.size(); i++)
    {
        if (checkBarrelCollision(&player, barrels[i]->getSprite())) {
            loseGame(game);
            break;
        }

        barrels[i]->getSprite().update(game->getUpdateInterval());
        barrels[i]->moving();

        if (barrels[i]->getSprite().getPosition().y > 800)
        {
            barrels.erase(barrels.begin() + i);
        }
    }

    if (barrels.size() < 30 && (rand() % 1000) > 950)
    {
        int randomPos = rand() % 25 + 1;
        barrels.push_back(new Barrel(randomPos * 32, 0));
    }

    time += clock.restart();
    score = static_cast<unsigned int>(time.asSeconds());

    if (player.getPosition().y < VICTORY_POSITION)
        wonGame(game);
}

void PlayState::draw(cgf::Game* game)
{
    screen = game->getScreen();
    map->Draw(*screen);
    screen->draw(player);
    screen-> draw(enemy);
    screen->draw(princess);

    for (int i = 0; i < barrels.size(); i++)
    {
        screen->draw(barrels[i]->getSprite());
    }

    std::string scoreStr = std::to_string(score);
    timerText.setString(scoreStr + " seconds");
    screen->draw(timerText);

    if (isWinner) {
        finishGameText.setColor(color.Blue);
        finishGameText.setString("YOU WIN! \n r to play again");
        levelSound.play();
        screen->draw(finishGameText);
    }

    if (isLoser) {
        finishGameText.setColor(color.Red);
        finishGameText.setString("YOU LOSE! \n r to play again");
        levelSound.play();
        screen->draw(finishGameText);
    }
}

bool PlayState::checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj)
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
    sf::Vector2u objsize = obj->getSize();
    objsize.x *= obj->getScale().x;
    objsize.y *= obj->getScale().y;

    float px = obj->getPosition().x;
    float py = obj->getPosition().y;

    double deltaTime = game->getUpdateInterval();

    sf::Vector2f offset(obj->getXspeed()/1000 * deltaTime, obj->getYspeed()/1000 * deltaTime);

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

    obj->setPosition(px+vx,py+vy);
    px = obj->getPosition().x;
    py = obj->getPosition().y;

    obj->update(deltaTime, false); // only update animation

    // Check collision with edges of map
    if (px < 0)
        obj->setPosition(px,py);
    else if (px + objsize.x >= mapsize.x * tilesize.x)
        obj->setPosition(mapsize.x*tilesize.x - objsize.x - 1,py);

    if(py < 0)
        obj->setPosition(px,0);
    else if(py + objsize.y >= mapsize.y * tilesize.y)
        obj->setPosition(px, mapsize.y*tilesize.y - objsize.y - 1);


    return bump;
}

// Get a cell GID from the map (x and y in global coords)
sf::Uint16 PlayState::getCellFromMap(uint8_t layernum, float x, float y)
{
    auto layers = map->GetLayers();
    tmx::MapLayer& layer = layers[layernum];
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2u tilesize = map->GetMapTileSize();
    mapsize.x /= tilesize.x;
    mapsize.y /= tilesize.y;
    int col = floor(x / tilesize.x);
    int row = floor(y / tilesize.y);
    return layer.tiles[row*mapsize.x + col].gid;
}
