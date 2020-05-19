#include "Engine.h"
#include "BattleSpace.h"

#include "stdio.h"


using namespace std;


void EngineMain()
{
	//srand((int) time(0));
	Engine engine;
	BattleSpace battleSpace;
	
	

	int x = (Engine::CanvasWidth - Engine::SpriteSize) / 2;
	double atime = 0;
	double rtime = 0;
	double ftime = 0;
	double btime = 0;
	double utime = 0;
	const int userstepsize = Engine::SpriteSize/2;
	
	while (engine.startFrame())
	{
		double timestamp = engine.getStopwatchElapsedSeconds();
		
		battleSpace.isUserAlive()&&battleSpace.drawMyShip(engine);
		
		
		battleSpace.drawAliens(engine);
		battleSpace.drawBombs(engine);
		battleSpace.drawRockets(engine);
		Engine::PlayerInput keys = engine.getPlayerInput();

		if(timestamp - utime > 0.1)
		{
			//Engine::PlayerInput keys = engine.getPlayerInput();

			if ((keys.left) && (x >= userstepsize)) x = x - userstepsize;
			if ((keys.right) && (x < Engine::CanvasWidth - Engine::SpriteSize)) x = x + userstepsize;
			
			battleSpace.moveMyShip(x);
			
			utime = timestamp;
			
		}

		if(timestamp - ftime > 0.4)
		{
			//Engine::PlayerInput keys = engine.getPlayerInput();

			if(keys.fire) battleSpace.fireRocket(x , (Engine::CanvasHeight - Engine::SpriteSize)/Engine::SpriteSize);
			
			ftime = timestamp;
			
		}

		if(timestamp - btime > 0.6)
		{

			battleSpace.isGameOn() && battleSpace.fireBomb();
			btime = timestamp;
			
		}


		
		if(timestamp - rtime> 0.2)
		{	
			
			battleSpace.isGameOn() && battleSpace.moveRockets();
			battleSpace.isGameOn() && battleSpace.moveBombs();
			battleSpace.processBattle();
			rtime = timestamp;
		}

		
	    
		if(timestamp - atime> 0.2)
		{
				
			battleSpace.isGameOn() && battleSpace.moveAliens();
			battleSpace.processBattle();
			atime = timestamp;
		}
		
		
		
		battleSpace.updateGameStatus(engine);
		
	}
	
}


