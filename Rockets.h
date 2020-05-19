#include <stdlib.h>
#include "Engine.h"
#include <vector>
#include <unordered_map>

using namespace std;

class Rockets
{
    public :
        static const uint32_t DEAD = 0;
        static const int maskBitCount = sizeof(uint32_t)*CHAR_BIT;
        static const int spacing = Engine::SpriteSize;
        
        
        Rockets();
        
        uint32_t getRocketsAtXPosition(int x);
        bool fire(int x, int y);
        bool vanish(int x, int y);
        void move();
        unordered_map<int, uint32_t>& getRocketsMap();

    private :
        
        /****
         * Rockets are stored as keys, values in a map
         * keys are the x co-ordinates of their position on screen
         * values are bit fields consistsing of each bit position representing 
         * the y co-ordinate(row) 
         * hence key = 2, value = 6 or (0110 in binary) would mean that there are live rockets
         * on column = 2 and rows = 1,2
         * The aliens move in steps of SpriteSize. hence in total there are 15 rows
         * on the screen
         ***/
        unordered_map<int, uint32_t> rocketMap;
        unordered_map<int, uint32_t>::iterator it;
        

};