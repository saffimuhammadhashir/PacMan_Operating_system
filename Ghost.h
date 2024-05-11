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
    bool home = true;
    int resident = 0;
    bool criticalstate = false;
    int prevx = 0;
    int prevy = 0;
    int dir = (rand() % 2 == 0) ? 1 : 3;
    int mytype;
    int myytype;
    Ghost()
    {
    }
    Ghost(int type)
    {
        int y = type / 4;
        myytype=y;
        type = type % 4;
        mytype = type;
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
        ghost_s.setPosition(type * 80 + 510, y * 60 + 360);
        ghost_s.setScale(0.15, 0.15);
    }
};

class Ghostlist
{
public:
    Ghost *start;
    int Gsize;
    int x;
    int y;
    int pchange;
    int schange;
    int pchange1;
    int schange1;
    int outside;
    int huntT;
    Ghostlist() {
        start=NULL;
        Gsize=0;
        x=0;
        y=0;
        pchange=0;
        schange=0;
        pchange1=0;
        schange1=0;
        outside=0;
        huntT=0;
    }
    void create(int i)
    {

        Ghost *create = new Ghost(i);
        if (start == nullptr)
        {
            start = create;
            Gsize++;
            return;
        }
        else
        {
            Ghost *temp=new Ghost[Gsize+1];
            for(int i=0;i<Gsize;i++){
                temp[i]=start[i];
            }
            temp[Gsize]=*create;
            start=temp;
            Gsize++;
        }
    }

    void updatechange(int xx,int yy){
        pchange=Gsize;
        pchange1=Gsize;
        x=xx;
        y=yy;
    }

    void updatehunt(){
        huntT=Gsize;
    }

    void updateself(){
        schange=Gsize;
        schange1=Gsize;
    }

    void move(int i)
    {
        if (start && pchange>Gsize-outside){
            pchange--;
            start[i].ghost_s.setPosition(start[i].ghost_s.getPosition().x + x, start[i].ghost_s.getPosition().y + y);
        }

    }
    void selfmove(int i)
    {
        if (start && schange>Gsize-outside)
        {
            schange--;
            if (start[i].currstate == 1)
            {
                start[i].ghost_s.setPosition(start[i].ghost_s.getPosition().x, start[i].ghost_s.getPosition().y - 2);
                start[i].currstate = 2;
            }
            else
            {
                start[i].ghost_s.setPosition(start[i].ghost_s.getPosition().x, start[i].ghost_s.getPosition().y + 2);
                start[i].currstate = 1;
            }

        }
    }
    void move1(int i)
    {
        if (start && pchange1>outside){
            pchange1--;
            start[i].ghost_s.setPosition(start[i].ghost_s.getPosition().x + x, start[i].ghost_s.getPosition().y + y);
        }

    }
    void selfmove1(int i)
    {
        if (start && schange1>outside)
        {
            schange1--;
            if (start[i].currstate == 1)
            {
                start[i].ghost_s.setPosition(start[i].ghost_s.getPosition().x, start[i].ghost_s.getPosition().y - 2);
                start[i].currstate = 2;
            }
            else
            {
                start[i].ghost_s.setPosition(start[i].ghost_s.getPosition().x, start[i].ghost_s.getPosition().y + 2);
                start[i].currstate = 1;
            }

        }
    }
    void display(RenderWindow &window)
    {
        if (start)
        {
            for(int i=0;i<Gsize;i++)
            {
                window.draw(start[i].ghost_s);
            }
        }
    }
    void blink()
    {
        if (start)
        {
            for(int i=0;i<Gsize;i++){
                Ghost *curr = &start[i];
                curr->ghost_t.loadFromFile("images/dyingghost.png");
                curr->ghost_s.setTexture(curr->ghost_t);
            }
        }
    }
    void unblink()
    {
        if (start)
        {
            for(int i=0;i<Gsize;i++){
                Ghost *curr = &start[i];

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
            }
        }
    }
};
