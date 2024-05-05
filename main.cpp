#include <SFML/Graphics.hpp>
#include "pacman.h"
#include "maze.h"
#include <time.h>
using namespace std;
class Game
{
private:
    RenderWindow window;
    Clock mouth_open, automoving;
    Pacman player;
    Maze maze;
    int previousmove = 0;
    Text x, y;
    Font font;

public:
    Game() : window(sf::VideoMode(1920, 1000), "SFML works!")
    {
        font.loadFromFile("font/BebasNeue-Regular.ttf");
    }
    bool checkCollision(const Sprite &pacman, const Sprite *walls, int numWalls)
    {

        FloatRect pacmanBounds = pacman.getGlobalBounds();

        for (int i = 0; i < numWalls; i++)
        {

            FloatRect wallBounds = walls[i].getGlobalBounds();

            if (pacmanBounds.intersects(wallBounds))
            {
                x.setString(to_string(walls[i].getPosition().x));
                y.setString(to_string(walls[i].getPosition().y));

                return true;
            }
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


            x.setFillColor(Color::White);
            y.setFillColor(Color::White);
            x.setFont(font);
            y.setFont(font);
            x.setPosition(1600, 100);
            y.setPosition(1750, 100);
            x.setCharacterSize(30);
            y.setCharacterSize(30);
            window.draw(x);
            window.draw(y);
            maze.display(window);
            window.display();
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
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical))
            {
                maze.move(0, moveAmount);
            }

            player.dir = 2;
            previousmove = 2;
        }
        else if (event.key.code == sf::Keyboard::Down)
        {
            player.pacman_S.setPosition(PosX, PosY + 10);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical))
            {
                maze.move(0, -moveAmount);
            }
            player.dir = 4;
            previousmove = 4;
        }
        else if (event.key.code == sf::Keyboard::Left)
        {
            player.pacman_S.setPosition(PosX - 10, PosY);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical))
            {
                maze.move(moveAmount, 0);
            }
            player.dir = 3;
            previousmove = 3;
        }
        else if (event.key.code == sf::Keyboard::Right)
        {
            player.pacman_S.setPosition(PosX + 10, PosY);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical))
            {
                maze.move(-moveAmount, 0);
            }
            player.dir = 1;
            previousmove = 1;
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
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical))
            {
                maze.move(0, moveAmount);
            }
            previousmove = 2;
            player.dir = 2;
        }
        else if (previousmove == 4)
        {
            player.pacman_S.setPosition(PosX, PosY + 10);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical))
            {
                maze.move(0, -moveAmount);
            }
            previousmove = 4;
            player.dir = 4;
        }
        else if (previousmove == 3)
        {
            player.pacman_S.setPosition(PosX - 10, PosY);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical))
            {
                maze.move(moveAmount, 0);
            }
            previousmove = 3;
            player.dir = 3;
        }
        else if (previousmove = 1)
        {
            player.pacman_S.setPosition(PosX + 10, PosY);
            if (!checkCollision(player.pacman_S, maze.wall_s_horizontal, maze.n_horizontal) &&
                !checkCollision(player.pacman_S, maze.wall_s_vertical, maze.n_vertical))
            {
                maze.move(-moveAmount, 0);
            }
            previousmove = 1;
            player.dir = 1;
        }
        player.pacman_S.setPosition(PosX, PosY);
    }
};

int main()
{
    Game game;
    game.run();
    return 0;
}