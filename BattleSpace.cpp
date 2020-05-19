#include "BattleSpace.h"
#include <unordered_map>
#include <map>
#include <iterator>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <chrono>
#include <thread>

BattleSpace::BattleSpace()
{
    aliens = Aliens();
	bombs = Bombs();
    rockets = Rockets();
	myship = MyShip();
	//printf("initialized battlespace");
}


/****
 * All the wartime processing is done here. user ship is checked 
 * for collision with aliens and bombs. user rockets are checked 
 * for hits with aliens. 
 * 
 * 
 **/
void BattleSpace::processBattle()
{
	processCollisionWithMyShip();
	processBombsWithMyShip();
	unordered_map<int, uint32_t> rocketsMap = rockets.getRocketsMap();
	unordered_map<int, uint32_t>::iterator it = rocketsMap.begin();
	while( it != rocketsMap.end() )
	{
		int col = it->first;
		int mask = it->second;
		if(mask != Rockets::DEAD) 
		{
			
			while (mask != 0) 
			{
				//printf("mask = %d\t", mask);
				int t = int(mask & -mask);
				//int r = __builtin_ctz(mask);
				int r = getTrailingZeroes(mask);
				int row = r*Engine::SpriteSize;
				uint32_t alienxmask = aliens.getAliensAtY(row);
				uint32_t hit = isAHit(col, alienxmask);
				//hit&&printf("checking hit at %d,%d,%d\t", col/Engine::SpriteSize,r, hit);
				hit&&aliens.removeAlien(col/Engine::SpriteSize, row);
				hit&&rockets.vanish(col, r);
				mask^= t;
			}
		}

		it++;
	}	
	

}


/*****
 * check collision of aliens with user ship
 * 
 ***/
void BattleSpace::processCollisionWithMyShip()
{
	uint32_t suspects = aliens.getAliensAtY(Engine::CanvasHeight - Engine::SpriteSize);
	bool hit = (suspects)&&(suspects & (1<<(myship.getX()/Engine::SpriteSize)) );
	//hit&&printf("process collision:: suspects = %d, ship at %d, mask is %d\t", suspects, myship.getX(), suspects & (1<<(myship.getX()/Engine::SpriteSize)));
	hit && aliens.removeAlien(myship.getX()/Engine::SpriteSize, Engine::CanvasHeight - Engine::SpriteSize)
	&& myship.hitship();
	//printf("collision:: after health = %d\t", myship.getHealth());
}


/****
 * check collision of bombs with user ship
 * 
 ***/
void BattleSpace::processBombsWithMyShip()
{
	int shipypos = (Engine::CanvasHeight - Engine::SpriteSize)/Engine::SpriteSize;
	int shipxpos = myship.getX();
	for(int i = -1; i <=1; i++)
	{
		int newshipxpos = shipxpos + i*Engine::SpriteSize/2;
	    if(	((newshipxpos < 0)) || ( newshipxpos > (Engine::CanvasWidth - Engine::SpriteSize)) ) 
		{
			continue;
		}

		uint32_t suspects = bombs.getBombsAtXPosition(newshipxpos);
		bool hit = (suspects)&&(suspects & (1<<(shipypos)) );
		hit&&printf("process bombed:: suspects = %d, ship at %d, mask is %d\t", suspects, shipxpos, suspects & (1<<(shipypos)));
		hit && bombs.vanish(newshipxpos, shipypos)
		&& myship.hitship();
		
	}
	
	
}

/****
 * get a new random bomb posiiton 
 * 
 ***/
uint32_t BattleSpace::getAlienBombPos()
{
	return aliens.getNewBombPos();
}

void BattleSpace::fireRocket(int x, int y)
{
    myship.isAlive()&&rockets.fire(x, y);
}


void BattleSpace::drawAliens(Engine& engine)
{
    for(int y = 0; y < Engine::CanvasWidth - Engine::SpriteSize; y = y + Engine::SpriteSize)
	{
		int mask = aliens.getAliensAtY(y);
		if(mask != Aliens::DEAD)
		{
			while (mask != 0) 
				{
					
					int t = (mask & -mask);
					//uint32_t c = __builtin_ctzl(mask);
					int c = getTrailingZeroes(mask);
					engine.drawSprite(Engine::Sprite::Enemy1,c*Engine::SpriteSize, y);
					mask^= t;
				}	
		}		
	}
	
 
}

void BattleSpace::drawRockets(Engine& engine) 
{
	
    for(int col = 0; col < Engine::CanvasWidth; col = col + Engine::SpriteSize/2)
		{
			int mask = rockets.getRocketsAtXPosition(col);
			if(mask != Rockets::DEAD) 
			{
				
				while (mask != 0) 
				{
					//get all the set bits one by one by efficiently ignoring 
					//the trailing zeroes in between
					int t = (mask & -mask);
					//int r = __builtin_ctzl(mask);
					int r = getTrailingZeroes(mask);
					
					engine.drawSprite(Engine::Sprite::Rocket,col, r*Engine::SpriteSize);
					mask^= t;
				}
			}
		}
}

bool BattleSpace::drawMyShip(Engine& engine)
{
	engine.drawSprite(
			Engine::Sprite::Player,
			myship.getX(), Engine::CanvasHeight - Engine::SpriteSize);
	return true;	
}

void BattleSpace::drawBombs(Engine& engine)
{
	for(int col = 0; col<Engine::CanvasWidth; col = col + Engine::SpriteSize)
		{
			int mask = bombs.getBombsAtXPosition(col);
			if(mask != Bombs::DEAD) 
			{
				
				while (mask != 0) 
				{
					
					int t =  (mask & -mask);
					int r = getTrailingZeroes(mask);
					engine.drawSprite(Engine::Sprite::Bomb,col, r*Engine::SpriteSize);
					mask^= t;
				}
			}
		}	
}

void BattleSpace::moveMyShip(int x)
{
	myship.setX(x);
}

bool BattleSpace::isUserAlive()
{
	return myship.isAlive();
}



bool BattleSpace::moveAliens()
{
	aliens.processAliens();
	aliens.moveAliens();
	return true;
}

bool BattleSpace::moveRockets()
{
	rockets.move();
	return true;
}

bool BattleSpace::moveBombs()
{
	bombs.move();
	return true;
	
}

bool BattleSpace::fireBomb()
{
	if(!aliens.getNumLiveAliens())
		return false;
	uint32_t pos = getAlienBombPos();
	int row = (pos/Engine::CanvasWidth)/Engine::SpriteSize;
	int col = (pos - row*Engine::CanvasWidth*Engine::SpriteSize);
	bombs.fire(col, row);
	return true;

}

/****
 * did the rocket hit the alien
 ***/
bool BattleSpace::isAHit(int x,  uint32_t mask)
{
	x = x/Engine::SpriteSize;
	return ( (mask & (1 << (x%Rockets::maskBitCount) )) != 0 );
	
}

bool BattleSpace::isGameOn()
{
	return (!aliens.didWeWin()) && myship.isAlive();
}

void BattleSpace::updateGameStatus(Engine& engine)
{
	if(!isGameOn())
		{
			const char message[] = "ALIENS WON!!";
		    
		
			engine.drawText(message,
			(Engine::CanvasWidth - (sizeof(message) - 1) * Engine::FontWidth) / 2,
			(Engine::CanvasHeight - Engine::FontRowHeight) / 2);
			
		}
	
	else 
	{
		
		string s = "ROUND " + std::to_string(aliens.getNumDefeats()+1) + " HEALTH = " + std::to_string(myship.getHealth());
		char message[s.length() + 1]; 
		strcpy(message, s.c_str());    
		
		engine.drawText(message,
		(Engine::CanvasWidth - (sizeof(message) - 1) * Engine::FontWidth) / 2,
		(Engine::CanvasHeight - Engine::FontRowHeight) / 2);	
	}
		

}

//Count the consecutive zero bits (trailing) on the right linearly
// we are using this method now instead of __built_ctzl because 
// visual studio does not support it
int BattleSpace::getTrailingZeroes(uint32_t v)
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