#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
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

const int GG=0;
pthread_t threadsG[GG];
int valuesG[GG];
pthread_mutex_t busyG[GG];

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


const int Tnumb=3;
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
    Clock mouth_open, automoving, ghostselfmove, movement, moveout, teleporting, critical_state_ghost, pallettimer, eatghosts;
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
    void moveghostsout(int i)
    {
        if (ghosts.start)
        {
            Ghost *curr = &(ghosts.start[i]);

            curr->ghost_s.setPosition(maze.centerx - 30 * i, maze.centery + 5);
            curr->home = false;

        }
    }

    //interface
    void interface_Game(int Tnum){
        while (window.isOpen()){
            //bakery_lock(Tnum);
                window.clear();
                window.draw(player.pacman_S);

                float time_left = pallettimer.getElapsedTime().asSeconds();
                float val = (time_left) / 15.0;
                food.display(window);
                //cout<<"before ghosts\n";
                ghosts.display(window);
                //cout<<"after ghosts\n";
                //pallet.display(window);
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
                // if(newEvent==1){
                //     newEvent=0;
                //     if (keypress.type == Event::Closed)
                //         window.close();
                //     else if (keypress.type == Event::KeyPressed)
                //     {
                //         move(keypress);
                //         player.face_movement(1);
                //     }
                // }
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
                // checkfood();
                cout<<"\tbruh"<<endl;
                //pacman mouth move
                // if (mouth_open.getElapsedTime().asSeconds() >= 0.4)
                // {
                //     player.face_movement(0);
                //     mouth_open.restart();
                // }
                //pacman automove
                if (automoving.getElapsedTime().asSeconds() >= 0.03)
                {
                    automove();
                    automoving.restart();
                }
                cout<<"\tbruh2"<<endl;
                //something related to teleporting idk
                // if (teleporting.getElapsedTime().asSeconds() >= 0.8f && teleported)
                // {
                //     teleported = false;
                // }
                //self move on ghosts idk what this does either
                // if (ghostselfmove.getElapsedTime().asSeconds() >= 0.06)
                // {
                //     ghosts.updateself();
                //     ghostselfmove.restart();
                // }
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
        //moveghostsout(Tnum);
        while (window.isOpen()){

            Gbakery_lock(Tnum);
                //ghosts.selfmove(Tnum);
                ghosts.move(Tnum);
            Gbakery_unlock(Tnum);


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

void* ghost_t(void*arg){
    int Tnum=*((int*)arg);
        G.ghost_Game(Tnum);
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

    srand(time(0));
    //interface
    pthread_create(&threads[0],NULL,interface_t,(void*)(new int(0)));
    //pthread_create(&threads[1],NULL,input_t,(void*)(new int(1)));
    pthread_create(&threads[2],NULL,engine_t,(void*)(new int(2)));
    // pthread_create(&threads[3],NULL,food_t,(void*)(new int(3)));

    for(int i=0;i<GG;i++){
        //pthread_create(&threadsG[i],NULL,ghost_t,(void*)(new int(i)));
    }
    //main thread
    while (window2.isOpen()){}




}