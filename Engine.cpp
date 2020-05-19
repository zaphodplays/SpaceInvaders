#include "Engine.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////
// DO NOT ADD YOUR CODE IN THIS FILE!
//////////////////////////////////////////////////////////////////////////////

namespace engine
{
	// A pixel is defined as having color 0xRRGGBB
	typedef uint32_t Pixel;

	static Pixel* canvas;

	void init();
	void deinit();
	void clearCanvas();
	void drawSprite(Engine::Sprite sprite, int x, int y);
	void drawText(const char* message, int x, int y);
}

namespace platform
{
	static Engine::PlayerInput playerInput;

	void init();
	void deinit();
	bool update(engine::Pixel* canvas);
	double getStopwatchElapsedSeconds();
}


// PUBLIC API

//////////////////////////////////////////////////////////////////////////////
// DO NOT ADD YOUR CODE IN THIS FILE!
//////////////////////////////////////////////////////////////////////////////

Engine::Engine()
{
	platform::init();
	engine::init();
}

Engine::~Engine()
{
	engine::deinit();
	platform::deinit();
}

bool Engine::startFrame()
{
	bool shouldKeepGoing = platform::update(engine::canvas);
	engine::clearCanvas();
	return shouldKeepGoing;
}

void Engine::drawSprite(Sprite sprite, int x, int y)
{
	engine::drawSprite(sprite, x, y);
}

void Engine::drawText(const char* message, int x, int y)
{
	engine::drawText(message, x, y);
}

double Engine::getStopwatchElapsedSeconds()
{
	return platform::getStopwatchElapsedSeconds();
}

Engine::PlayerInput Engine::getPlayerInput()
{
	return platform::playerInput;
}

// IMPLEMENTATION

//////////////////////////////////////////////////////////////////////////////
// DO NOT ADD YOUR CODE IN THIS FILE!
//////////////////////////////////////////////////////////////////////////////

namespace engine
{
	static const int FontFirstCharacter      = 32;
	static const int FontLineCount           = 16;
	static const int FontLastCharacter       = 126;
	static const int FontCharacterCount      = FontLastCharacter - FontFirstCharacter + 1;

	static const int Scale1x                 = 1;
	static const int Scale4x                 = 4;

	static const Pixel WhiteColor            = 0xffffffu;
	static const Pixel YellowColor           = 0xffff00u;
	static const Pixel RedColor              = 0xff0000u;
	static const Pixel GreenColor            = 0x00ff00u;

	static const int SpriteMaskBitCount      = 8;
	static const int SpriteLineCount         = 8;

	static const int CanvasByteCountPerPixel = sizeof(Pixel);
	static const int CanvasByteCount         = CanvasByteCountPerPixel * Engine::CanvasWidth * Engine::CanvasHeight;

	static const uint8_t sprites_[(int)(Engine::Sprite::Count)][SpriteLineCount] =
	{
		{0x00, 0x00, 0x10, 0x7c, 0xfe, 0xfe, 0x00, 0x00}, // Player
		{0x00, 0x28, 0x38, 0x38, 0x10, 0x10, 0x00, 0x00}, // Bomb
		{0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00}, // Rocket
		{0x00, 0x00, 0x38, 0x54, 0x7c, 0x28, 0x44, 0x00}, // Enemy1
		{0x00, 0x00, 0x38, 0x54, 0xba, 0x28, 0x10, 0x00}, // Enemy2
	};

	static const Pixel spriteColors_[(int)(Engine::Sprite::Count)] =
	{
		WhiteColor,  // Player
		YellowColor, // Bomb
		WhiteColor,  // Rocket
		RedColor,    // Enemy1
		GreenColor,  // Enemy2
	};

	// Subset of Public Domain font 'unscii-8-tall' from http://pelulamu.net/unscii/
	static const uint8_t fontSprites_[FontCharacterCount][FontLineCount] =
	{
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 32
		{0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00}, // 33
		{0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 34
		{0x36, 0x36, 0x36, 0x36, 0x7f, 0x7f, 0x36, 0x36, 0x7f, 0x7f, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00}, // 35
		{0x18, 0x18, 0x7c, 0x7c, 0x06, 0x06, 0x3c, 0x3c, 0x60, 0x60, 0x3e, 0x3e, 0x18, 0x18, 0x00, 0x00}, // 36
		{0x00, 0x00, 0x63, 0x63, 0x33, 0x33, 0x18, 0x18, 0x0c, 0x0c, 0x66, 0x66, 0x63, 0x63, 0x00, 0x00}, // 37
		{0x1c, 0x1c, 0x36, 0x36, 0x1c, 0x1c, 0x6e, 0x6e, 0x3b, 0x3b, 0x33, 0x33, 0x6e, 0x6e, 0x00, 0x00}, // 38
		{0x18, 0x18, 0x18, 0x18, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 39
		{0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x00, 0x00}, // 40
		{0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x00, 0x00}, // 41
		{0x00, 0x00, 0x66, 0x66, 0x3c, 0x3c, 0xff, 0xff, 0x3c, 0x3c, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00}, // 42
		{0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00}, // 43
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x0c, 0x0c}, // 44
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 45
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00}, // 46
		{0xc0, 0xc0, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x03, 0x03, 0x00, 0x00}, // 47
		{0x3c, 0x3c, 0x66, 0x66, 0x76, 0x76, 0x6e, 0x6e, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 48
		{0x18, 0x18, 0x1c, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x7e, 0x00, 0x00}, // 49
		{0x3c, 0x3c, 0x66, 0x66, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x7e, 0x7e, 0x00, 0x00}, // 50
		{0x3c, 0x3c, 0x66, 0x66, 0x60, 0x60, 0x38, 0x38, 0x60, 0x60, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 51
		{0x38, 0x38, 0x3c, 0x3c, 0x36, 0x36, 0x33, 0x33, 0x7f, 0x7f, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00}, // 52
		{0x7e, 0x7e, 0x06, 0x06, 0x3e, 0x3e, 0x60, 0x60, 0x60, 0x60, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 53
		{0x38, 0x38, 0x0c, 0x0c, 0x06, 0x06, 0x3e, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 54
		{0x7e, 0x7e, 0x60, 0x60, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00}, // 55
		{0x3c, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 56
		{0x3c, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x7c, 0x60, 0x60, 0x30, 0x30, 0x1c, 0x1c, 0x00, 0x00}, // 57
		{0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00}, // 58
		{0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x0c, 0x0c}, // 59
		{0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x00, 0x00}, // 60
		{0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 61
		{0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x00, 0x00}, // 62
		{0x3c, 0x3c, 0x66, 0x66, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00}, // 63
		{0x3e, 0x3e, 0x63, 0x63, 0x7b, 0x7b, 0x7b, 0x7b, 0x7b, 0x7b, 0x03, 0x03, 0x3e, 0x3e, 0x00, 0x00}, // 64
		{0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00}, // 65
		{0x3e, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x3e, 0x00, 0x00}, // 66
		{0x3c, 0x3c, 0x66, 0x66, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 67
		{0x1e, 0x1e, 0x36, 0x36, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x36, 0x36, 0x1e, 0x1e, 0x00, 0x00}, // 68
		{0x7e, 0x7e, 0x06, 0x06, 0x06, 0x06, 0x3e, 0x3e, 0x06, 0x06, 0x06, 0x06, 0x7e, 0x7e, 0x00, 0x00}, // 69
		{0x7e, 0x7e, 0x06, 0x06, 0x06, 0x06, 0x3e, 0x3e, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00}, // 70
		{0x3c, 0x3c, 0x66, 0x66, 0x06, 0x06, 0x76, 0x76, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x7c, 0x00, 0x00}, // 71
		{0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00}, // 72
		{0x7e, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x7e, 0x00, 0x00}, // 73
		{0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 74
		{0x63, 0x63, 0x33, 0x33, 0x1b, 0x1b, 0x0f, 0x0f, 0x1b, 0x1b, 0x33, 0x33, 0x63, 0x63, 0x00, 0x00}, // 75
		{0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x7e, 0x7e, 0x00, 0x00}, // 76
		{0x63, 0x63, 0x77, 0x77, 0x7f, 0x7f, 0x6b, 0x6b, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x00, 0x00}, // 77
		{0x63, 0x63, 0x67, 0x67, 0x6f, 0x6f, 0x7b, 0x7b, 0x73, 0x73, 0x63, 0x63, 0x63, 0x63, 0x00, 0x00}, // 78
		{0x3c, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 79
		{0x3e, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x3e, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00}, // 80
		{0x3c, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x36, 0x36, 0x6c, 0x6c, 0x00, 0x00}, // 81
		{0x3e, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x3e, 0x36, 0x36, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00}, // 82
		{0x3c, 0x3c, 0x66, 0x66, 0x06, 0x06, 0x3c, 0x3c, 0x60, 0x60, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 83
		{0x7e, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00}, // 84
		{0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 85
		{0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x00}, // 86
		{0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x6b, 0x6b, 0x7f, 0x7f, 0x77, 0x77, 0x63, 0x63, 0x00, 0x00}, // 87
		{0xc3, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x00, 0x00}, // 88
		{0xc3, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00}, // 89
		{0x7e, 0x7e, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x7e, 0x7e, 0x00, 0x00}, // 90
		{0x3c, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x3c, 0x00, 0x00}, // 91
		{0x03, 0x03, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60, 0xc0, 0xc0, 0x00, 0x00}, // 92
		{0x3c, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x3c, 0x00, 0x00}, // 93
		{0x08, 0x08, 0x1c, 0x1c, 0x36, 0x36, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 94
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff}, // 95
		{0x18, 0x18, 0x30, 0x30, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 96
		{0x00, 0x00, 0x00, 0x00, 0x3c, 0x3c, 0x60, 0x60, 0x7c, 0x7c, 0x66, 0x66, 0x7c, 0x7c, 0x00, 0x00}, // 97
		{0x06, 0x06, 0x06, 0x06, 0x3e, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x3e, 0x00, 0x00}, // 98
		{0x00, 0x00, 0x00, 0x00, 0x3c, 0x3c, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x3c, 0x3c, 0x00, 0x00}, // 99
		{0x60, 0x60, 0x60, 0x60, 0x7c, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x7c, 0x00, 0x00}, // 100
		{0x00, 0x00, 0x00, 0x00, 0x3c, 0x3c, 0x66, 0x66, 0x7e, 0x7e, 0x06, 0x06, 0x3c, 0x3c, 0x00, 0x00}, // 101
		{0x38, 0x38, 0x0c, 0x0c, 0x3e, 0x3e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00}, // 102
		{0x00, 0x00, 0x00, 0x00, 0x7c, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x7c, 0x60, 0x60, 0x3e, 0x3e}, // 103
		{0x06, 0x06, 0x06, 0x06, 0x3e, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00}, // 104
		{0x18, 0x18, 0x00, 0x00, 0x1c, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x78, 0x00, 0x00}, // 105
		{0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x1e, 0x1e}, // 106
		{0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x36, 0x36, 0x1e, 0x1e, 0x36, 0x36, 0x66, 0x66, 0x00, 0x00}, // 107
		{0x1c, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x78, 0x00, 0x00}, // 108
		{0x00, 0x00, 0x00, 0x00, 0x33, 0x33, 0x7f, 0x7f, 0x6b, 0x6b, 0x6b, 0x6b, 0x63, 0x63, 0x00, 0x00}, // 109
		{0x00, 0x00, 0x00, 0x00, 0x3e, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00}, // 110
		{0x00, 0x00, 0x00, 0x00, 0x3c, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x00, 0x00}, // 111
		{0x00, 0x00, 0x00, 0x00, 0x3e, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x3e, 0x06, 0x06, 0x06, 0x06}, // 112
		{0x00, 0x00, 0x00, 0x00, 0x7c, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x7c, 0x60, 0x60, 0x60, 0x60}, // 113
		{0x00, 0x00, 0x00, 0x00, 0x3e, 0x3e, 0x66, 0x66, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00}, // 114
		{0x00, 0x00, 0x00, 0x00, 0x7c, 0x7c, 0x06, 0x06, 0x3c, 0x3c, 0x60, 0x60, 0x3e, 0x3e, 0x00, 0x00}, // 115
		{0x0c, 0x0c, 0x0c, 0x0c, 0x7e, 0x7e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x78, 0x78, 0x00, 0x00}, // 116
		{0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x7c, 0x00, 0x00}, // 117
		{0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x00}, // 118
		{0x00, 0x00, 0x00, 0x00, 0x63, 0x63, 0x63, 0x63, 0x6b, 0x6b, 0x3e, 0x3e, 0x36, 0x36, 0x00, 0x00}, // 119
		{0x00, 0x00, 0x00, 0x00, 0x63, 0x63, 0x36, 0x36, 0x1c, 0x1c, 0x36, 0x36, 0x63, 0x63, 0x00, 0x00}, // 120
		{0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x7c, 0x60, 0x60, 0x3c, 0x3c}, // 121
		{0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x7e, 0x7e, 0x00, 0x00}, // 122
		{0x70, 0x70, 0x18, 0x18, 0x18, 0x18, 0x0e, 0x0e, 0x18, 0x18, 0x18, 0x18, 0x70, 0x70, 0x00, 0x00}, // 123
		{0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00}, // 124
		{0x0e, 0x0e, 0x18, 0x18, 0x18, 0x18, 0x70, 0x70, 0x18, 0x18, 0x18, 0x18, 0x0e, 0x0e, 0x00, 0x00}, // 125
		{0x6e, 0x6e, 0x3b, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 126
	};

	void init()
	{
		canvas = (Pixel*)calloc(1, CanvasByteCount);
	}

	void deinit()
	{
		free(canvas);
	}

	void clearCanvas()
	{
		memset(canvas, 0, CanvasByteCount);
	}

	struct Range
	{
		int begin, end;
	};

	Range clip1d(int v, int width, int clip)
	{
		Range result;
		result.begin = (v < 0) ? -v : 0;
		result.end = (v + width > clip) ? clip - v : width;
		return result;
	}

	// Copy sprite defined by a bit mask to canvas starting at top-left (x, y)
	// Each bit expands to scale X scale pixels set to color.
	void copySpriteToCanvas(const uint8_t* masks, int maskCount, int scale, int x, int y, Pixel color)
	{
		// Clip to canvas
		Range sxrange = clip1d(x, SpriteMaskBitCount * scale, Engine::CanvasWidth);
		Range syrange = clip1d(y, maskCount * scale, Engine::CanvasHeight);

		for (int sy = syrange.begin; sy < syrange.end; ++sy)
		{
			int desty = y + sy;
			uint8_t mask = masks[sy/scale];
			if (!mask) continue;
			for (int sx = sxrange.begin; sx < sxrange.end; ++sx)
			{
				int destx = x + sx;
				if (mask & (0x1u << (sx/scale)))
				{
					canvas[desty * Engine::CanvasWidth + destx] = color;
				}
			}
		}
	}

	void drawSprite(Engine::Sprite sprite, int x, int y)
	{
		const Pixel color = spriteColors_[(int)sprite];
		const uint8_t* masks = sprites_[(int)sprite];
		copySpriteToCanvas(masks, SpriteLineCount, Scale4x, x, y, color);
	}

	void drawText(const char* msg, int x, int y)
	{
		int cx = x;
		int cy = y;
		for(;;)
		{
			char c = *msg++;
			if (!c) break;
			if (c == '\n')
			{
				cx = x;
				cy = cy + Engine::FontRowHeight;
			}
			else if (c >= FontFirstCharacter && c <= FontLastCharacter)
			{
				c = c - FontFirstCharacter;
				const uint8_t* masks = fontSprites_[c];
				copySpriteToCanvas(masks, FontLineCount, Scale1x, cx, cy, WhiteColor);
				cx += Engine::FontWidth;
			}
			else
			{
				cx += Engine::FontWidth;
			}
		}
	}
}

// WIN32 PLATFORM

//////////////////////////////////////////////////////////////////////////////
// DO NOT ADD YOUR CODE IN THIS FILE!
//////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#include <windows.h>
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

namespace platform
{
	static HWND gameWindow_;
	static engine::Pixel* canvas_;
	static LARGE_INTEGER performanceCounterStart_;
	static LARGE_INTEGER performanceCounterFrequency_;

	static LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = 0;

		switch (message)
		{
		case WM_PAINT: {
			PAINTSTRUCT ps;
			const int WindowWidth = Engine::CanvasWidth;
			const int WindowHeight = Engine::CanvasHeight;
			HDC dc = BeginPaint(window, &ps);
			if (canvas_)
			{
				BITMAPINFO bi{};
				bi.bmiHeader.biSize = sizeof(bi);
				bi.bmiHeader.biWidth = (LONG)Engine::CanvasWidth;
				bi.bmiHeader.biHeight = -(LONG)Engine::CanvasHeight;
				bi.bmiHeader.biPlanes = 1;
				bi.bmiHeader.biBitCount = 32;
				bi.bmiHeader.biCompression = BI_RGB;

				StretchDIBits(dc,
					0, 0, WindowWidth, WindowHeight,
					0, 0, Engine::CanvasWidth, Engine::CanvasHeight,
					canvas_, &bi, DIB_RGB_COLORS, SRCCOPY);
			}
			else
			{
				RECT rect {0, 0, WindowWidth, WindowHeight};
				HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
				FillRect(dc, &rect, black);
				DeleteObject(black);
			}
			EndPaint(window, &ps);
		} break;

		case WM_KEYDOWN: {
			// Quit when pressing escape
			if ((wParam & 0xFF) == 27)
				PostQuitMessage(0);
		} break;

		case WM_CLOSE: {
			PostQuitMessage(0);
		} break;

		default: {
			result = DefWindowProc(window, message, wParam, lParam);
		}
		}

		return result;
	}

	void init()
	{
		int width = Engine::CanvasWidth;
		int height = Engine::CanvasHeight;

		const char* title = "Dice Invaders";

		RECT rect = { 0 };

		WNDCLASSA wndclass{};
		wndclass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.hCursor = LoadCursor(0, IDC_ARROW);
		wndclass.lpszClassName = title;
		RegisterClassA(&wndclass);

		rect.right = width;
		rect.bottom = height;

		AdjustWindowRect(&rect, WS_POPUP | WS_SYSMENU | WS_CAPTION, 0);

		rect.right -= rect.left;
		rect.bottom -= rect.top;

		gameWindow_ = CreateWindowExA(0,
			title, title,
			WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right, rect.bottom,
			0, 0, 0, 0);

		if (gameWindow_)
			ShowWindow(gameWindow_, SW_NORMAL);

		QueryPerformanceFrequency(&performanceCounterFrequency_);
		QueryPerformanceCounter(&performanceCounterStart_);
	}

	void deinit()
	{
		if (gameWindow_) DestroyWindow(gameWindow_);
	}

	bool update(engine::Pixel* canvas)
	{
		if (!gameWindow_ || !canvas)
			return false;

		// Copy canvas to window and pump messages
		{
			canvas_ = canvas;

			InvalidateRect(gameWindow_, NULL, FALSE);

			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT)
				{
					canvas_ = nullptr;
					return false;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			canvas_ = nullptr;
		}

		// Sample the player input keys
		{
			playerInput.left = !!GetAsyncKeyState(VK_LEFT);
			playerInput.right = !!GetAsyncKeyState(VK_RIGHT);
			playerInput.fire = !!GetAsyncKeyState(VK_SPACE);
		}

		return true;
	}

	double getStopwatchElapsedSeconds()
	{
		LARGE_INTEGER performanceCounter;
		QueryPerformanceCounter(&performanceCounter);
		int64_t elapsed = performanceCounter.QuadPart - performanceCounterStart_.QuadPart;
		return ((double)elapsed/(double)performanceCounterFrequency_.QuadPart);
	}
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	extern void EngineMain();
	EngineMain();
	return 0;
}

#else

#include <SDL2/SDL.h>
#include <time.h>

namespace platform
{
	SDL_Window* gameWindow_;

	void init()
	{
		gameWindow_ = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Engine::CanvasWidth, Engine::CanvasHeight, SDL_WINDOW_SHOWN);
	}

	void deinit()
	{
		SDL_DestroyWindow(gameWindow_);
		SDL_Quit();
	}

	bool update(engine::Pixel* canvas)
	{
		if (!gameWindow_ || !canvas)
			return false;

		SDL_Rect rect = {0, 0, Engine::CanvasWidth, Engine::CanvasHeight};
		SDL_Surface* canvasSurface = SDL_CreateRGBSurfaceFrom(canvas, rect.w, rect.h, 32, rect.w * 4, 0xff0000, 0xff00,  0xff, 0);

		if (!canvasSurface)
		{
			SDL_FreeSurface(canvasSurface);
			return false;
		}

		SDL_Surface* screenSurface = SDL_GetWindowSurface(gameWindow_);
		SDL_BlitSurface(canvasSurface, 0, screenSurface, 0);
		SDL_UpdateWindowSurface(gameWindow_);

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				deinit();
				return false;
			}
		}

		const Uint8* keyboard = SDL_GetKeyboardState(0);

		if (keyboard[SDL_SCANCODE_ESCAPE])
		{
			deinit();
			return false;
		}

		playerInput.left  = keyboard[SDL_SCANCODE_LEFT];
		playerInput.right = keyboard[SDL_SCANCODE_RIGHT];
		playerInput.fire  = keyboard[SDL_SCANCODE_SPACE];

		return true;
	}

	double getStopwatchElapsedSeconds()
	{
		struct timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);
		return (double)(t.tv_sec) + (double)(t.tv_nsec) * 1e-9;
	}
}

int main()
{
	extern void EngineMain();
	EngineMain();
	return 0;
}

#endif //_WIN32

