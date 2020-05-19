#include <stdlib.h>
#include "Engine.h"
#include <vector>
#include <unordered_map>

using namespace std;

class Bombs
{
    public :
        static const uint32_t DEAD = 0;
       
        
        
        
        Bombs();
        
        uint32_t getBombsAtXPosition(int x);
        void fire(int x, uint32_t y);
        bool vanish(int x, int y);
        void move();
        void process();


    private :
        /****
         * Bombs are stored as keys, values in a map
         * keys are the x co-ordinates of their position on screen
         * values are bit fields that consist of each bit position 
         * representing the y co-ordinate(row) 
         * hence value = 6 or (0110 in binary) would mean that there are live aliens 
         * on row = 1 and row = 2
         * The aliens move in steps of SpriteSize. hence in total there are 15 rows
         * on the screen
         ***/
        unordered_map<int, uint32_t> bombMap;
        unordered_map<int, uint32_t>::iterator it;
        

};