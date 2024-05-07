#include <SFML/Graphics.hpp>
#include "pacman.h"
#include "maze.h"
#include "Food.h"
#include "highscore.h"
#include "Ghost.h"
#include <time.h>
using namespace std;
class Game
{
private:
    RenderWindow window;
    Clock mouth_open, automoving,ghostselfmove;
    Pacman player;
    Maze maze;
    Food food;
    Ghostlist ghosts;
    ScoreList highscore;
    Font font;
    int previousmove = 0;
    int score = 0;
    string name = "Guest";
    int validitycount=0;
    int level=1;

public:
    Game() : window(sf::VideoMode(1220, 800), "SFML works!")
    {
        font.loadFromFile("font/BebasNeue-Regular.ttf");
        highscore.readFromFile();
        for (int i = 0; i < 9; i++)
        {
            ghosts.create(i);
        }
        
    }

    int checkCollisionfood(const Sprite &pacman, const Sprite *walls, int numWalls)
    {

        FloatRect pacmanBounds = pacman.getGlobalBounds();

        for (int i = 0; i < numWalls; i++)
        {

            FloatRect wallBounds = walls[i].getGlobalBounds();

            if (pacmanBounds.intersects(wallBounds) && food.valid[i]!=0)
            {
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

            if (mouth_open.getElapsedTime().asSeconds() >= 0.4)
            {
                player.face_movement(0);
                mouth_open.restart();
            }

            window.clear();
            window.draw(player.pacman_S);
            if (automoving.getElapsedTime().asSeconds() >= 0.01)
            {
                automove();
                automoving.restart();
            }
            if(ghostselfmove.getElapsedTime().asSeconds()>=0.1){
                ghosts.selfmove();
                ghostselfmove.restart();
            }
            food.display(window);
            ghosts.display(window);
            checkfood();
            maze.display(window,level,validitycount,food.n_food,name);
            displayTopThreeScores();
            window.display();
        }
    }

    void checkfood()
    {
        int index = checkCollisionfood(player.pacman_S, food.food_S, food.n_food);
        if (index != -1)
        {
            food.valid[index] = 0;
            score += 50;
            validitycount++;
            if(validitycount==food.n_food-1){
                for (int i = 0; i < food.n_food; i++)
                {
                    food.valid[i]=1;
                }
                validitycount=0;
                level++;
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
                player.dir = 2;

                previousmove = 2;
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
                player.dir = 4;
                previousmove = 4;
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
                player.dir = 3;
                previousmove = 3;
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
                player.dir = 1;
                previousmove = 1;
            }
        }
        else if (event.key.code == sf::Keyboard::Escape)
        {
            exit(0);
        }
        player.pacman_S.setPosition(PosX, PosY);
    }

    void automove()
    {
        int PosY = player.pacman_S.getPosition().y;
        int PosX = player.pacman_S.getPosition().x;
        int moveAmount = 5;

        if (previousmove == 2)
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
                player.dir = 2;
                previousmove = 2;
            }
        }
        else if (previousmove == 4)
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
                player.dir = 4;
                previousmove = 4;
            }
        }
        else if (previousmove == 3)
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
                player.dir = 3;
                previousmove = 3;
            }
        }
        else if (previousmove = 1)
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
                previousmove = 1;
                player.dir = 1;
            }
        }

        if (teleport(player.pacman_S, maze.Teleportationx))
        {
            if (rand() % 2 == 0)
            {
                maze.move(-580, 450);
                food.move(-580, 450);
                ghosts.move(-580, 450);
                player.dir = 3;
            }
            else
            {
                maze.move(+580, 450);
                food.move(+580, 450);
                ghosts.move(+580, 450);
                player.dir = 1;
            }
        }
        else if (teleport(player.pacman_S, maze.Teleportationy1))
        {
            maze.move(-1270, 0);
            food.move(-1270, 0);
            ghosts.move(-1270, 0);
            player.dir = 1;
        }
        else if (teleport(player.pacman_S, maze.Teleportationy2))
        {
            maze.move(1270, 0);
            food.move(1270, 0);
            ghosts.move(1270, 0);
            player.dir = 3;
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
};

int main()
{
    Game game;
    game.run();
    return 0;
}