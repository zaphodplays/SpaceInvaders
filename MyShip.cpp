#include <stdio.h>
#include "MyShip.h"

MyShip::MyShip()
{
    health = 3;
    score = 1;
}



int MyShip::getX()
{
    return x;
}

void MyShip::setX(int xpos)
{
    x = xpos;
}

int MyShip::getHealth()
{
    return health;
}

void MyShip::setHealth(int h)
{
    health = h;
}

bool MyShip::hitship()
{
     health--;
     return health > 0;

}

void MyShip::setSpeed(int s)
{
    speed = s;
}

int MyShip::getSpeed()
{
    return speed;
}

void MyShip::incrementScore()
{
    ++score;
}

int MyShip::getScore()
{
    return score;
}

bool MyShip::isAlive()
{
    return (health > 0);
}