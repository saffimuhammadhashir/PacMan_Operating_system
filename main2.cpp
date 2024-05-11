#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
//
#include <SFML/Graphics.hpp>
#include "pacman.h"
#include "maze.h"
#include "Food.h"
#include "highscore.h"
#include "Ghost.h"
#include "PowerPallet.h"
#include <cmath>
#include <time.h>

using namespace std;

const int Ghost_speed=5;

sem_t ghost_key,booster_key;

const int GG=5;
pthread_t threadsG[GG];
int valuesG[GG];
pthread_mutex_t busyG[GG];

pthread_t threadsBoost[GG];

void Gbakery_lock(int num){
    pthread_mutex_lock(&busyG[num]);
    int maxe=-1;
    for(int i=0;i<GG;i++){
        if(valuesG[i]>maxe){
            maxe=valuesG[i];
        }
    }
    maxe++;
    valuesG[num]=maxe;
    pthread_mutex_unlock(&busyG[num]);

    for(int i=0;i<GG;i++){
        //seeing it thread is busy assigning number
        pthread_mutex_lock(&busyG[i]);
        pthread_mutex_unlock(&busyG[i]);

        while((valuesG[i]!=0)&& (valuesG[i]<valuesG[num])){
            //waiting for others turn to happen
        }
    }

    cout<<"GThread "<<num<<" has entered the critical region\n";
}

void Gbakery_unlock(int num){
    cout<<"GThread "<<num<<" is leaving the critical region\n\n";
    valuesG[num]=0;
    usleep(10000);
}


const int Tnumb=5;
pthread_t threads[Tnumb];
int values[Tnumb];
pthread_mutex_t busy[Tnumb];

void bakery_lock(int num){
    pthread_mutex_lock(&busy[num]);
    int maxe=-1;
    for(int i=0;i<Tnumb;i++){
        if(values[i]>maxe){
            maxe=values[i];
        }
    }
    maxe++;
    values[num]=maxe;
    pthread_mutex_unlock(&busy[num]);

    for(int i=0;i<Tnumb;i++){
        //seeing it thread is busy assigning number
        pthread_mutex_lock(&busy[i]);
        pthread_mutex_unlock(&busy[i]);

        while((values[i]!=0)&& (values[i]<values[num])){
            //waiting for others turn to happen
        }
    }

    cout<<"Thread "<<num<<" has entered the critical region\n";
}

void bakery_unlock(int num){
    cout<<"Thread "<<num<<" is leaving the critical region\n\n";
    values[num]=0;
    usleep(10000);
}

sf::RenderWindow window2(sf::VideoMode(800, 600), "SFML Window");

class Game
{

public:
    RenderWindow window;
    Clock mouth_open, automoving, boostT, lifeT, ghostselfmove, movement, moveout, teleporting, critical_state_ghost, pallettimer, eatghosts,ghostkeyT;
    Pacman player;
    Maze maze;
    Food food;
    Event keypress;
    bool newEvent;
    Ghostlist ghosts;
    ScoreList highscore;
    PowerPallet pallet;
    bool activepallet;
    Font font;
    bool firstappearance = true;
    int previousmove = 0;
    int score = 0;
    string name = "Guest";
    int validitycount = 0;
    int level = 1;
    int targetx;
    int targety;
    bool ghostlaunched = false;
    bool teleported = false;
    int lives = 3;

    Game() : window(sf::VideoMode(1220, 800), "SFML works!")
    {
        font.loadFromFile("font/BebasNeue-Regular.ttf");
        highscore.readFromFile();
        for (int i = 0; i < GG; i++)
        {
            ghosts.create(i);
        }
        pallet.Visible = false;
        newEvent=0;
    }

    bool checkCollision(const Sprite &pacman, const Sprite *walls, int numWalls)
    {

        FloatRect pacmanBounds = pacman.getGlobalBounds();

        for (int i = 0; i < numWalls; i++)
        {

            FloatRect wallBounds = walls[i].getGlobalBounds();

            if (pacmanBounds.intersects(wallBounds))
            {
                return true;
            }
        }

        return false;
    }

    void palletcollection()
    {

        FloatRect pacmanBounds = player.pacman_S.getGlobalBounds();

        FloatRect wallBounds = pallet.pallet_s.getGlobalBounds();

        if (pacmanBounds.intersects(wallBounds) && pallet.Visible)
        {
            pallet.Visible = false;
            score += 100;
            pallettimer.restart();
            eatghosts.restart();
            ghosts.blink();
            activepallet = true;
            return;
        }
    }

    void move(Event &event)
    {
        int PosY = player.pacman_S.getPosition().y;
        int PosX = player.pacman_S.getPosition().x;
        int moveAmount = 5;

        if (event.key.code == sf::Keyboard::Up)
        {
            player.pacman_S.setPosition(PosX, PosY - 10);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical) &&
                !checkCollision(player.pacman_S, maze.wall_s_leftbottom, maze.n_leftbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_lefttop, maze.n_lefttop) &&
                !checkCollision(player.pacman_S, maze.wall_s_rightbottom, maze.n_rightbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_righttop, maze.n_righttop))
            {
                maze.move(0, moveAmount);
                food.move(0, moveAmount);
                ghosts.updatechange(0, moveAmount);
                pallet.move(0, moveAmount);
                previousmove = player.dir;
                player.dir = 2;
            }
        }
        else if (event.key.code == sf::Keyboard::Down)
        {
            player.pacman_S.setPosition(PosX, PosY + 10);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical) &&
                !checkCollision(player.pacman_S, maze.wall_s_leftbottom, maze.n_leftbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_lefttop, maze.n_lefttop) &&
                !checkCollision(player.pacman_S, maze.wall_s_rightbottom, maze.n_rightbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_righttop, maze.n_righttop))
            {
                maze.move(0, -moveAmount);
                food.move(0, -moveAmount);
                ghosts.updatechange(0, -moveAmount);
                pallet.move(0, -moveAmount);
                previousmove = player.dir;
                player.dir = 4;
            }
        }
        else if (event.key.code == sf::Keyboard::Left)
        {
            player.pacman_S.setPosition(PosX - 10, PosY);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical) &&
                !checkCollision(player.pacman_S, maze.wall_s_leftbottom, maze.n_leftbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_lefttop, maze.n_lefttop) &&
                !checkCollision(player.pacman_S, maze.wall_s_rightbottom, maze.n_rightbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_righttop, maze.n_righttop))
            {
                maze.move(moveAmount, 0);
                food.move(moveAmount, 0);
                ghosts.updatechange(moveAmount, 0);
                pallet.move(moveAmount, 0);
                previousmove = player.dir;
                player.dir = 3;
            }
        }
        else if (event.key.code == sf::Keyboard::Right)
        {
            player.pacman_S.setPosition(PosX + 10, PosY);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical) &&
                !checkCollision(player.pacman_S, maze.wall_s_leftbottom, maze.n_leftbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_lefttop, maze.n_lefttop) &&
                !checkCollision(player.pacman_S, maze.wall_s_rightbottom, maze.n_rightbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_righttop, maze.n_righttop))
            {
                maze.move(-moveAmount, 0);
                food.move(-moveAmount, 0);
                ghosts.updatechange(-moveAmount, 0);
                pallet.move(-moveAmount, 0);
                previousmove = player.dir;
                player.dir = 1;
            }
        }
        else if (event.key.code == sf::Keyboard::Escape)
        {
            exit(0);
        }
        player.pacman_S.setPosition(PosX, PosY);
    }

    void displayTopThreeScores()
    {
        Text text;
        text.setFont(font);
        text.setCharacterSize(30);
        text.setFillColor(Color::White);

        float x = 965.0f;
        float y = 580.0f;
        score_body *temp = highscore.head;
        int count = 0;
        bool done_display = false;
        string scoreText = "";
        while (temp != nullptr && count < 5)
        {

            if (temp->score < score && !done_display)
            {
                done_display = true;
                scoreText = to_string(count + 1) + ".  " + "You" + "(" + to_string(score) + ")";
                text.setFillColor(Color::Red);
            }
            else
            {
                scoreText = to_string(count + 1) + ".  " + temp->name + "(" + to_string(temp->score) + ")";
                if (count == 0)
                {
                    text.setFillColor(Color::Blue);
                }
                else if (count == 1)
                {
                    text.setFillColor(Color::Green);
                }
                else if (count == 2)
                {
                    text.setFillColor(Color::Yellow);
                }
                else
                {
                    text.setFillColor(Color::White);
                }
                temp = temp->next;
            }

            text.setString(scoreText);

            text.setPosition(x, y);

            window.draw(text);

            y += 30.f;

            count++;
        }
    }
    bool teleport(const Sprite &pacman, const Sprite portal)
    {

        FloatRect pacmanBounds = pacman.getGlobalBounds();

        FloatRect wallBounds = portal.getGlobalBounds();

        if (pacmanBounds.intersects(wallBounds))
        {
            return true;
        }

        return false;
    }


    //automove
    void automove()
    {

        int PosY = player.pacman_S.getPosition().y;
        int PosX = player.pacman_S.getPosition().x;
        int moveAmount = 10;

        if (player.dir == 2)
        {
            player.pacman_S.setPosition(PosX, PosY - 10);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical) &&
                !checkCollision(player.pacman_S, maze.wall_s_leftbottom, maze.n_leftbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_lefttop, maze.n_lefttop) &&
                !checkCollision(player.pacman_S, maze.wall_s_rightbottom, maze.n_rightbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_righttop, maze.n_righttop))
            {
                maze.move(0, moveAmount);
                food.move(0, moveAmount);
                ghosts.updatechange(0, moveAmount);
                pallet.move(0, moveAmount);
                previousmove = player.dir;
                player.dir = 2;
            }
        }
        else if (player.dir == 4)
        {
            player.pacman_S.setPosition(PosX, PosY + 10);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical) &&
                !checkCollision(player.pacman_S, maze.wall_s_leftbottom, maze.n_leftbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_lefttop, maze.n_lefttop) &&
                !checkCollision(player.pacman_S, maze.wall_s_rightbottom, maze.n_rightbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_righttop, maze.n_righttop))
            {
                maze.move(0, -moveAmount);
                food.move(0, -moveAmount);
                ghosts.updatechange(0, -moveAmount);
                pallet.move(0, -moveAmount);
                previousmove = player.dir;
                player.dir = 4;
            }
        }
        else if (player.dir == 3)
        {
            player.pacman_S.setPosition(PosX - 10, PosY);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical) &&
                !checkCollision(player.pacman_S, maze.wall_s_leftbottom, maze.n_leftbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_lefttop, maze.n_lefttop) &&
                !checkCollision(player.pacman_S, maze.wall_s_rightbottom, maze.n_rightbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_righttop, maze.n_righttop))
            {
                maze.move(moveAmount, 0);
                food.move(moveAmount, 0);
                ghosts.updatechange(moveAmount, 0);
                pallet.move(moveAmount, 0);
                previousmove = player.dir;
                player.dir = 3;
            }
        }
        else if (player.dir = 1)
        {
            player.pacman_S.setPosition(PosX + 10, PosY);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical) &&
                !checkCollision(player.pacman_S, maze.wall_s_leftbottom, maze.n_leftbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_lefttop, maze.n_lefttop) &&
                !checkCollision(player.pacman_S, maze.wall_s_rightbottom, maze.n_rightbottom) &&
                !checkCollision(player.pacman_S, maze.wall_s_righttop, maze.n_righttop))
            {
                maze.move(-moveAmount, 0);
                food.move(-moveAmount, 0);
                ghosts.updatechange(-moveAmount, 0);
                pallet.move(-moveAmount, 0);
                previousmove = player.dir;
                player.dir = 1;
            }
        }

        if (teleport(player.pacman_S, maze.Teleportationx))
        {
            if (rand() % 2 == 0)
            {
                maze.move(-550, 430);
                food.move(-550, 430);
                ghosts.updatechange(-550, 430);
                pallet.move(-550, 430);
                previousmove = player.dir;
                player.dir = 3;
                teleported = true;
                teleporting.restart();
            }
            else
            {
                maze.move(+550, 430);
                food.move(+550, 430);
                ghosts.updatechange(+550, 430);
                pallet.move(+550, 430);
                previousmove = player.dir;
                player.dir = 1;
                teleported = true;
                teleporting.restart();
            }
        }
        else if (teleport(player.pacman_S, maze.Teleportationy1))
        {
            maze.move(-1240, 0);
            food.move(-1240, 0);
            ghosts.updatechange(-1240, 0);
            pallet.move(-1240, 0);
            previousmove = player.dir;
            player.dir = 3;
            teleported = true;
            teleporting.restart();
        }
        else if (teleport(player.pacman_S, maze.Teleportationy2))
        {
            maze.move(1240, 0);
            food.move(1240, 0);
            ghosts.updatechange(1240, 0);
            pallet.move(1240, 0);
            previousmove = player.dir;
            player.dir = 1;
            teleported = true;
            teleporting.restart();
        }

        player.pacman_S.setPosition(PosX, PosY);
    }

    void checkfood()
    {
        int index = checkCollisionfood(player.pacman_S, food.food_S, food.n_food);
        if (index != -1)
        {
            food.valid[index] = 0;
            score += 50;
            validitycount++;
            if (validitycount == food.n_food - 1)
            {
                for (int i = 0; i < food.n_food; i++)
                {
                    food.valid[i] = 1;
                }
                validitycount = 0;
                level++;
            }
        }
    }
    int checkCollisionfood(const Sprite &pacman, const Sprite *walls, int numWalls)
    {

        FloatRect pacmanBounds = pacman.getGlobalBounds();

        for (int i = 0; i < numWalls; i++)
        {

            FloatRect wallBounds = walls[i].getGlobalBounds();

            if (pacmanBounds.intersects(wallBounds) && food.valid[i] != 0)
            {
                if (!firstappearance)
                {
                    firstappearance = true;
                    pallettimer.restart();
                }
                return i;
            }
        }

        return -1;
    }
    bool ghostcheck(Ghost *org)
    {

        if (ghosts.start)
        {
            FloatRect pacmanBounds = org->ghost_s.getGlobalBounds();
            Ghost *curr = ghosts.start;
            while (curr)
            {
                if (org != curr)
                {
                    FloatRect wallBounds = curr->ghost_s.getGlobalBounds();
                    if (pacmanBounds.intersects(wallBounds))
                    {
                        return true;
                    }
                }
                curr = curr->next;
            }
        }
        if (org->ghost_s.getPosition().x > maze.centerx - 100 && org->ghost_s.getPosition().x < maze.centerx + 100 && org->ghost_s.getPosition().y < maze.centery + 80 && org->ghost_s.getPosition().y > maze.centery + 20 && !org->home)
        {
            return true;
        }
        return false;
    }
    void movementdetector()
    {
        bool wallfound = false;
        int PosY = player.pacman_S.getPosition().y;
        int PosX = player.pacman_S.getPosition().x;

        while (!wallfound)
        {
            if (player.dir == 1)
            {
                player.pacman_S.setPosition(player.pacman_S.getPosition().x + 10, PosY);
            }
            else if (player.dir == 3)
            {
                player.pacman_S.setPosition(player.pacman_S.getPosition().x - 10, PosY);
            }
            else if (player.dir == 2)
            {
                player.pacman_S.setPosition(PosX, player.pacman_S.getPosition().y - 10);
            }
            else
            {
                player.pacman_S.setPosition(PosX, player.pacman_S.getPosition().y + 10);
            }

            if (checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) ||
                checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical) ||
                checkCollision(player.pacman_S, maze.wall_s_leftbottom, maze.n_leftbottom) ||
                checkCollision(player.pacman_S, maze.wall_s_lefttop, maze.n_lefttop) ||
                checkCollision(player.pacman_S, maze.wall_s_rightbottom, maze.n_rightbottom) ||
                checkCollision(player.pacman_S, maze.wall_s_righttop, maze.n_righttop) ||
                teleport(player.pacman_S, maze.Teleportationx) || teleport(player.pacman_S, maze.Teleportationy1) || teleport(player.pacman_S, maze.Teleportationy2))
            {
                wallfound = true;
                targetx = PosX;
                targety = PosY;
            }
        }
        player.pacman_S.setPosition(PosX, PosY);
    }

    void chase(Ghost *curr)
    {
        moveHorizontally(curr);

        //moveVertically(curr);
    }

    void resolvestatic(Ghost *curr)
    {
        int PosY = curr->ghost_s.getPosition().y;
        int PosX = curr->ghost_s.getPosition().x;
        int orgposY = PosY;
        int orgposX = PosX;
        if (curr->dir != 1 && curr->dir != 2 && curr->dir != 3 && curr->dir != 4)
        {
            curr->dir = rand() % 3 + 1;
        }
        if (curr->dir == 1)
        {
            PosX++;
        }
        else if (curr->dir == 2)
        {
            PosY--;
        }
        else if (curr->dir == 3)
        {
            PosX--;
        }
        else if (curr->dir == 4)
        {
            PosY++;
        }
        curr->ghost_s.setPosition(PosX, PosY);
        if (!checkCollision(curr->ghost_s, maze.wall_s_horizontal, maze.n_horizontal) &&
            !checkCollision(curr->ghost_s, maze.wall_s_vertical, maze.n_vertical) &&
            !checkCollision(curr->ghost_s, maze.wall_s_leftbottom, maze.n_leftbottom) &&
            !checkCollision(curr->ghost_s, maze.wall_s_lefttop, maze.n_lefttop) &&
            !checkCollision(curr->ghost_s, maze.wall_s_rightbottom, maze.n_rightbottom) &&
            !checkCollision(curr->ghost_s, maze.wall_s_righttop, maze.n_righttop) &&
            !ghostcheck(curr))
        {
            return;
        }
        else
        {
            curr->ghost_s.setPosition(orgposX, orgposY);
            curr->dir++;
        }
    }

    void hunt(int i)
    {
        if (ghosts.start && ghosts.huntT>0)
        {
            ghosts.huntT-=1;
            Ghost *curr = &ghosts.start[i];

            if (!curr->criticalstate && !curr->home)
            {
                curr->prevx = curr->ghost_s.getPosition().x;
                curr->prevy = curr->ghost_s.getPosition().y;
                chase(curr);
                if (curr->ghost_s.getPosition().x == curr->prevx && abs(curr->ghost_s.getPosition().y - curr->prevy) < 2)
                {
                    curr->criticalstate = true;
                    critical_state_ghost.restart();
                }
            }
            else
            {
                resolvestatic(curr);
            }

        }

    }

    bool moveHorizontally(Ghost *curr)
    {
        int PosY = curr->ghost_s.getPosition().y;
        int PosX = curr->ghost_s.getPosition().x;
        int orgposY = PosY;
        int orgposX = PosX;

        if (targetx > PosX)
        {
            PosX += 2;
            if(curr->Bactive){
                PosX+=Ghost_speed;
            }
        }
        else if (targetx < PosX)
        {
            PosX -= 2;
            if(curr->Bactive){
                PosX-=Ghost_speed;
            }
        }
        else
        {
            if (targety > PosY)
            {
                PosY += 2;
                if(curr->Bactive){
                    PosY+=Ghost_speed;
                }
            }
            else if (targety < PosY)
            {
                PosY -= 2;
                if(curr->Bactive){
                    PosY-=Ghost_speed;
                }
            }
            
            curr->ghost_s.setPosition(orgposX, PosY);
            if (!checkCollision(curr->ghost_s, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(curr->ghost_s, maze.wall_s_vertical, maze.n_vertical) &&
                !checkCollision(curr->ghost_s, maze.wall_s_leftbottom, maze.n_leftbottom) &&
                !checkCollision(curr->ghost_s, maze.wall_s_lefttop, maze.n_lefttop) &&
                !checkCollision(curr->ghost_s, maze.wall_s_rightbottom, maze.n_rightbottom) &&
                !checkCollision(curr->ghost_s, maze.wall_s_righttop, maze.n_righttop) &&
                !ghostcheck(curr))
            {
                curr->ghost_s.setPosition(PosX, PosY);
                return true;
            }
            curr->ghost_s.setPosition(orgposX, orgposY);
            return false;
        }
        curr->ghost_s.setPosition(PosX, orgposY);
        if (!checkCollision(curr->ghost_s, maze.wall_s_horizontal, maze.n_horizontal) &&
            !checkCollision(curr->ghost_s, maze.wall_s_vertical, maze.n_vertical) &&
            !checkCollision(curr->ghost_s, maze.wall_s_leftbottom, maze.n_leftbottom) &&
            !checkCollision(curr->ghost_s, maze.wall_s_lefttop, maze.n_lefttop) &&
            !checkCollision(curr->ghost_s, maze.wall_s_rightbottom, maze.n_rightbottom) &&
            !checkCollision(curr->ghost_s, maze.wall_s_righttop, maze.n_righttop) &&
            !ghostcheck(curr))
        {
            curr->ghost_s.setPosition(PosX, PosY);
            return true;
        }
        curr->ghost_s.setPosition(orgposX, orgposY);
        return false;
    }

    bool moveVertically(Ghost *curr)
    {
        int PosY = curr->ghost_s.getPosition().y;
        int PosX = curr->ghost_s.getPosition().x;
        int orgposY = PosY;
        int orgposX = PosX;

        if (targety > PosY)
        {
            PosY += 2;
        }
        else if (targety < PosY)
        {
            PosY -= 2;
        }
        else
        {
            if (targetx > PosX)
            {
                PosX += 2;
            }
            else if (targetx < PosX)
            {
                PosX -= 2;
            }
            curr->ghost_s.setPosition(PosX, orgposY);
            if (!checkCollision(curr->ghost_s, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(curr->ghost_s, maze.wall_s_vertical, maze.n_vertical) &&
                !checkCollision(curr->ghost_s, maze.wall_s_leftbottom, maze.n_leftbottom) &&
                !checkCollision(curr->ghost_s, maze.wall_s_lefttop, maze.n_lefttop) &&
                !checkCollision(curr->ghost_s, maze.wall_s_rightbottom, maze.n_rightbottom) &&
                !checkCollision(curr->ghost_s, maze.wall_s_righttop, maze.n_righttop) &&
                !ghostcheck(curr))
            {
                curr->ghost_s.setPosition(PosX, PosY);
                return true;
            }
            curr->ghost_s.setPosition(orgposX, orgposY);
            return false;
        }
        curr->ghost_s.setPosition(orgposX, PosY);
        if (!checkCollision(curr->ghost_s, maze.wall_s_horizontal, maze.n_horizontal) &&
            !checkCollision(curr->ghost_s, maze.wall_s_vertical, maze.n_vertical) &&
            !checkCollision(curr->ghost_s, maze.wall_s_leftbottom, maze.n_leftbottom) &&
            !checkCollision(curr->ghost_s, maze.wall_s_lefttop, maze.n_lefttop) &&
            !checkCollision(curr->ghost_s, maze.wall_s_rightbottom, maze.n_rightbottom) &&
            !checkCollision(curr->ghost_s, maze.wall_s_righttop, maze.n_righttop) &&
            !ghostcheck(curr))
        {
            curr->ghost_s.setPosition(PosX, PosY);
            return true;
        }
        curr->ghost_s.setPosition(orgposX, orgposY);
        return false;
    }

    void moveghostsout(int i)
    {
        int key_c=0;
        while(key_c<2){
            sem_wait(&ghost_key);
            key_c++;
            usleep(10000);
        }
        ghosts.outside+=1;
        if (ghosts.start)
        {
            Ghost *curr = &(ghosts.start[i]);

            curr->ghost_s.setPosition(maze.centerx+20 - 30 * i, maze.centery - 5*(rand()%5));
            curr->dir=rand()%4+1;
            curr->home = false;

        }
    }

    void moveghostsout1(int i)
    {
        if (ghosts.start)
        {
            Ghost *curr = &(ghosts.start[i]);
            if(curr->home){
                curr->ghost_s.setPosition(maze.centerx+20 - 30 * i, maze.centery - 5*(rand()%5));
                curr->dir=rand()%4+1;
                curr->home = false;
            }


        }
    }

    void deploypallet()
    {
        while (!pallet.Visible)
        {

            for (int i = 0; i < food.n_food; i++)
            {

                    if (rand() % food.n_food == 0)
                    {
                        pallet.Visible = true;
                        pallet.pallet_s.setPosition(food.food_coords[i][0] - 15, food.food_coords[i][1] - 15);
                        return;
                    }
                }
            
        }
    }

    bool enemy_pacman(Ghost *curr)
    {
        FloatRect pacmanBounds = player.pacman_S.getGlobalBounds();
        FloatRect wallBounds = curr->ghost_s.getGlobalBounds();
        if (pacmanBounds.intersects(wallBounds))
        {
            return true;
        }
        return false;
    }

    void reconsider_critical_state_ghost()
    {
        if (ghosts.start)
        {
            for(int i=0;i<ghosts.Gsize;i++){
                Ghost *curr = ghosts.start;
                curr->criticalstate = false;
            }

        }
    }

    void pacman_ghost_collision(int i)
    {
        Ghost *curr = &ghosts.start[i];
            if (enemy_pacman(curr))
            {
                if (activepallet)
                {
                    if (curr->mytype == 0)
                    {
                        curr->ghost_t.loadFromFile("images/ghost1.png");
                    }
                    else if (curr->mytype == 1)
                    {
                        curr->ghost_t.loadFromFile("images/ghost2.png");
                    }
                    else
                    {
                        curr->ghost_t.loadFromFile("images/ghost3.png");
                    }
                    curr->ghost_s.setTexture(curr->ghost_t);
                    curr->ghost_s.setPosition(curr->mytype * 80 + maze.centerx - 100, curr->myytype * 60 + maze.centery + 100);
                    curr->home = true;
                    score += 150;
                }
                else
                {
                    Ghost *newcurr = ghosts.start;
                    while (newcurr)
                    {
                        newcurr->ghost_s.setPosition(newcurr->mytype * 80 + maze.centerx - 100, newcurr->myytype * 60 + maze.centery + 100);
                        newcurr->home = true;
                        newcurr = newcurr->next;
                    }
                    if(lifeT.getElapsedTime().asSeconds()>=2){
                        lives--;
                        lifeT.restart();
                    }
                    
                    return;
                }
            }
    }

    //interface
    void interface_Game(int Tnum){
        while (window.isOpen()){
            //bakery_lock(Tnum);
                window.clear();
                window.draw(player.pacman_S);

                float time_left = pallettimer.getElapsedTime().asSeconds();
                if (!firstappearance)
                {
                    time_left = 15.0;
                }
                float val = (time_left) / 15.0;
                if (val >= 1 && !pallet.Visible)
                {
                    val = 0;
                }
                else if (val >= 1 && pallet.Visible)
                {
                    val = 1;
                }
                food.display(window);
                //cout<<"before ghosts\n";
                ghosts.display(window);
                //cout<<"after ghosts\n";
                pallet.display(window);
                maze.display(window, level, validitycount, food.n_food, name, val, lives);
                displayTopThreeScores();
                window.display();
                usleep(10000);
            //bakery_unlock(Tnum);
        }
    }
    //keypress
    void input_Game(int Tnum){
        while (window.isOpen()){
            Event event;
            while (window.pollEvent(event))
            {
                bakery_lock(Tnum);
                    keypress=event;
                    newEvent=1;
                bakery_unlock(Tnum);
            }
        }
    }
    //engine
    void engine_Game(int Tnum){
        while (window.isOpen()){

            //keypress check
            bakery_lock(Tnum);
                if(newEvent==1){
                    newEvent=0;
                    if (keypress.type == Event::Closed)
                        window.close();
                    else if (keypress.type == Event::KeyPressed)
                    {
                        move(keypress);
                        player.face_movement(1);
                    }
                }
                //when inside cage
                //cout<<"outside: "<<ghosts.outside<<endl;
                if(ghosts.outside<GG){
                    for(int i=0;i<GG;i++){
                        if(ghosts.start[i].home==1){
                            ghosts.selfmove1(i);
                            ghosts.move1(i);
                        }

                    }
                }
                if(ghosts.outside>0){
                    ghostlaunched = true;
                }
                if(ghosts.outside>=GG){
                    for(int i=0;i<GG;i++){
                        moveghostsout1(i);
                    }
                }
                //boost
                if (boostT.getElapsedTime().asSeconds() >= 10)
                {
                    ghosts.removeactive();
                    boostT.restart();
                }

                //pacman mouth move
                if (mouth_open.getElapsedTime().asSeconds() >= 0.4)
                {
                    player.face_movement(0);
                    mouth_open.restart();
                }
                //pacman automove
                if (automoving.getElapsedTime().asSeconds() >= 0.03)
                {
                    automove();
                    automoving.restart();
                }
                //eat plate

                //something related to teleporting idk
                if (teleporting.getElapsedTime().asSeconds() >= 0.8f && teleported)
                {
                    teleported = false;
                }
                //self move on ghosts idk what this does either
                if (ghostselfmove.getElapsedTime().asSeconds() >= 0.06)
                {
                    ghosts.updateself();
                    ghostselfmove.restart();
                }
                //key released
                int value;
                sem_getvalue(&ghost_key, &value);
                if(ghostkeyT.getElapsedTime().asSeconds() >=1.5 && value==0){
                    sem_post(&ghost_key);
                    ghostkeyT.restart();
                }
                //ghost movement
                if (movement.getElapsedTime().asSeconds() > 0.0405f)
                {
                    movement.restart();
                }
                if (movement.getElapsedTime().asSeconds() > 0.010f && !teleported && ghostlaunched)
                {
                    if (previousmove != player.dir)
                    {
                        movementdetector();
                    }
                    ghosts.updatehunt();
                }
                //lives
                if (lives <= 0)
                {
                    highscore.insert(score, name);
                    return;
                }
            bakery_unlock(Tnum);
        }
    }
    //food consumption
    void food_Game(int Tnum){
        while (window.isOpen()){
            bakery_lock(Tnum);
                checkfood();
            bakery_unlock(Tnum);
        }
    }
    //ghosts
    void ghost_Game(int Tnum){
        moveghostsout(Tnum);
        while (window.isOpen()){
            
            Gbakery_lock(Tnum);
                ghosts.selfmove(Tnum);
                ghosts.move(Tnum);
                hunt(Tnum);
                pacman_ghost_collision(Tnum);
            Gbakery_unlock(Tnum);


        }
    }
    //boost
    void boost_Game(int Tnum){
        while (window.isOpen()){
            
            sem_wait(&booster_key);

            ghosts.start[Tnum].makeactive();
            while(ghosts.start[Tnum].Bactive){
                sleep(1);
            }

            sem_post(&booster_key);
            ghosts.start[Tnum].makeinactive();
            usleep(10000);
        }
    }
    //pallet
    void pallet_Game(int Tnum){

        while (window.isOpen()){
            
            bakery_lock(Tnum);
                palletcollection();
                //pallets
                if (pallettimer.getElapsedTime().asSeconds() >= 15.0f && !pallet.Visible)
                {
                    deploypallet();
                }
                //gaari ke blinker
                if (eatghosts.getElapsedTime().asSeconds() > 9.0f && eatghosts.getElapsedTime().asSeconds() < 9.3f)
                {
                    ghosts.unblink();
                    activepallet = false;
                }
                //pata nahi kia
                if (critical_state_ghost.getElapsedTime().asSeconds() >= 0.5f)
                {
                    critical_state_ghost.restart();
                    reconsider_critical_state_ghost();
                }
                
            bakery_unlock(Tnum);
        }
    }
};

Game G;

void* engine_t(void*arg){
    int Tnum=*((int*)arg);
        G.engine_Game(Tnum);
    pthread_exit(NULL);
}

void* interface_t(void*arg){
    int Tnum=*((int*)arg);
        G.interface_Game(Tnum);
    pthread_exit(NULL);
}

void* input_t(void*arg){
    int Tnum=*((int*)arg);
        G.input_Game(Tnum);
    pthread_exit(NULL);
}

void* food_t(void*arg){
    int Tnum=*((int*)arg);
        G.food_Game(Tnum);
    pthread_exit(NULL);
}

void* pallet_t(void*arg){
    int Tnum=*((int*)arg);
        G.pallet_Game(Tnum);
    pthread_exit(NULL);
}

void* ghost_t(void*arg){
    int Tnum=*((int*)arg);
        G.ghost_Game(Tnum);
    pthread_exit(NULL);
}

void* boost_t(void*arg){
    int Tnum=*((int*)arg);
        G.boost_Game(Tnum);
    pthread_exit(NULL);
}


int main(){

    for(int i=0;i<Tnumb;i++){
        pthread_mutex_init(&busy[i],NULL);
        values[i]=0;
    }

    for(int i=0;i<GG;i++){
        pthread_mutex_init(&busyG[i],NULL);
        valuesG[i]=0;
    }

    sem_init(&ghost_key,0,1);
    sem_init(&booster_key,0,2);

    srand(time(0));
    //interface
    pthread_create(&threads[0],NULL,interface_t,(void*)(new int(0)));
    pthread_create(&threads[1],NULL,input_t,(void*)(new int(1)));
    pthread_create(&threads[2],NULL,engine_t,(void*)(new int(2)));
    pthread_create(&threads[3],NULL,food_t,(void*)(new int(3)));
    pthread_create(&threads[4],NULL,pallet_t,(void*)(new int(4)));

    for(int i=0;i<GG;i++){
        pthread_create(&threadsG[i],NULL,ghost_t,(void*)(new int(i)));
        pthread_create(&threadsBoost[i],NULL,boost_t,(void*)(new int(i)));
    }
    //main thread
    while (window2.isOpen()){}




}