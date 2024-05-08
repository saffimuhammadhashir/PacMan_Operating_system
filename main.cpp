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
class Game
{
private:
    RenderWindow window;
    Clock mouth_open, automoving, ghostselfmove, movement, moveout, teleporting, critical_state_ghost, pallettimer, eatghosts;
    Pacman player;
    Maze maze;
    Food food;
    Ghostlist ghosts;
    ScoreList highscore;
    PowerPallet pallet;
    bool activepallet;
    Font font;
    bool firstappearance = false;
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

public:
    Game() : window(sf::VideoMode(1220, 800), "SFML works!")
    {
        font.loadFromFile("font/BebasNeue-Regular.ttf");
        highscore.readFromFile();
        for (int i = 0; i < 5; i++)
        {
            ghosts.create(i);
        }
        pallet.Visible = false;
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

    void run()
    {
        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
                else if (event.type == Event::KeyPressed)
                {
                    move(event);
                    player.face_movement(1);
                }
            }
            window.clear();
            window.draw(player.pacman_S);

            if (eatghosts.getElapsedTime().asSeconds() > 5.0f && eatghosts.getElapsedTime().asSeconds() < 5.3f)
            {
                ghosts.unblink();
                activepallet = false;
            }

            if (mouth_open.getElapsedTime().asSeconds() >= 0.4)
            {
                player.face_movement(0);
                mouth_open.restart();
            }

            if (pallettimer.getElapsedTime().asSeconds() >= 15.0f && !pallet.Visible)
            {
                deploypallet();
            }
            if (automoving.getElapsedTime().asSeconds() >= 0.03)
            {
                automove();
                automoving.restart();
            }
            if (ghostselfmove.getElapsedTime().asSeconds() >= 0.06)
            {
                ghosts.selfmove();
                ghostselfmove.restart();
            }
            if (moveout.getElapsedTime().asSeconds() >= 3.0f)
            {
                moveghostsout();
                ghostlaunched = true;
                moveout.restart();
            }

            if (movement.getElapsedTime().asSeconds() > 0.0105f)
            {

                movement.restart();
            }
            if (movement.getElapsedTime().asSeconds() > 0.010f && !teleported && ghostlaunched)
            {
                if (previousmove != player.dir)
                {
                    movementdetector();
                }
                hunt();
            }
            if (teleporting.getElapsedTime().asSeconds() >= 0.8f && teleported)
            {
                teleported = false;
            }
            if (critical_state_ghost.getElapsedTime().asSeconds() >= 0.5f)
            {
                critical_state_ghost.restart();
                reconsider_critical_state_ghost();
            }
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
            pacman_ghost_collision();
            palletcollection();
            food.display(window);
            ghosts.display(window);
            pallet.display(window);
            checkfood();
            maze.display(window, level, validitycount, food.n_food, name, val, lives);
            displayTopThreeScores();
            window.display();
            if (lives <= 0)
            {
                highscore.insert(score, name);
                return;
            }
        }
    }

    void deploypallet()
    {
        while (!pallet.Visible)
        {

            for (int i = 0; i < food.n_food; i++)
            {
                if (food.valid[i] != 1)
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

    void reconsider_critical_state_ghost()
    {
        if (ghosts.start)
        {
            Ghost *curr = ghosts.start;
            while (curr)
            {

                curr->criticalstate = false;
                curr = curr->next;
            }
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
                ghosts.move(0, moveAmount);
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
                ghosts.move(0, -moveAmount);
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
                ghosts.move(moveAmount, 0);
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
                ghosts.move(-moveAmount, 0);
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

    void moveghostsout()
    {
        if (ghosts.start)
        {
            Ghost *curr = ghosts.start;
            int i = 0;
            while (curr)
            {
                if (curr->home)
                {
                    curr->ghost_s.setPosition(maze.centerx - 30 * i, maze.centery + 5);
                    curr->home = false;
                    i++;
                }
                curr = curr->next;
            }
        }
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

    bool moveHorizontally(Ghost *curr)
    {
        int PosY = curr->ghost_s.getPosition().y;
        int PosX = curr->ghost_s.getPosition().x;
        int orgposY = PosY;
        int orgposX = PosX;

        if (targetx > PosX)
        {
            PosX += 2;
        }
        else if (targetx < PosX)
        {
            PosX -= 2;
        }
        else
        {
            if (targety > PosY)
            {
                PosY += 2;
            }
            else if (targety < PosY)
            {
                PosY -= 2;
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

    void hunt()
    {
        if (ghosts.start)
        {
            Ghost *curr = ghosts.start;
            while (curr)
            {
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
                curr = curr->next;
            }
        }
    }

    void automove()
    {

        int PosY = player.pacman_S.getPosition().y;
        int PosX = player.pacman_S.getPosition().x;
        int moveAmount = 5;

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
                ghosts.move(0, moveAmount);
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
                ghosts.move(0, -moveAmount);
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
                ghosts.move(moveAmount, 0);
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
                ghosts.move(-moveAmount, 0);
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
                ghosts.move(-550, 430);
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
                ghosts.move(+550, 430);
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
            ghosts.move(-1240, 0);
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
            ghosts.move(1240, 0);
            pallet.move(1240, 0);
            previousmove = player.dir;
            player.dir = 1;
            teleported = true;
            teleporting.restart();
        }

        player.pacman_S.setPosition(PosX, PosY);
    }

    void pacman_ghost_collision()
    {
        Ghost *curr = ghosts.start;
        while (curr)
        {
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
                    lives--;
                    return;
                }
            }
            curr = curr->next;
        }
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
};

int main()
{
    Game game;
    game.run();
    return 0;
}