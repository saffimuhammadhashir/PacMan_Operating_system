#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;
class Pacman
{

public:
    Texture pacman_T;
    Sprite pacman_S;
    int face = 1;
    int dir = 1; // 1 right     2 top       3 left         4 down
    Pacman()
    {
        pacman_T.loadFromFile("images/1_right.png");
        pacman_S.setTexture(pacman_T);
        pacman_S.setPosition(350, 200);
        pacman_S.setScale(0.06, 0.06);
    }
    void face_movement(int type)
    {
        if (type == 0)
        {
            if (face == 1)
            {
                if (dir == 1)
                {
                    pacman_T.loadFromFile("images/2_right.png");
                }
                else if (dir == 2)
                {
                    pacman_T.loadFromFile("images/2_top.png");
                }
                else if (dir == 3)
                {
                    pacman_T.loadFromFile("images/2_left.png");
                }
                else if (dir == 4)
                {
                    pacman_T.loadFromFile("images/2_down.png");
                }
                face = 2;
            }
            else if (face == 2)
            {
                if (dir == 1)
                {
                    pacman_T.loadFromFile("images/1_right.png");
                }
                else if (dir == 2)
                {
                    pacman_T.loadFromFile("images/1_top.png");
                }
                else if (dir == 3)
                {
                    pacman_T.loadFromFile("images/1_left.png");
                }
                else if (dir == 4)
                {
                    pacman_T.loadFromFile("images/1_down.png");
                }
                face = 1;
            }
        }
        else
        {
            if (face == 2)
            {
                if (dir == 1)
                {
                    pacman_T.loadFromFile("images/2_right.png");
                }
                else if (dir == 2)
                {
                    pacman_T.loadFromFile("images/2_top.png");
                }
                else if (dir == 3)
                {
                    pacman_T.loadFromFile("images/2_left.png");
                }
                else if (dir == 4)
                {
                    pacman_T.loadFromFile("images/2_down.png");
                }
                face = 2;
            }
            else if (face == 1)
            {
                if (dir == 1)
                {
                    pacman_T.loadFromFile("images/1_right.png");
                }
                else if (dir == 2)
                {
                    pacman_T.loadFromFile("images/1_top.png");
                }
                else if (dir == 3)
                {
                    pacman_T.loadFromFile("images/1_left.png");
                }
                else if (dir == 4)
                {
                    pacman_T.loadFromFile("images/1_down.png");
                }
                face = 1;
            }
        }
    
    pacman_S.setTexture(pacman_T);
    pacman_S.setScale(0.06, 0.06);
}
}
;
