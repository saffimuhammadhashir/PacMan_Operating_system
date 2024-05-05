#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include "pacman.h"
#include <cmath>
using namespace std;
using namespace sf;

class Maze
{
private:
public:
    int **horizontalwalls;
    int **verticalwalls;
    Texture wall_T_horizontal;
    Texture wall_T_vertical;
    Sprite *wall_s_horizontal;
    Sprite *wall_s_vertical;
    int n_horizontal;
    int n_vertical;

    Maze()
    {
        ifstream file_horizontal("data/horizontal.txt");
        ifstream file_vertical("data/vertical.txt");

        file_horizontal >> n_horizontal;
        file_vertical >> n_vertical;
        horizontalwalls = new int *[n_horizontal];
        verticalwalls = new int *[n_vertical];
        for (int i = 0; i < n_horizontal; i++)
        {
            horizontalwalls[i] = new int[2];
        }
        for (int i = 0; i < n_vertical; i++)
        {
            verticalwalls[i] = new int[2];
        }

        for (int i = 0; i < n_horizontal; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                file_horizontal >> horizontalwalls[i][j];
            }
        }

        for (int i = 0; i < n_vertical; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                file_vertical >> verticalwalls[i][j];
            }
        }

        wall_T_horizontal.loadFromFile("images/wall1_horizontal.png");
        wall_T_vertical.loadFromFile("images/wall1_vertical.png");
        wall_s_horizontal = new Sprite[n_horizontal];
        wall_s_vertical = new Sprite[n_vertical];

        for (int i = 0; i < n_horizontal; i++)
        {
            wall_s_horizontal[i].setTexture(wall_T_horizontal);
            wall_s_horizontal[i].setPosition(horizontalwalls[i][0], horizontalwalls[i][1]);
            wall_s_horizontal[i].setScale(0.1, 0.1);
        }
        for (int i = 0; i < n_vertical; i++)
        {
            wall_s_vertical[i].setTexture(wall_T_vertical);
            wall_s_vertical[i].setPosition(verticalwalls[i][0], verticalwalls[i][1]);
            wall_s_vertical[i].setScale(0.1, 0.1);
        }
    }

    void display(RenderWindow &window)
    {

        for (int i = 0; i < n_horizontal; i++)
        {
            window.draw(wall_s_horizontal[i]);
        }
        for (int i = 0; i < n_vertical; i++)
        {
            window.draw(wall_s_vertical[i]);
        }
    }
    void move(int x, int y)
    {
        for (int i = 0; i < n_horizontal; i++)
        {
            horizontalwalls[i][0] += x;
            horizontalwalls[i][1] += y;
            wall_s_horizontal[i].setPosition(horizontalwalls[i][0], horizontalwalls[i][1]);
        }
        for (int i = 0; i < n_vertical; i++)
        {
            verticalwalls[i][0] += x;
            verticalwalls[i][1] += y;
            wall_s_vertical[i].setPosition(verticalwalls[i][0], verticalwalls[i][1]);
        }
    }
};
