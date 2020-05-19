#include <stdlib.h>
#include <vector>
#include "Bombs.h"


using namespace std;

Bombs::Bombs()
{
    for(int i = 0; i < Engine::CanvasWidth; i++)
    {
        //initialize all keys with values of DEAD mask for every row
        bombMap[i] = DEAD;
        
    }
}

uint32_t Bombs::getBombsAtXPosition(int x)
{
    return bombMap[x];
}

/***
 * set the yth bit in the value bitfield to generate the bomb at x,y 
 * where x co-ordinate acts as the key
 **/
void Bombs::fire(int x, uint32_t y)
{
    bombMap[x] |= 1 << (y); 
    
}


/***
 * Un-set the yth bit in the value bitfield to eliminate the bomb at x,y 
 * where x co-ordinate acts as the key
 **/
bool Bombs::vanish(int x, int y) 
{
    bombMap[x] &= ~(1 << (y));
    return true;
}


/*****
 * move all the bombs downwards by one step by leftshifting all the value bitfields
 *  by 1 bit position.
 * This effectively means increasing the y co-ordinate of each rocket by one step(SpriteSize)
 ****/
void Bombs::move()
{
    // Iterate over keys and modify values of unordered_map
    it = bombMap.begin();
 
    // Iterate over the map using iterator
    while(it != bombMap.end())
    {
        
        
        //it->second << 1;
        bombMap[it->first] = it->second<<1;
        
        it++;
    }
    
}



