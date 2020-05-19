#include<vector>
#include <map>
#include"Engine.h"


using namespace std;


class Aliens 
{
    public :
        static const int num_aliens = 25;
        static const int rows = 5;
        static const int cols  = 5;

    private :
        static const int ALL_DEAD = 999;
        
        static const uint32_t MSB_MASK = 1<<rows;
        static const uint32_t LSB_MASK = 1;
        static const int spacing = Engine::SpriteSize;
        
        
        
        static const int ALIVE = 1;
        

        enum class Motion : int
        {
            Left = 1,
            Right = 2,
            Up = 3,
            Down = 4,
            Stop = 5
        };
        
        
        int aliens[num_aliens];
        
        Motion currentMotion;
        Motion previousMotion;
        int mostbottompos;
        int mosttoppos;
        int num_live_aliens;
        int num_defeats;

        /****
         * Aliens are stored as keys, values in a map
         * keys are the y co-ordinates of their position on screen
         * values consist of each bit position representing the x co-ordinate(column) 
         * hence value = 6 or (0110 in binary) would mean that there are live aliens 
         * on column = 1 and column = 2
         * The aliens move in steps of SpriteSize. hence in total there are 20 columns
         * on the screen
         ***/
        map<int, uint32_t> alienMap;
        map<int, uint32_t>::iterator it;

        int getLeftPos(uint32_t val);
        int getRightPos(uint32_t val);
        bool result;
        
    public :

        static const uint32_t ALL_ALIVE_ROW_MASK = (1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4);
        static const uint32_t ALL_DEAD_ROW_MASK = 0x0000;

        Aliens();
        static const int DEAD = -999;
        static const int maskBitCount = sizeof(uint32_t)*CHAR_BIT;
        
        int* getAliens();
        void move();
        int getLeftMostAlienPosition();
        int getRightMostAlienPosition();
        Motion getLastMotion();
        void process();

        void spawn();
        void processAliens();
        void decideNextMove(int mostleftpos, int mostrightpos);
        bool didWeWin();
        void moveAliens();
        int getAliensAtY(int y);
        map<int, uint32_t> & getAliensMap();
        int removeAlien(int x, int y);
        uint32_t getNewBombPos();
        int getNumLiveAliens();
        int getNumDefeats();
        
    private :
        
        int msb_int(uint32_t x);
        int getSetBitsCount(uint32_t val);
        void initializeLookupTable();
        int getTrailingZeroes(uint32_t v);
        
        unsigned int findNthSetBitPos(uint32_t value, unsigned int n);
        int setcount(uint32_t v);
        

};