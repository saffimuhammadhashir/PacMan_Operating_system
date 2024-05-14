#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <SFML/Audio.hpp>
#include <cmath>
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
#include <menu.h>
using namespace std;

const int Ghost_speed = 1;

sem_t ghost_key, booster_key;

const int GG = 5;
pthread_t threadsG[GG];
int valuesG[GG];
pthread_mutex_t busyG[GG];

pthread_t threadsBoost[GG];

void Gbakery_lock(int num)
{
    pthread_mutex_lock(&busyG[num]);
    int maxe = -1;
    for (int i = 0; i < GG; i++)
    {
        if (valuesG[i] > maxe)
        {
            maxe = valuesG[i];
        }
    }
    maxe++;
    valuesG[num] = maxe;
    pthread_mutex_unlock(&busyG[num]);

    for (int i = 0; i < GG; i++)
    {
        // seeing it thread is busy assigning number
        pthread_mutex_lock(&busyG[i]);
        pthread_mutex_unlock(&busyG[i]);

        while ((valuesG[i] != 0) && (valuesG[i] < valuesG[num]))
        {
            // waiting for others turn to happen
        }
    }

    cout << "GThread " << num << " has entered the critical region\n";
}

void Gbakery_unlock(int num)
{
    cout << "GThread " << num << " is leaving the critical region\n\n";
    valuesG[num] = 0;
    usleep(10000);
}

const int Tnumb = 6;
pthread_t threads[Tnumb];
int values[Tnumb];
pthread_mutex_t busy[Tnumb];

void bakery_lock(int num)
{
    pthread_mutex_lock(&busy[num]);
    int maxe = -1;
    for (int i = 0; i < Tnumb; i++)
    {
        if (values[i] > maxe)
        {
            maxe = values[i];
        }
    }
    maxe++;
    values[num] = maxe;
    pthread_mutex_unlock(&busy[num]);

    for (int i = 0; i < Tnumb; i++)
    {
        // seeing it thread is busy assigning number
        pthread_mutex_lock(&busy[i]);
        pthread_mutex_unlock(&busy[i]);

        while ((values[i] != 0) && (values[i] < values[num]))
        {
            // waiting for others turn to happen
        }
    }

    cout << "Thread " << num << " has entered the critical region\n";
}

void bakery_unlock(int num)
{

    cout << "Thread " << num << " is leaving the critical region\n\n";
    values[num] = 0;
    usleep(10000);
}

// sf::RenderWindow window2(sf::VideoMode(800, 600), "SFML Window");

class Game
{

public:
    RenderWindow window;
    Clock mouth_open, Tautomove, automoving, boostT, lifeT, ghostselfmove, movement, moveout, teleporting, critical_state_ghost, pallettimer, eatghosts, ghostkeyT;
    Pacman player;
    Maze maze;
    Food food;
    Event keypress;
    bool newEvent;

    bool automover;
    Event autopress;
    int autod;
    bool autop;
    bool findnext;
    Sprite oldsprite;
    Clock oldT;
    Clock StuckT;

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
    bool activestate = true;
    Texture invincibility, ring, Super_powerT;
    Sprite invincibilityS, ringS, Super_powerS;

    Text inv, AImover;
    bool firstdead = false;
    Game()
    {
        font.loadFromFile("font/BebasNeue-Regular.ttf");
        highscore.readFromFile();
        for (int i = 0; i < GG; i++)
        {
            ghosts.create(i);
        }
        inv.setString("Invincibility");
        inv.setFont(font);
        inv.setPosition(1000, 20);
        inv.setCharacterSize(20);
        inv.setFillColor(Color::Red);
        pallet.Visible = false;
        invincibility.loadFromFile("images/Highscore.png");
        invincibilityS.setTexture(invincibility);
        invincibilityS.setScale(0.4, 0.5);
        invincibilityS.setPosition(910, 50);

        Super_powerT.loadFromFile("images/buttons.png");
        Super_powerS.setTexture(Super_powerT);
        Super_powerS.setScale(0.5, 0.4);
        Super_powerS.setPosition(650, 600);

        AImover.setFont(font);
        AImover.setCharacterSize(32);
        AImover.setFillColor(Color::Black);
        AImover.setString("AUTO MOVER ACTIVE!");
        AImover.setPosition(680, 680);

        automover = 0;
        autod = 0;
        autop = 0;
        findnext = 1;

        ring.loadFromFile("images/ring.png");
        ringS.setTexture(ring);
        ringS.setScale(0.1, 0.1);
        ringS.setPosition(player.pacman_S.getPosition().x - 9, player.pacman_S.getPosition().y - 9);
        newEvent = 0;
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

        if (pacman.getPosition().x > maze.centerx - 100 && pacman.getPosition().x < maze.centerx + 100 && pacman.getPosition().y < maze.centery + 80 && pacman.getPosition().y > maze.centery + 20)
        {
            return true;
        }
        return false;
    }

    bool checkCollision2(const Sprite &pacman, const Sprite *walls, int numWalls)
    {
        for (int i = 0; i < numWalls; i++)
        {
            if (checkOverlap(pacman, walls[i]))
            {
                return true;
            }
        }
        return false;
    }
    bool checkOverlap(const Sprite &sprite1, const Sprite &sprite2)
    {
        FloatRect bounds1 = sprite1.getGlobalBounds();
        FloatRect bounds2 = sprite2.getGlobalBounds();

        return bounds1.intersects(bounds2);
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
        int moveAmount = 10;

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
                ghosts.allchange(0, moveAmount);
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
                ghosts.allchange(0, -moveAmount);
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
                ghosts.allchange(moveAmount, 0);
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
                ghosts.allchange(-moveAmount, 0);
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

    // automove
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
                ghosts.allchange(0, moveAmount);
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
                ghosts.allchange(0, -moveAmount);
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
                ghosts.allchange(moveAmount, 0);
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
                ghosts.allchange(-moveAmount, 0);
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
                ghosts.allchange(-550, 430);
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
                ghosts.allchange(+550, 430);
                pallet.move(+550, 430);
                previousmove = player.dir;
                player.dir = 1;
                teleported = true;
                teleporting.restart();
            }
            // moveghostoutofwall();
        }
        else if (teleport(player.pacman_S, maze.Teleportationy1))
        {
            maze.move(-1240, 0);
            food.move(-1240, 0);
            ghosts.allchange(-1240, 0);
            pallet.move(-1240, 0);
            previousmove = player.dir;
            player.dir = 3;
            teleported = true;
            teleporting.restart();
            // moveghostoutofwall();
        }
        else if (teleport(player.pacman_S, maze.Teleportationy2))
        {
            maze.move(1240, 0);
            food.move(1240, 0);
            ghosts.allchange(1240, 0);
            pallet.move(1240, 0);
            previousmove = player.dir;
            player.dir = 1;
            teleported = true;
            teleporting.restart();
            // moveghostoutofwall();
        }

        player.pacman_S.setPosition(PosX, PosY);
    }

    void checkfood()
    {
        int index = checkCollisionfood(player.pacman_S, food.food_S, food.n_food);
        if (index != -1)
        {
            food.valid[index] = 0;
            Tautomove.restart();
            findnext = 1;
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
        int x = pacman.getPosition().x;

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

        moveVertically(curr);
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
        if (ghosts.start && ghosts.huntT > 0)
        {
            ghosts.huntT -= 1;
            Ghost *curr = &ghosts.start[i];

            // if (!curr->criticalstate && !curr->home)
            // {
            curr->prevx = curr->ghost_s.getPosition().x;
            curr->prevy = curr->ghost_s.getPosition().y;
            chase(curr);
            if (curr->ghost_s.getPosition().x == curr->prevx && abs(curr->ghost_s.getPosition().y - curr->prevy) < 2)
            {
                curr->criticalstate = true;
                critical_state_ghost.restart();
            }
            // }
            // else
            // {
            //     resolvestatic(curr);
            // }
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
            if (curr->Bactive)
            {
                PosX += Ghost_speed;
            }
        }
        else if (targetx < PosX)
        {
            PosX -= 2;
            if (curr->Bactive)
            {
                PosX -= Ghost_speed;
            }
        }
        else
        {
            if (targety > PosY)
            {
                PosY += 2;
                if (curr->Bactive)
                {
                    PosY += Ghost_speed;
                }
            }
            else if (targety < PosY)
            {
                PosY -= 2;
                if (curr->Bactive)
                {
                    PosY -= Ghost_speed;
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
        int key_c = 0;
        while (key_c < 2)
        {
            sem_wait(&ghost_key);
            key_c++;
            usleep(10000);
        }
        ghosts.outside += 1;
        if (ghosts.start)
        {
            Ghost *curr = &(ghosts.start[i]);

            curr->ghost_s.setPosition(maze.centerx + 20 - 30 * i, maze.centery - 5 * (rand() % 5));
            curr->dir = rand() % 4 + 1;
            curr->home = false;
        }
    }

    void moveghostsout1(int i)
    {
        if (ghosts.start)
        {
            Ghost *curr = &(ghosts.start[i]);
            if (curr->home)
            {
                curr->ghost_s.setPosition(maze.centerx + 20 - 30 * i, maze.centery - 5 * (rand() % 5));
                curr->dir = rand() % 4 + 1;
                curr->home = false;
            }
        }
    }

    void moveghostoutofwall()
    {
        if (ghosts.start)
        {
            Ghost *curr = ghosts.start;
            while (curr)
            {

                curr->ghost_s.setPosition(maze.centerx + 20, maze.centery + 50);

                curr = curr->next;
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
            for (int i = 0; i < ghosts.Gsize; i++)
            {
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
                if (lifeT.getElapsedTime().asSeconds() >= 5 && !automover)
                {
                    Ghost *newcurr = &ghosts.start[i];
                    newcurr->ghost_s.setPosition(newcurr->mytype * 80 + maze.centerx - 100, newcurr->myytype * 60 + maze.centery + 100);
                    newcurr->home = true;

                    lives--;
                    lifeT.restart();
                    firstdead = true;
                }

                return;
            }
        }
    }

    // interface
    void interface_Game(int Tnum)
    {

        while (window.isOpen())
        {
            if (activestate)
            {
                // bakery_lock(Tnum);
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
                // cout<<"before ghosts\n";
                ghosts.display(window);
                // cout<<"after ghosts\n";
                pallet.display(window);
                maze.display(window, level, validitycount, food.n_food, name, val, lives);
                displayTopThreeScores();
                float vallife = 0.0f;
                if (lifeT.getElapsedTime().asSeconds() <= 5.0f && firstdead)
                {

                    vallife = lifeT.getElapsedTime().asSeconds() / 5.0f;
                    window.draw(ringS);
                }
                invincibilityS.setScale(vallife * 0.4, 0.5);

                window.draw(invincibilityS);
                window.draw(inv);
                if (automover)
                {

                    window.draw(Super_powerS);
                    window.draw(AImover);
                }
                window.display();
                usleep(10000);
            }
            // bakery_unlock(Tnum);
        }
    }
    // keypress
    void input_Game(int Tnum)
    {
        while (window.isOpen())
        {

            Event event;
            while (window.pollEvent(event))
            {
                bakery_lock(Tnum);
                keypress = event;
                newEvent = 1;
                bakery_unlock(Tnum);
            }
        }
    }
    // spam automove

    void auto_spam()
    {
        if (automover)
        {
            if (autod == 0)
            {
                if (autop == 0)
                {
                    Event keyEvent;
                    keyEvent.type = Event::KeyPressed;
                    keyEvent.key.code = Keyboard::Up;
                    keypress = keyEvent;
                    newEvent = 1;
                    autop = 1;
                }
                else
                {
                    Event keyEvent;
                    keyEvent.type = Event::KeyPressed;
                    keyEvent.key.code = Keyboard::Left;
                    keypress = keyEvent;
                    newEvent = 1;
                    autop = 0;
                }
            }
            else if (autod == 1)
            {
                if (autop == 0)
                {
                    Event keyEvent;
                    keyEvent.type = Event::KeyPressed;
                    keyEvent.key.code = Keyboard::Up;
                    keypress = keyEvent;
                    newEvent = 1;
                    autop = 1;
                }
                else
                {
                    Event keyEvent;
                    keyEvent.type = Event::KeyPressed;
                    keyEvent.key.code = Keyboard::Right;
                    keypress = keyEvent;
                    newEvent = 1;
                    autop = 0;
                }
            }
            else if (autod == 2)
            {
                if (autop == 0)
                {
                    Event keyEvent;
                    keyEvent.type = Event::KeyPressed;
                    keyEvent.key.code = Keyboard::Down;
                    keypress = keyEvent;
                    newEvent = 1;
                    autop = 1;
                }
                else
                {
                    Event keyEvent;
                    keyEvent.type = Event::KeyPressed;
                    keyEvent.key.code = Keyboard::Left;
                    keypress = keyEvent;
                    newEvent = 1;
                    autop = 0;
                }
            }
            else if (autod == 3)
            {
                if (autop == 0)
                {
                    Event keyEvent;
                    keyEvent.type = Event::KeyPressed;
                    keyEvent.key.code = Keyboard::Down;
                    keypress = keyEvent;
                    newEvent = 1;
                    autop = 1;
                }
                else
                {
                    Event keyEvent;
                    keyEvent.type = Event::KeyPressed;
                    keyEvent.key.code = Keyboard::Right;
                    keypress = keyEvent;
                    newEvent = 1;
                    autop = 0;
                }
            }
            move(keypress);
        }
    }

    void wallmerge()
    {
        for (int i = 0; i < ghosts.Gsize; i++)
        {
            int found1 = -1;
            FloatRect wall;
            FloatRect bounds1 = ghosts.start[i].ghost_s.getGlobalBounds();
            for (int j = 0; j < maze.n_horizontal; j++)
            {
                FloatRect bounds2 = maze.wall_s_horizontal[j].getGlobalBounds();
                if (bounds1.intersects(bounds2))
                {
                    wall = bounds2;
                    found1 = 1;
                    break;
                }
            }
            for (int j = 0; j < maze.n_leftbottom; j++)
            {
                FloatRect bounds2 = maze.wall_s_leftbottom[j].getGlobalBounds();
                if (bounds1.intersects(bounds2))
                {
                    wall = bounds2;
                    found1 = 1;
                    break;
                }
            }
            for (int j = 0; j < maze.n_lefttop; j++)
            {
                FloatRect bounds2 = maze.wall_s_lefttop[j].getGlobalBounds();
                if (bounds1.intersects(bounds2))
                {
                    wall = bounds2;
                    found1 = 1;
                    break;
                }
            }
            for (int j = 0; j < maze.n_rightbottom; j++)
            {
                FloatRect bounds2 = maze.wall_s_rightbottom[j].getGlobalBounds();
                if (bounds1.intersects(bounds2))
                {
                    wall = bounds2;
                    found1 = 1;
                    break;
                }
            }
            for (int j = 0; j < maze.n_righttop; j++)
            {
                FloatRect bounds2 = maze.wall_s_righttop[j].getGlobalBounds();
                if (bounds1.intersects(bounds2))
                {
                    wall = bounds2;
                    found1 = 1;
                    break;
                }
            }
            for (int j = 0; j < maze.n_vertical; j++)
            {
                FloatRect bounds2 = maze.wall_s_vertical[j].getGlobalBounds();
                if (bounds1.intersects(bounds2))
                {
                    wall = bounds2;
                    found1 = 1;
                    break;
                }
            }

            if (found1 != -1)
            {
                int mov = -1;

                if (bounds1.top < wall.top)
                {
                    ghosts.start[i].ghost_s.setPosition(ghosts.start[i].ghost_s.getPosition().x, ghosts.start[i].ghost_s.getPosition().y - 10);
                }
                else if ((bounds1.left + bounds1.width) > (wall.left + wall.width))
                {
                    ghosts.start[i].ghost_s.setPosition(ghosts.start[i].ghost_s.getPosition().x + 10, ghosts.start[i].ghost_s.getPosition().y);
                }
                else if ((bounds1.top + bounds1.height) > (wall.top + wall.height))
                {
                    ghosts.start[i].ghost_s.setPosition(ghosts.start[i].ghost_s.getPosition().x, ghosts.start[i].ghost_s.getPosition().y + 10);
                }
                else
                {
                    ghosts.start[i].ghost_s.setPosition(ghosts.start[i].ghost_s.getPosition().x - 10, ghosts.start[i].ghost_s.getPosition().y);
                }
            }
        }
    }

    void wallmerge2()
    {

        int found1 = -1;
        FloatRect wall;
        FloatRect bounds1 = player.pacman_S.getGlobalBounds();
        for (int j = 0; j < maze.n_horizontal; j++)
        {
            FloatRect bounds2 = maze.wall_s_horizontal[j].getGlobalBounds();
            if (bounds1.intersects(bounds2))
            {
                wall = bounds2;
                found1 = 1;
                break;
            }
        }
        for (int j = 0; j < maze.n_leftbottom; j++)
        {
            FloatRect bounds2 = maze.wall_s_leftbottom[j].getGlobalBounds();
            if (bounds1.intersects(bounds2))
            {
                wall = bounds2;
                found1 = 1;
                break;
            }
        }
        for (int j = 0; j < maze.n_lefttop; j++)
        {
            FloatRect bounds2 = maze.wall_s_lefttop[j].getGlobalBounds();
            if (bounds1.intersects(bounds2))
            {
                wall = bounds2;
                found1 = 1;
                break;
            }
        }
        for (int j = 0; j < maze.n_rightbottom; j++)
        {
            FloatRect bounds2 = maze.wall_s_rightbottom[j].getGlobalBounds();
            if (bounds1.intersects(bounds2))
            {
                wall = bounds2;
                found1 = 1;
                break;
            }
        }
        for (int j = 0; j < maze.n_righttop; j++)
        {
            FloatRect bounds2 = maze.wall_s_righttop[j].getGlobalBounds();
            if (bounds1.intersects(bounds2))
            {
                wall = bounds2;
                found1 = 1;
                break;
            }
        }
        for (int j = 0; j < maze.n_vertical; j++)
        {
            FloatRect bounds2 = maze.wall_s_vertical[j].getGlobalBounds();
            if (bounds1.intersects(bounds2))
            {
                wall = bounds2;
                found1 = 1;
                break;
            }
        }

        if (found1 != -1)
        {
            int mov = -1;

            if (bounds1.top < wall.top)
            {
                player.pacman_S.setPosition(player.pacman_S.getPosition().x, player.pacman_S.getPosition().y - 10);
            }
            else if ((bounds1.left + bounds1.width) > (wall.left + wall.width))
            {
                player.pacman_S.setPosition(player.pacman_S.getPosition().x + 10, player.pacman_S.getPosition().y);
            }
            else if ((bounds1.top + bounds1.height) > (wall.top + wall.height))
            {
                player.pacman_S.setPosition(player.pacman_S.getPosition().x, player.pacman_S.getPosition().y + 10);
            }
            else
            {
                player.pacman_S.setPosition(player.pacman_S.getPosition().x - 10, player.pacman_S.getPosition().y);
            }
        }
    }

    // engine
    void engine_Game(int Tnum)
    {
        while (window.isOpen())
        {
            if (activestate)
            {
                // keypress check
                bakery_lock(Tnum);

                // oldsprite

                // if(oldT.getElapsedTime().asSeconds()>=0.1 && automover){
                //     if(oldsprite.getPosition()==player.pacman_S.getPosition()){
                //         findnext=1;
                //         cout<<"find next\n";
                //     }
                //     oldT.restart();
                // }
                // if(StuckT.getElapsedTime().asSeconds()>=0.1){
                //     oldsprite.setPosition(player.pacman_S.getPosition());
                //     StuckT.restart();
                // }

                if (newEvent == 1)
                {
                    newEvent = 0;
                    if (keypress.type == Event::Closed)
                        window.close();
                    else if (keypress.type == Event::KeyPressed)
                    {
                        if (keypress.key.code == sf::Keyboard::A)
                        {
                            if (!automover)
                            {
                                automover = 1;
                            }
                            else
                            {
                                automover = 0;
                            }
                        }
                        else
                        {

                            move(keypress);

                            player.face_movement(1);
                        }
                    }
                }
                // spam
                auto_spam();

                // when inside cage
                // cout<<"outside: "<<ghosts.outside<<endl;

                wallmerge();
                wallmerge2();
                if (ghosts.outside < GG)
                {
                    for (int i = 0; i < GG; i++)
                    {
                        if (ghosts.start[i].home == 1)
                        {
                            // ghosts.selfmove1(i);
                            ghosts.move1(i);
                        }
                    }
                }
                if (ghosts.outside > 0)
                {
                    ghostlaunched = true;
                }
                if (ghosts.outside >= GG)
                {
                    for (int i = 0; i < GG; i++)
                    {
                        moveghostsout1(i);
                    }
                }
                // boost
                if (boostT.getElapsedTime().asSeconds() >= 10)
                {
                    ghosts.removeactive();
                    boostT.restart();
                }

                // pacman mouth move
                if (mouth_open.getElapsedTime().asSeconds() >= 0.4)
                {
                    player.face_movement(0);
                    mouth_open.restart();
                }
                // pacman automove
                if (automoving.getElapsedTime().asSeconds() >= 0.03)
                {
                    automove();
                    automoving.restart();
                }
                // eat plate

                // something related to teleporting idk
                if (teleporting.getElapsedTime().asSeconds() >= 0.3f && teleported)
                {
                    teleported = false;
                }
                // self move on ghosts idk what this does either
                if (ghostselfmove.getElapsedTime().asSeconds() >= 0.06)
                {
                    // ghosts.allselfchange();
                    ghostselfmove.restart();
                }
                // key released
                int value;
                sem_getvalue(&ghost_key, &value);
                if (ghostkeyT.getElapsedTime().asSeconds() >= 1.5 && value == 0)
                {
                    sem_post(&ghost_key);
                    ghostkeyT.restart();
                }
                // ghost movement
                if (movement.getElapsedTime().asSeconds() > 0.0405f)
                {
                    movement.restart();
                }
                if (movement.getElapsedTime().asSeconds() > 0.010f && !teleported && ghostlaunched)
                {
                    if (previousmove != player.dir && lifeT.getElapsedTime().asSeconds() >= 5.0f)
                    {
                        movementdetector();
                    }
                    ghosts.updatehunt();
                }
                // lives
                if (lives <= 0)
                {
                    highscore.insert(score, name);
                    window.close();
                    return;
                }

                bakery_unlock(Tnum);
            }
        }
    }
    // food consumption
    void food_Game(int Tnum)
    {
        while (window.isOpen())
        {
            if (activestate)
            {

                bakery_lock(Tnum);
                checkfood();
                bakery_unlock(Tnum);
            }
        }
    }
    // ghosts
    void ghost_Game(int Tnum)
    {
        moveghostsout(Tnum);
        while (window.isOpen())
        {
            if (activestate)
            {

                Gbakery_lock(Tnum);
                ghosts.selfmove(Tnum);
                ghosts.move(Tnum);
                hunt(Tnum);
                pacman_ghost_collision(Tnum);
                Gbakery_unlock(Tnum);
            }
        }
    }

    void auto_Game(int Tnum)
    {
        while (window.isOpen())
        {
            if (automover && (findnext || Tautomove.getElapsedTime().asSeconds() >= 0.55))
            {
                bakery_lock(Tnum);
                float mindis = 10000;
                int dd = 0;
                int px = player.pacman_S.getPosition().x;
                int py = player.pacman_S.getPosition().y;
                for (int i = 0; i < food.n_food; i++)
                {
                    if (food.valid[i] == 1)
                    {
                        int fx = food.food_S[i].getPosition().x;
                        int fy = food.food_S[i].getPosition().y;
                        // distance formula
                        float d = pow((pow(px - fx, 2) + pow(py - fy, 2)), 0.5);
                        if (d < mindis)
                        {
                            mindis = d;
                            dd = 0;
                            if (px < fx)
                            {
                                dd++;
                            }
                            if (py < fy)
                            {
                                dd += 2;
                            }
                        }
                    }
                }
                autod = dd;
                Tautomove.restart();
                findnext = 0;
                bakery_unlock(Tnum);
            }
        }
    }

    // boost
    void boost_Game(int Tnum)
    {
        while (window.isOpen())
        {
            sem_wait(&booster_key);
            ghosts.start[Tnum].makeactive();
            while (ghosts.start[Tnum].Bactive)
            {
                sleep(1);
            }
            sem_post(&booster_key);
            ghosts.start[Tnum].makeinactive();
            usleep(10000);
        }
    }
    // pallet
    void pallet_Game(int Tnum)
    {

        while (window.isOpen())
        {

            if (activestate)
            {
                bakery_lock(Tnum);
                palletcollection();
                // pallets
                if (pallettimer.getElapsedTime().asSeconds() >= 15.0f && !pallet.Visible)
                {
                    deploypallet();
                }
                // gaari ke blinker
                if (eatghosts.getElapsedTime().asSeconds() > 9.0f && eatghosts.getElapsedTime().asSeconds() < 9.3f)
                {
                    ghosts.unblink();
                    activepallet = false;
                }
                // pata nahi kia
                if (critical_state_ghost.getElapsedTime().asSeconds() >= 0.5f)
                {
                    critical_state_ghost.restart();
                    reconsider_critical_state_ghost();
                }

                bakery_unlock(Tnum);
            }
        }
    }
};

Game G;
void *engine_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.engine_Game(Tnum);
    pthread_exit(NULL);
}

void *interface_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.interface_Game(Tnum);
    pthread_exit(NULL);
}

void *input_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.input_Game(Tnum);
    pthread_exit(NULL);
}

void *food_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.food_Game(Tnum);
    pthread_exit(NULL);
}

void *pallet_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.pallet_Game(Tnum);
    pthread_exit(NULL);
}

void *ghost_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.ghost_Game(Tnum);
    pthread_exit(NULL);
}

void *boost_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.boost_Game(Tnum);
    pthread_exit(NULL);
}

void *auto_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.auto_Game(Tnum);
    pthread_exit(NULL);
}

void basefunction()
{

    G.window.create(sf::VideoMode(1200, 800), "PACMAN");

    for (int i = 0; i < Tnumb; i++)
    {
        pthread_mutex_init(&busy[i], NULL);
        values[i] = 0;
    }

    for (int i = 0; i < GG; i++)
    {
        pthread_mutex_init(&busyG[i], NULL);
        valuesG[i] = 0;
    }

    sem_init(&ghost_key, 0, 1);
    sem_init(&booster_key, 0, 2);

    srand(time(0));
    // interface
    //  pthread_create(&threads[0],NULL,interface_t,(void*)(new int(0)));
    pthread_create(&threads[1], NULL, input_t, (void *)(new int(1)));
    pthread_create(&threads[2], NULL, engine_t, (void *)(new int(2)));
    pthread_create(&threads[3], NULL, food_t, (void *)(new int(3)));
    pthread_create(&threads[4], NULL, pallet_t, (void *)(new int(4)));
    pthread_create(&threads[5], NULL, auto_t, (void *)(new int(5)));
    // cout << "HERE" << endl;

    for (int i = 0; i < GG; i++)
    {
        pthread_create(&threadsG[i], NULL, ghost_t, (void *)(new int(i)));
        pthread_create(&threadsBoost[i], NULL, boost_t, (void *)(new int(i)));
    }
    G.interface_Game(0);
    // main thread
    //  while (window2.isOpen()){}
}

void displayHighScores(RenderWindow &window)
{
    // Create window

    // Create background
    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(sf::Color::White);

    // Load font
    sf::Font font;
    if (!font.loadFromFile("font/BebasNeue-Regular.ttf"))
    {
        // Error handling
        return;
    }

    // Load high scores
    ScoreList highscore;
    highscore.readFromFile();

    // Text setup

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::Black);

    // Button setup
    sf::RectangleShape returnButton(sf::Vector2f(200.f, 50.f));
    returnButton.setFillColor(sf::Color::Blue);
    returnButton.setPosition(window.getSize().x / 2 - returnButton.getGlobalBounds().width / 2, 600);

    sf::Text returnButtonText("Return", font, 20);
    returnButtonText.setFillColor(sf::Color::White);
    returnButtonText.setPosition(window.getSize().x / 2 - returnButtonText.getGlobalBounds().width / 2, 610.f);
    // Display window until closed
    sf::Text title("Highscores", font, 50);
    title.setFillColor(sf::Color::Black);
    title.setStyle(sf::Text::Bold);
    title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 50);

    Clock rotate, size;
    int val = 0;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    // Check if the return button is pressed
                    if (returnButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        return;
                    }
                }
            }
            if (event.type == sf::Event::MouseMoved)
            {

                if (returnButton.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y))
                {
                    val = 1;

                    returnButton.setFillColor(sf::Color::Red);
                }
                else if (!returnButton.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y))
                {
                    val = 0;

                    returnButton.setFillColor(sf::Color::Blue);
                }
            }
        }
        if (size.getElapsedTime().asSeconds() >= 0.1f)
        {
            if (returnButton.getScale().x <= 2.0f && val == 1)
            {

                returnButton.setScale(returnButton.getScale().x + 0.1f, 1.0f);
                returnButton.setPosition(returnButton.getPosition().x - 9, returnButton.getPosition().y);
            }

            if (returnButton.getScale().x >= 1.1f && val == 0)
            {

                returnButton.setScale(returnButton.getScale().x - 0.1f, 1.0f);
                returnButton.setPosition(returnButton.getPosition().x + 9, returnButton.getPosition().y);
            }
            size.restart();
        }
        // if (rotate.getElapsedTime().asSeconds() >= 0.1)
        // {
        //     y -= 1;
        //     rotate.restart();
        // }
        float x = 500.0f;
        float y = 150.0f; // Adjusted starting y position
        int count = 0;
        window.clear();
        window.draw(background);

        if (y <= -100)
        {
            y = 180.0f;
        }
        // Positioning variables
        score_body *temp = highscore.head;
        while (temp != nullptr && count < 8)
        {
            std::string scoreText = std::to_string(count + 1) + ".  " + temp->name + " (" + std::to_string(temp->score) + ")";
            text.setString(scoreText);
            text.setPosition(window.getSize().x / 2 - text.getGlobalBounds().width / 2, y);
            window.draw(text);
            y += 50.f;
            temp = temp->next;
            count++;
        }

        // Draw return button
        window.draw(title);
        window.draw(returnButton);
        window.draw(returnButtonText);
        window.display();
    }
}

void displayPacmanInstructions(sf::RenderWindow &window)
{
    // Create background
    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(sf::Color::White);

    // Load font
    sf::Font font;
    if (!font.loadFromFile("font/BebasNeue-Regular.ttf"))
    {
        // Error handling
        return;
    }

    // Text setup
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::Black);

    // Display window until closed
    sf::Text title("Pacman Instructions", font, 50);
    title.setFillColor(sf::Color::Black);
    title.setStyle(sf::Text::Bold);
    title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 50);

    sf::Text instructionText(

        "1. Use the arrow keys (up, down, left, right) to move Pacman.\n"
        "2. Eat all the dots on the maze while avoiding the ghosts.\n"
        "3. Power pellets will appear in the corners of the maze. Eating a power pellet allows Pacman to eat ghosts for a limited time.\n"
        "4. Score points by:\n"
        "   - Eating dots: Each dot eaten earns you 100 points.\n"
        "   - Eating ghosts: Each ghost eaten while under the effect of a power pellet earns you 150 points.\n"
        "5. There are four ghosts in the maze, each with unique behavior:\n"
        "   - Blinky (Red): Chases Pacman aggressively.\n"
        "6. Pacman loses a life when it collides with a ghost. The game ends when all lives are lost.\n"
        "7. Eating fruits that occasionally appear in the maze provides bonus points.\n"
        "8. Complete each maze by eating all the dots to progress to the next level, where the difficulty increases.\n"
        "9. Enjoy the classic arcade gameplay of Pacman and aim for the highest score!\n",
        font,
        25);
    instructionText.setFillColor(sf::Color::Black);
    instructionText.setPosition(window.getSize().x / 2 - instructionText.getGlobalBounds().width / 2, 180);

    // Button setup
    sf::RectangleShape returnButton(sf::Vector2f(200.f, 50.f));
    returnButton.setFillColor(sf::Color::Blue);
    returnButton.setPosition(window.getSize().x / 2 - returnButton.getGlobalBounds().width / 2, 600);

    sf::Text returnButtonText("Return", font, 20);
    returnButtonText.setFillColor(sf::Color::White);
    returnButtonText.setPosition(window.getSize().x / 2 - returnButtonText.getGlobalBounds().width / 2, 610.f);

    // Display window until closed
    int val = 0;
    Clock size;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    // Check if the return button is pressed
                    if (returnButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        return;
                    }
                }
            }
            if (event.type == sf::Event::MouseMoved)
            {

                if (returnButton.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y))
                {
                    val = 1;

                    returnButton.setFillColor(sf::Color::Red);
                }
                else if (!returnButton.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y))
                {
                    val = 0;

                    returnButton.setFillColor(sf::Color::Blue);
                }
            }
        }
        if (size.getElapsedTime().asSeconds() >= 0.1f)
        {
            if (returnButton.getScale().x <= 2.0f && val == 1)
            {

                returnButton.setScale(returnButton.getScale().x + 0.1f, 1.0f);
                returnButton.setPosition(returnButton.getPosition().x - 9, returnButton.getPosition().y);
            }

            if (returnButton.getScale().x >= 1.1f && val == 0)
            {

                returnButton.setScale(returnButton.getScale().x - 0.1f, 1.0f);
                returnButton.setPosition(returnButton.getPosition().x + 9, returnButton.getPosition().y);
            }
            size.restart();
        }
        // Draw content
        window.clear();
        window.draw(background);
        window.draw(title);
        window.draw(instructionText);
        window.draw(returnButton);
        window.draw(returnButtonText);
        window.display();
    }
}

std::string getUserInput(sf::RenderWindow &window)
{
    std::string input;

    sf::Font font;
    font.loadFromFile("font/BebasNeue-Regular.ttf");
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);

    sf::Text title("Input Name", font, 50);
    title.setFillColor(sf::Color::Black);
    title.setStyle(sf::Text::Bold);
    title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 50);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode < 128)
                {
                    if (event.text.unicode == 8 && !input.empty())
                    { // Handle backspace
                        input.pop_back();
                    }
                    else if (event.text.unicode == 13)
                    { // Handle enter
                    return input;
                    }
                    else
                    {
                        input += static_cast<char>(event.text.unicode);
                    }
                    text.setString(input);
                }
            }
        }

        window.clear(sf::Color::White);
        text.setPosition(window.getSize().x / 2 - text.getGlobalBounds().width / 2, 300.f);

        window.draw(title);
        window.draw(text);
        window.display();
    }

    return ""; // Return empty string if window is closed
}

void startmenu()
{
    sf::RenderWindow window(sf::VideoMode(1200, 700), "PACMAN");

    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(sf::Color::White);

    sf::Font font;
    if (!font.loadFromFile("font/BebasNeue-Regular.ttf"))
    {
        // Error handling
        return;
    }
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("sounds/menu.ogg"))
    {

        return;
    }

    // Create the sound object and set its buffer
    sf::Sound sound;
    sound.setBuffer(buffer);
    int ring = 0;
    // Title
    sf::Text title("Main Menu", font, 50);
    title.setFillColor(sf::Color::Black);
    title.setStyle(sf::Text::Bold);
    title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 50);

    // Options
    sf::Text startOption("Start Game", font, 30);
    startOption.setFillColor(sf::Color::Black);
    startOption.setPosition(window.getSize().x / 2 - startOption.getGlobalBounds().width / 2, 200);

    sf::Text highscoreOption("Highscore", font, 30);
    highscoreOption.setFillColor(sf::Color::Black);
    highscoreOption.setPosition(window.getSize().x / 2 - highscoreOption.getGlobalBounds().width / 2, 280);

    sf::Text instructionsOption("Instructions", font, 30);
    instructionsOption.setFillColor(sf::Color::Black);
    instructionsOption.setPosition(window.getSize().x / 2 - instructionsOption.getGlobalBounds().width / 2, 360);

    sf::Text exitOption("Exit", font, 30);
    exitOption.setFillColor(sf::Color::Black);
    exitOption.setPosition(window.getSize().x / 2 - exitOption.getGlobalBounds().width / 2, 440);

    Texture start, instruction, highscore, exit;
    start.loadFromFile("images/start.png");
    instruction.loadFromFile("images/instruction.png");
    highscore.loadFromFile("images/Highscore.png");
    exit.loadFromFile("images/exit.png");
    Sprite startS, instructionS, highscoreS, exitS;
    startS.setTexture(start);
    startS.setScale(0.0f, 0.3f);
    startS.setPosition(450, 240);

    instructionS.setTexture(instruction);
    instructionS.setScale(0.0f, 0.3f);
    instructionS.setPosition(450, 320);

    highscoreS.setTexture(highscore);
    highscoreS.setScale(0.0f, 0.3f);
    highscoreS.setPosition(450, 400);

    exitS.setTexture(exit);
    exitS.setScale(0.0f, 0.3f);
    exitS.setPosition(450, 480);
    Clock grow1, grow2, grow3, grow4;

    int value1 = 0, value2 = 0, value3 = 0, value4 = 0;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {

                    window.close();
                }
            }
            if (event.type == sf::Event::MouseMoved)
            {

                if (startOption.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y) && value1 != 1)
                {

                    ring = 1;
                    startOption.setFillColor(sf::Color::Red);
                    value1 = 1;
                }
                else if (!startOption.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y))
                {
                    value1 = 0;
                    startOption.setFillColor(sf::Color::Black);
                }

                if (highscoreOption.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y) && value1 != 2)
                {

                    ring = 1;
                    highscoreOption.setFillColor(sf::Color::Red);
                    value2 = 1;
                }
                else if (!highscoreOption.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y))
                {
                    highscoreOption.setFillColor(sf::Color::Black);
                    value2 = 0;
                }

                if (instructionsOption.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y) && value1 != 3)
                {

                    ring = 1;
                    instructionsOption.setFillColor(sf::Color::Red);
                    value3 = 1;
                }
                else if (!instructionsOption.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y))
                {
                    value3 = 0;
                    instructionsOption.setFillColor(sf::Color::Black);
                }

                if (exitOption.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y) && value1 != 4)
                {

                    exitOption.setFillColor(sf::Color::Red);
                    value4 = 1;
                    ring = 1;
                }
                else if (!exitOption.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y))
                {
                    value4 = 0;
                    exitOption.setFillColor(sf::Color::Black);
                }
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    // Check if the mouse click is within the bounds of the startOption
                    if (startOption.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        G.name=getUserInput(window);
                        window.close();
                        basefunction();
                    }
                    else if (highscoreOption.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        window.clear();

                        displayHighScores(window);
                        std::cout << "Highscore Option Selected" << std::endl;
                    }
                    else if (instructionsOption.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        displayPacmanInstructions(window);
                        std::cout << "Instructions Option Selected" << std::endl;
                    }
                    else if (exitOption.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        std::cout << "Exit Option Selected" << std::endl;
                        return;
                    }
                }
            }
        }

        if (value1 == 1)
        {
            if (grow1.getElapsedTime().asSeconds() > 0.1f)
            {
                if (startS.getScale().x < 0.5f)
                {
                    startS.setScale(startS.getScale().x + 0.1f, 0.3f);
                    grow1.restart();
                }
            }
        }
        else if (value1 == 0)
        {
            if (grow1.getElapsedTime().asSeconds() > 0.1f)
            {
                if (startS.getScale().x >= 0.1f)
                {
                    startS.setScale(startS.getScale().x - 0.1f, 0.3f);
                    grow1.restart();
                }
            }
        }
        if (value2 == 1)
        {
            if (grow2.getElapsedTime().asSeconds() > 0.1f)
            {
                if (instructionS.getScale().x < 0.5f)
                {
                    instructionS.setScale(instructionS.getScale().x + 0.1f, 0.3f);
                    grow2.restart();
                }
            }
        }
        else if (value2 == 0)
        {
            if (grow2.getElapsedTime().asSeconds() > 0.1f)
            {
                if (instructionS.getScale().x >= 0.1f)
                {
                    instructionS.setScale(instructionS.getScale().x - 0.1f, 0.3f);
                    grow2.restart();
                }
            }
        }
        if (value3 == 1)
        {
            if (grow3.getElapsedTime().asSeconds() > 0.1f)
            {
                if (highscoreS.getScale().x < 0.5f)
                {
                    highscoreS.setScale(highscoreS.getScale().x + 0.1f, 0.3f);
                    grow3.restart();
                }
            }
        }
        else if (value3 == 0)
        {
            if (grow3.getElapsedTime().asSeconds() > 0.1f)
            {
                if (highscoreS.getScale().x >= 0.1f)
                {
                    highscoreS.setScale(highscoreS.getScale().x - 0.1f, 0.3f);
                    grow3.restart();
                }
            }
        }
        if (value4 == 1)
        {
            if (grow4.getElapsedTime().asSeconds() > 0.1f)
            {
                if (exitS.getScale().x < 0.5f)
                {
                    exitS.setScale(exitS.getScale().x + 0.1f, 0.3f);
                    grow4.restart();
                }
            }
        }
        else if (value4 == 0)
        {
            if (grow4.getElapsedTime().asSeconds() > 0.1f)
            {
                if (exitS.getScale().x >= 0.1f)
                {
                    exitS.setScale(exitS.getScale().x - 0.1f, 0.3f);
                    grow4.restart();
                }
            }
        }

        if (ring == 1)
        {
            sound.play();
            ring = 0;
        }
        window.clear();

        window.draw(background);
        window.draw(title);
        window.draw(startOption);
        window.draw(highscoreOption);
        window.draw(instructionsOption);
        window.draw(exitOption);
        window.draw(startS);
        window.draw(instructionS);
        window.draw(highscoreS);
        window.draw(exitS);
        window.display();
    }
}

int main()
{
    // basefunction();

    startmenu();
}