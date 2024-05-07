#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <time.h>
using namespace sf;
using namespace std;

class Ghost
{
public:
    Texture ghost_t;
    Sprite ghost_s;
    Ghost *next = nullptr;
    Ghost *prev = nullptr;
    int currstate = 1;
    Ghost()
    {
    }
    Ghost(int type)
    {
        int y=type/4;
        type=type%4;
        srand(time(0));
        if (type == 0)
        {
            ghost_t.loadFromFile("images/ghost1.png");
        }
        else if (type == 1)
        {
            ghost_t.loadFromFile("images/ghost2.png");
        }
        else
        {
            ghost_t.loadFromFile("images/ghost3.png");
        }
        ghost_s.setTexture(ghost_t);
        ghost_s.setPosition(type*80 + 500,  y*60 + 360);
        ghost_s.setScale(0.2, 0.2);
    }
};

class Ghostlist
{
public:
    Ghost *start = nullptr;
    Ghostlist() {}
    void create(int i)
    {

        Ghost *create = new Ghost(i);
        if (start == nullptr)
        {
            start = create;
            return;
        }
        else
        {
            Ghost *curr = start;
            while (curr->next)
            {
                curr = curr->next;
            }
            create->prev = curr;
            curr->next = create;
        }
    }
    void move(int x, int y)
    {
        if (start)
        {
            Ghost *curr = start;
            while (curr->next)
            {
                curr->ghost_s.setPosition(curr->ghost_s.getPosition().x + x, curr->ghost_s.getPosition().y + y);
                curr = curr->next;
            }
        }
    }
    void selfmove()
    {
        if (start)
        {
            Ghost *curr = start;
            while (curr->next)
            {
                if (curr->currstate == 1)
                {
                    curr->ghost_s.setPosition(curr->ghost_s.getPosition().x, curr->ghost_s.getPosition().y - 3);
                    curr->currstate = 2;
                }
                else
                {
                    curr->ghost_s.setPosition(curr->ghost_s.getPosition().x, curr->ghost_s.getPosition().y + 3);
                    curr->currstate = 1;
                }
                curr = curr->next;
            }
        }
    }
    void display(RenderWindow &window)
    {
        if (start)
        {
            Ghost *curr = start;
            while (curr->next)
            {

                window.draw(curr->ghost_s);
                curr = curr->next;
            }
        }
    }
};
