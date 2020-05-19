#include "Rockets.h"
#include "stdio.h"



using namespace std;

Rockets::Rockets()
{
    for(int i = 0; i < Engine::CanvasWidth; i++)
    {
    
        rocketMap[i] = DEAD;
        
    }
    
} 



uint32_t Rockets::getRocketsAtXPosition(int x) 
{
    return rocketMap[x];
}

/***
 * set the yth bit in the value bitfield to generate the rocket at x,y 
 * where x co-ordinate acts as the key
 **/
bool Rockets::fire(int x, int y) 
{
     
    rocketMap[x] |= 1 << (y); 
    return true;
}

/***
 * Un-set the yth bit in the value bitfield to eliminate the rocket at x,y 
 * where x co-ordinate acts as the key
 **/
bool Rockets::vanish(int x, int y) 
{
    rocketMap[x] &= ~(1 << (y));
    return true;
}



/*****
 * move all the rockets upwards by rightshifting all the value bitfields by 1 position
 * This effectively means decreasing the y co-ordinate of each rocket by one step(SpriteSize)
 ****/
void Rockets::move()
{
    // Iterate over keys and modify values of unordered_map
    it = rocketMap.begin();
 
    // Iterate over the map using iterator
    while(it != rocketMap.end())
    {
        
        
        //it->second << 1;
        rocketMap[it->first] = it->second>>1;
        
        it++;
    }
    
}



unordered_map<int, uint32_t>& Rockets::getRocketsMap()
{
    return rocketMap;
}





