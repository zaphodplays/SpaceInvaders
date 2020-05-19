#include "Engine.h"

class MyShip
{
    private :
        int health;
        int score;
        int speed;
        int x;

    public :
        static const int spacing = Engine::SpriteSize;
        int getX();
        void setX(int x);
       
        int getHealth();
        void setHealth(int h);
        bool hitship();
        int getSpeed();
        void setSpeed(int speed);
        void incrementScore();
        int getScore();
        bool isAlive();


        MyShip();
        

};