#include "Engine.h"
#include "Aliens.h"
#include "Bombs.h"
#include "Rockets.h"
#include "MyShip.h"



class BattleSpace
{

    /**********
     * This class handles all the interactions between the different entities 
     * which are aliens, user, bombs and rockets
     * It also decides who lives or dies
     * 
     * 
     ********/

    public :

        static const int ROWS = Aliens::rows;
        static const int COLUMNS = Aliens::cols;
        static const int NUM_ALIENS = Aliens::num_aliens;
       
        static const int ALIVE = 1;

        Aliens aliens;
        Bombs bombs;
        Rockets rockets;
        MyShip myship;
        
        BattleSpace();
        
        void process();
        void fireRocket(int x, int y);
        bool fireBomb();

        uint32_t getAlienBombPos();
        
        
        void drawAliens(Engine& engine);

        void drawRockets(Engine& engine);

        bool drawMyShip(Engine& engine);
        void moveMyShip(int x);
        void drawBombs(Engine& engine);

        
        bool moveAliens();
        bool moveRockets();
        bool moveBombs();
        void processAll();
        void processBattle();
        void updateGameStatus(Engine& engine);
        bool isUserAlive();
        bool isGameOn();
    private :

        bool isAHit(int x,  uint32_t mask);
        void processCollisionWithMyShip();
        void processBombsWithMyShip();
        int getTrailingZeroes(uint32_t v);

        
        
};

