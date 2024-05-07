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
    int **lefttopwalls;
    int **righttopwalls;
    int **leftbottomwalls;
    int **rightbottomwalls;
    int **verticalwalls;
    Texture bg_main_T;
    Texture bg2_main_T;
    Texture highscores_main_T;
    Texture level_main_T;
    Texture wall_T_horizontal;
    Texture wall_T_vertical;
    Texture wall_T_lefttop;
    Texture wall_T_righttop;
    Texture wall_T_leftbottom;
    Texture wall_T_rightbottom;
    Texture Y1teleportation;
    Texture Y2teleportation;
    Texture Xteleportation;
    Font font;
    Sprite bg_main_S;
    Sprite bg2_main_S;
    Sprite highscores_main_S;
    Sprite level_main_S;
    Sprite Teleportationy1;
    Sprite Teleportationy2;
    Sprite Teleportationx;
    Sprite *wall_s_horizontal;
    Sprite *wall_s_vertical;
    Sprite *wall_s_lefttop;
    Sprite *wall_s_righttop;
    Sprite *wall_s_leftbottom;
    Sprite *wall_s_rightbottom;
    Text pacman_title, Highscore_title, level_title, level_percentage, name_s, level_s;
    int n_horizontal;
    int n_vertical;
    int n_lefttop;
    int n_righttop;
    int n_leftbottom;
    int n_rightbottom;

    Maze()
    {
        ifstream file_horizontal("data/horizontal.txt");
        ifstream file_vertical("data/vertical.txt");
        ifstream file_lefttop("data/lefttop.txt");
        ifstream file_righttop("data/righttop.txt");
        ifstream file_rightbottom("data/rightbottom.txt");
        ifstream file_leftbottom("data/leftbottom.txt");
        font.loadFromFile("font/BebasNeue-Regular.ttf");
        file_horizontal >> n_horizontal;
        file_vertical >> n_vertical;
        file_lefttop >> n_lefttop;
        file_righttop >> n_righttop;
        file_rightbottom >> n_rightbottom;
        file_leftbottom >> n_leftbottom;
        horizontalwalls = new int *[n_horizontal];
        verticalwalls = new int *[n_vertical];
        lefttopwalls = new int *[n_lefttop];
        righttopwalls = new int *[n_righttop];
        leftbottomwalls = new int *[n_leftbottom];
        rightbottomwalls = new int *[n_rightbottom];

        for (int i = 0; i < n_horizontal; i++)
        {
            horizontalwalls[i] = new int[2];
            for (int j = 0; j < 2; j++)
            {
                file_horizontal >> horizontalwalls[i][j];
            }
        }
        for (int i = 0; i < n_vertical; i++)
        {
            verticalwalls[i] = new int[2];
            for (int j = 0; j < 2; j++)
            {
                file_vertical >> verticalwalls[i][j];
            }
        }
        for (int i = 0; i < n_lefttop; i++)
        {
            lefttopwalls[i] = new int[2];
            for (int j = 0; j < 2; j++)
            {
                file_lefttop >> lefttopwalls[i][j];
            }
        }
        for (int i = 0; i < n_righttop; i++)
        {
            righttopwalls[i] = new int[2];
            for (int j = 0; j < 2; j++)
            {
                file_righttop >> righttopwalls[i][j];
            }
        }
        for (int i = 0; i < n_leftbottom; i++)
        {
            leftbottomwalls[i] = new int[2];
            for (int j = 0; j < 2; j++)
            {
                file_leftbottom >> leftbottomwalls[i][j];
            }
        }
        for (int i = 0; i < n_rightbottom; i++)
        {
            rightbottomwalls[i] = new int[2];
            for (int j = 0; j < 2; j++)
            {
                file_rightbottom >> rightbottomwalls[i][j];
            }
        }

        wall_T_horizontal.loadFromFile("images/wall1_horizontal.png");
        wall_T_vertical.loadFromFile("images/wall1_vertical.png");
        wall_T_lefttop.loadFromFile("images/wall1_lt.png");
        wall_T_leftbottom.loadFromFile("images/wall1_lb.png");
        wall_T_rightbottom.loadFromFile("images/wall1_rb.png");
        wall_T_righttop.loadFromFile("images/wall1_rt.png");
        Y1teleportation.loadFromFile("images/teleportationy1.png");
        Y2teleportation.loadFromFile("images/teleportationy2.png");
        Xteleportation.loadFromFile("images/teleportationx.png");
        bg_main_T.loadFromFile("images/background.png");
        bg2_main_T.loadFromFile("images/background2.png");
        highscores_main_T.loadFromFile("images/highscorebox.png");
        level_main_T.loadFromFile("images/level.png");
        bg_main_S.setTexture(bg_main_T);
        bg2_main_S.setTexture(bg2_main_T);
        highscores_main_S.setTexture(highscores_main_T);
        level_main_S.setTexture(level_main_T);
        name_s.setCharacterSize(24);
        level_s.setCharacterSize(35);
        name_s.setFillColor(Color::White);
        level_s.setFillColor(Color::White);
        name_s.setPosition(990,400);
        level_s.setPosition(50,660);
        bg_main_S.setPosition(900, 0);
        bg2_main_S.setPosition(0, 650);
        highscores_main_S.setPosition(880, 420);
        level_main_S.setPosition(50, 680);
        bg_main_S.setScale(3.5f, 2.0f);
        bg2_main_S.setScale(2.5f, 1.7f);
        highscores_main_S.setScale(0.7f, 0.8f);
        level_main_S.setScale(1.3f, 1.3f);
        Teleportationy1.setTexture(Y1teleportation);
        Teleportationy2.setTexture(Y2teleportation);
        Teleportationx.setTexture(Xteleportation);
        Teleportationy1.setScale(0.08, 0.2);
        Teleportationy2.setScale(0.08, 0.2);
        Teleportationx.setScale(0.2, 0.08);
        Teleportationy1.setPosition(-5, 370);
        Teleportationy2.setPosition(1295, 370);
        Teleportationx.setPosition(580, 890);
        wall_s_horizontal = new Sprite[n_horizontal];
        wall_s_vertical = new Sprite[n_vertical];
        wall_s_lefttop = new Sprite[n_lefttop];
        wall_s_leftbottom = new Sprite[n_leftbottom];
        wall_s_righttop = new Sprite[n_righttop];
        wall_s_rightbottom = new Sprite[n_rightbottom];

        pacman_title.setFont(font);
        Highscore_title.setFont(font);
        level_title.setFont(font);
        level_percentage.setFont(font);
        name_s.setFont(font);
        level_s.setFont(font);
        pacman_title.setString("PacMan");
        Highscore_title.setString("Highscores");
        level_title.setString("Level Bar");

        pacman_title.setCharacterSize(80);
        Highscore_title.setCharacterSize(54);
        level_title.setCharacterSize(24);
        pacman_title.setFillColor(Color::White);
        Highscore_title.setFillColor(Color::Black);
        level_title.setFillColor(Color::Red);
        level_percentage.setCharacterSize(24);
        pacman_title.setPosition(960, 100);
        Highscore_title.setPosition(960, 480);
        level_title.setPosition(310, 670);
        level_percentage.setPosition(330, 710);

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
        for (int i = 0; i < n_lefttop; i++)
        {
            wall_s_lefttop[i].setTexture(wall_T_lefttop);
            wall_s_lefttop[i].setPosition(lefttopwalls[i][0], lefttopwalls[i][1]);
            wall_s_lefttop[i].setScale(0.1, 0.1);
        }
        for (int i = 0; i < n_leftbottom; i++)
        {
            wall_s_leftbottom[i].setTexture(wall_T_leftbottom);
            wall_s_leftbottom[i].setPosition(leftbottomwalls[i][0], leftbottomwalls[i][1]);
            wall_s_leftbottom[i].setScale(0.1, 0.1);
        }
        for (int i = 0; i < n_righttop; i++)
        {
            wall_s_righttop[i].setTexture(wall_T_righttop);
            wall_s_righttop[i].setPosition(righttopwalls[i][0], righttopwalls[i][1]);
            wall_s_righttop[i].setScale(0.1, 0.1);
        }
        for (int i = 0; i < n_rightbottom; i++)
        {
            wall_s_rightbottom[i].setTexture(wall_T_rightbottom);
            wall_s_rightbottom[i].setPosition(rightbottomwalls[i][0], rightbottomwalls[i][1]);
            wall_s_rightbottom[i].setScale(0.1, 0.1);
        }
    }

    void display(RenderWindow &window, int level, int validitycount, int foodcount, string name)
    {

        for (int i = 0; i < n_horizontal; i++)
        {
            window.draw(wall_s_horizontal[i]);
        }
        for (int i = 0; i < n_vertical; i++)
        {
            window.draw(wall_s_vertical[i]);
        }
        for (int i = 0; i < n_leftbottom; i++)
        {
            window.draw(wall_s_leftbottom[i]);
        }
        for (int i = 0; i < n_lefttop; i++)
        {
            window.draw(wall_s_lefttop[i]);
        }
        for (int i = 0; i < n_rightbottom; i++)
        {
            window.draw(wall_s_rightbottom[i]);
        }
        for (int i = 0; i < n_righttop; i++)
        {
            window.draw(wall_s_righttop[i]);
        }
        float scalingfactor = ((float)validitycount) / ((float)foodcount);
      
        level_main_S.setScale(scalingfactor, 1.3f);
        
        level_s.setString("Level "+to_string(level));
        name_s.setString("Name "+name);
        window.draw(Teleportationy1);
        window.draw(Teleportationy2);
        window.draw(Teleportationx);
        window.draw(bg_main_S);
        window.draw(bg2_main_S);
        window.draw(highscores_main_S);
        window.draw(level_main_S);
        window.draw(Highscore_title);
        window.draw(pacman_title);
        window.draw(level_title);
        window.draw(level_s);
        window.draw(name_s);
        level_percentage.setString(to_string((int)(scalingfactor * 100)) + "%");
        window.draw(level_percentage);
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

        for (int i = 0; i < n_leftbottom; i++)
        {
            leftbottomwalls[i][0] += x;
            leftbottomwalls[i][1] += y;
            wall_s_leftbottom[i].setPosition(leftbottomwalls[i][0], leftbottomwalls[i][1]);
        }
        for (int i = 0; i < n_lefttop; i++)
        {
            lefttopwalls[i][0] += x;
            lefttopwalls[i][1] += y;
            wall_s_lefttop[i].setPosition(lefttopwalls[i][0], lefttopwalls[i][1]);
        }
        for (int i = 0; i < n_righttop; i++)
        {
            righttopwalls[i][0] += x;
            righttopwalls[i][1] += y;
            wall_s_righttop[i].setPosition(righttopwalls[i][0], righttopwalls[i][1]);
        }
        for (int i = 0; i < n_rightbottom; i++)
        {
            rightbottomwalls[i][0] += x;
            rightbottomwalls[i][1] += y;
            wall_s_rightbottom[i].setPosition(rightbottomwalls[i][0], rightbottomwalls[i][1]);
        }
        sf::Vector2f currentPosition = Teleportationx.getPosition();
        Teleportationx.setPosition(currentPosition.x + x, currentPosition.y + y);
        currentPosition = Teleportationy1.getPosition();
        Teleportationy1.setPosition(currentPosition.x + x, currentPosition.y + y);
        currentPosition = Teleportationy2.getPosition();
        Teleportationy2.setPosition(currentPosition.x + x, currentPosition.y + y);
    }
};
