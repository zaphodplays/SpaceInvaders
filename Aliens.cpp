#include "Aliens.h"
#include "Engine.h"
#include <vector>
#include "stdio.h"
#include <random>

using namespace std;





Aliens::Aliens()
{
    spawn();
    result = false;
   
}


void Aliens::spawn()
{
    for(int i = 0; i < Engine::CanvasHeight/spacing; i++)
    {
        int y = i*spacing;
        if(i < rows)
            alienMap[y] = ALL_ALIVE_ROW_MASK;
        else
            alienMap[y] = ALL_DEAD_ROW_MASK;
    }  
    mostbottompos = (rows - 1)*Engine::SpriteSize;
    mosttoppos = 0;
    currentMotion = Motion::Right;
    previousMotion = Motion::Right;
    num_live_aliens = num_aliens;
            
}

void Aliens::processAliens()
{
    // for all y rows, check the most LSB by anding with 1 
    // to find out the rightmost alien alive
    // check the MSB by anding with 0x80000
    // use lastmotion and currentmotion attributes to decide motion after DOWN
    int mostleftpos = maskBitCount;
    int mostrightpos = 0;
    int bottompos = -1;
    int toppos = Engine::CanvasWidth;
    it = alienMap.begin();
    // Iterate over the map using iterator
    while(it != alienMap.end())
    {
        
        int y = it->first;
        
        int val = it->second;
        
        if(val != ALL_DEAD_ROW_MASK)
        {
            
            /* if(y > bottompos) 
            {
                bottompos = y;
            } */
            //r = x ^ ((x ^ y) & -(x < y)); // max(x, y)
            bottompos = y ^ ((y ^ bottompos) & -(y < bottompos));
            toppos = y ^ ((toppos ^ y) & -(toppos < y));
            int leftpos = getLeftPos(val);
            int rightpos = getRightPos(val);
            
            /* if(leftpos < mostleftpos) 
            {
                mostleftpos = leftpos;
            } */
            // min(x, y)
            mostleftpos = mostleftpos ^ ((leftpos ^ mostleftpos) & -(leftpos < mostleftpos));
            /* if(rightpos > mostrightpos) 
            {
                mostrightpos = rightpos;
            } */
            //r = x ^ ((x ^ y) & -(x < y)); // max(x, y)
            mostrightpos = rightpos ^ ((rightpos ^ mostrightpos) & -(rightpos < mostrightpos)); 


            
        }
        it++;
    }
    mostbottompos = bottompos;
    mosttoppos = toppos;
    
    
     if(mostbottompos == -1) 
    {
        spawn();
        return;
    }
    //if(mostbottompos != -1)
        decideNextMove(mostleftpos, mostrightpos);
}

/*****
 * decides the direction to move depending on the spatial arrangement of the 
 * alien stack
 ****/
void Aliens::decideNextMove(int minleftpos, int maxrightpos)
{
    /* if(currentMotion == Motion::Stop)
    {
        result = true;
    } */
    if(mostbottompos == Engine::CanvasHeight - Engine::SpriteSize)
    {
        result = true;
        currentMotion = Motion::Stop;
        return;
    }
    else if( (currentMotion == Motion::Right) && maxrightpos == (Engine::CanvasWidth - Engine::SpriteSize)/Engine::SpriteSize ) 
    {   
        //Next motion is to Go Down
        //printf("right:go down");
        currentMotion = Motion::Down;
        previousMotion = Motion::Right;
    }
    else if ( (currentMotion == Motion::Left) && (minleftpos == (0)))
    {
        //Next motion is to Go Down
        //printf("left:go down");
        currentMotion = Motion::Down;
        previousMotion = Motion::Left;
    }
    else if ( currentMotion == Motion::Down && previousMotion == Motion::Right)
    {
        // Next Motion is to Turn Left;
        //printf("down:go left");
        currentMotion = Motion::Left;
        previousMotion = Motion::Down;
    }
    else if( currentMotion == Motion::Down && previousMotion == Motion::Left)
    {
        //Next Motion is to Turn Right
        //printf("down:go right");
        currentMotion = Motion::Right;
        previousMotion = Motion::Down;
    }
     
}


/******
 * Here we move the aliens depending on the decided direction
 * FOR LEFT:: we rightshift all the bitfields
 * FOR RIGHT:: we leftshift all the bitfields
 *****/
void Aliens::moveAliens()
{
    //int minrow = mostbottompos - (rows-1)*Engine::SpriteSize;
    switch(currentMotion)
    {
        case Motion::Right :

            it = alienMap.begin();
            // Iterate over the map using iterator
            while(it != alienMap.end())
            {
                int y = it->first;
                if(y > mostbottompos) 
                {
                    it++;
                    continue;
                }
                
                
                uint32_t val = it->second;
                
                if(val != ALL_DEAD_ROW_MASK)
                    alienMap[it->first] =  (val<<1);

                it++;

            }
            break;  

        case Motion::Left :
            it = alienMap.begin();
            // Iterate over the map using iterator
            while(it != alienMap.end())
            {
                int y = it->first;
                

                if(y > mostbottompos) 
                {
                    it++;
                    continue;
                }
                uint32_t val = it->second;
                if(val != ALL_DEAD_ROW_MASK)
                    alienMap[it->first] =  val>>1;

                it++;

            }   
            break;

        case Motion::Down :
            
            for (int i = mostbottompos; i >= mosttoppos ; i = i - Engine::SpriteSize)
            {
                alienMap[i+spacing] = alienMap[i];
                alienMap[i] = ALL_DEAD_ROW_MASK;    
            }
            
            break;   

        case Motion::Up :
            break; 

        case Motion:: Stop :
            break;

        
    }
} 
/****
 * This Unsets the x-th bit in the value bitfield for key = y
 ***/
int Aliens::removeAlien(int x, int y)
{
    uint32_t val = alienMap[y];
    uint32_t removemask = ~(1<<x);
    
    alienMap[y] = val & removemask;
    num_live_aliens--;
    (!num_live_aliens) && num_defeats++;
    return 1;
}

int Aliens::getNumDefeats()
{
    return num_defeats;
}




/****
 * This generates a random alien position from which a bomb can be fired
 * The range of random numbers generated are between {1 , number of live aliens + 1}
 * 
 ***/
uint32_t Aliens::getNewBombPos()
{
    if(!num_live_aliens)
        return 0;
    
    
    int rnum = rand() % (num_live_aliens+1);
    
    !rnum&&++rnum;
    int val = 0;
    int lastval = 0;
    int ypos = 0;
    int xpos = 0;
    it = alienMap.begin();
    while(it != alienMap.end())
    {
        int row = it->first;
        
        uint32_t posx = it->second;
        lastval = val;
        
        val = val + setcount(posx); 
        if(val >= rnum)
        {
            ypos = row/Engine::SpriteSize;
            
            xpos = (rnum - lastval)-1;
            
            xpos = findNthSetBitPos(posx, xpos);
            break;
        } 
        it++;   
    }
    
    uint32_t pos = (ypos*Engine::CanvasWidth + xpos)*Engine::SpriteSize;
    return pos;

}


/****
 * Find the distance of the left most Alien from the left screen edge
 * This is effectively the position of the LSB in our bits field
 * 
 ***/
int Aliens::getLeftPos(uint32_t val)
{
    
    //return val & (val - 1);
    return (getTrailingZeroes(val));
    
}

//Count the consecutive zero bits (trailing) on the right linearly
// we are using this method now instead of __built_ctzl because 
// visual studio does not support it
int Aliens::getTrailingZeroes(uint32_t v)
{

    int c;  // output: c will count v's trailing zero bits,
            // so if v is 1101000 (base 2), then c will be 3
    if (v)
    {
        v = (v ^ (v - 1)) >> 1;  // Set v's trailing 0s to 1s and zero rest
        for (c = 0; v; c++)
        {
            v >>= 1;
        }
    }
    else
    {
        c = CHAR_BIT * sizeof(v);
    }
    return c;
}


/****
 * Find the distance of the right most Alien from the right screen edge
 * This is effectively the position of the MSB in our bits field
 * 
 ***/
int Aliens::getRightPos(uint32_t val)
{
    
    return msb_int(val);
    
}

/***
 * returns true if an alien reaches the bottom of the screen alive
 ***/
bool Aliens::didWeWin()
{
    return result;
}

int Aliens::getAliensAtY(int y)
{
    return alienMap[y];    
}

int Aliens::getNumLiveAliens()
{
    return num_live_aliens;
}

 map<int, uint32_t> & Aliens::getAliensMap()
{
    return alienMap;
}


/***
 * This method returns the position of the MSB in an unsigned 32 bit integer
 ***/
int Aliens::msb_int(uint32_t n) {
	//int ret = sizeof(uint32_t) * CHAR_BIT - 1;
	//return x ? ret - __builtin_clz(x) : ret;

    return (int)(log2(n));
}

/**
 * Divide and Conquer paradigm
    ---------------------------------------------
 |   v    |   (v >> 1) & 0b0101   |  v - x   |
 ---------------------------------------------
   0b00           0b00               0b00   
   0b01           0b00               0b01     
   0b10           0b01               0b01
   0b11           0b01               0b10
 **/
/*****
 * This method counts the number of set bits in a 32 bit decimal
 * time complexity O(n)
 * 
 ***/

int Aliens::setcount(uint32_t v) {
    v = v - ((v >> 1) & 0x55555555);                // put count of each 2 bits into those 2 bits
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333); // put count of each 4 bits into those 4 bits  
    return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}


/****
 * find the position of the nth set bit
 ***/
unsigned int Aliens::findNthSetBitPos(uint32_t value, unsigned int n)
{
    const uint32_t  pop2  = (value & 0x55555555u) + ((value >> 1) & 0x55555555u);
    const uint32_t  pop4  = (pop2  & 0x33333333u) + ((pop2  >> 2) & 0x33333333u);
    const uint32_t  pop8  = (pop4  & 0x0f0f0f0fu) + ((pop4  >> 4) & 0x0f0f0f0fu);
    const uint32_t  pop16 = (pop8  & 0x00ff00ffu) + ((pop8  >> 8) & 0x00ff00ffu);
    const uint32_t  pop32 = (pop16 & 0x000000ffu) + ((pop16 >>16) & 0x000000ffu);
    unsigned int    rank  = 0;
    unsigned int    temp;

    if (n++ >= pop32)
        return 32;

    temp = pop16 & 0xffu;
    /* if (n > temp) { n -= temp; rank += 16; } */
    rank += ((temp - n) & 256) >> 4;
    n -= temp & ((temp - n) >> 8);

    temp = (pop8 >> rank) & 0xffu;
    /* if (n > temp) { n -= temp; rank += 8; } */
    rank += ((temp - n) & 256) >> 5;
    n -= temp & ((temp - n) >> 8);

    temp = (pop4 >> rank) & 0x0fu;
    /* if (n > temp) { n -= temp; rank += 4; } */
    rank += ((temp - n) & 256) >> 6;
    n -= temp & ((temp - n) >> 8);

    temp = (pop2 >> rank) & 0x03u;
    /* if (n > temp) { n -= temp; rank += 2; } */
    rank += ((temp - n) & 256) >> 7;
    n -= temp & ((temp - n) >> 8);

    temp = (value >> rank) & 0x01u;
    /* if (n > temp) rank += 1; */
    rank += ((temp - n) & 256) >> 8;

    return rank;
}


