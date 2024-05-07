#include <SFML/Graphics.hpp>
#include <fstream>
using namespace std;
using namespace sf;
class Food
{
public:
    Texture food_T;
    Sprite *food_S;
    int *valid;
    int **food_coords;
    int n_food;
    Food()
    {
        ifstream file_food("data/food.txt");
        food_T.loadFromFile("images/food.png");
        file_food >> n_food;
        food_coords = new int *[n_food];
        food_S = new Sprite[n_food];
        valid = new int[n_food];
        for (int i = 0; i < n_food; i++)
        {
            food_coords[i] = new int[2];
            file_food >> food_coords[i][0];
            file_food >> food_coords[i][1];
        }
        for (int i = 0; i < n_food; i++)
        {
            valid[i] = 1;
            food_S[i].setTexture(food_T);
            food_S[i].setPosition(food_coords[i][0], food_coords[i][1]);
            food_S[i].setScale(0.03, 0.03);
        }
    }
    void move(int x, int y)
    {
        for (int i = 0; i < n_food; i++)
        {
            food_coords[i][0] += x;
            food_coords[i][1] += y;
            food_S[i].setPosition(food_coords[i][0], food_coords[i][1]);
        }
    }
    void display(RenderWindow &window)
    {
        for (int i = 0; i < n_food; i++)
        {
            if (valid[i])
            {
                window.draw(food_S[i]);
            }
        }
    }
};