#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <time.h>
using namespace std;
using namespace sf;
class PowerPallet
{
public:
    Texture pallet_t;
    Sprite pallet_s;
    bool Visible=false;
    PowerPallet(){
        pallet_t.loadFromFile("images/powerpallet.png");
        pallet_s.setTexture(pallet_t);
        pallet_s.setScale(0.08,0.08);
    }
    void move(int x,int y){
        pallet_s.setPosition(pallet_s.getPosition().x+x,pallet_s.getPosition().y+y);
    }
    void display(RenderWindow& window){
        if(Visible){
            window.draw(pallet_s);
        }
    }

};