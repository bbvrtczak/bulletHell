#pragma once

// display
#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define SURFACES_COUNT 23

// game values
#define VIEW_X 420
#define VIEW_Y -150
#define PLAYER_SIZE 90
#define SPEED 0   // bigger value means slower movement (0 is the minimum value)

// default positions
#define DEFAULT_POS_X SCREEN_WIDTH/2
#define DEFAULT_POS_Y SCREEN_HEIGHT/2

// map borders
#define LEFT_BORDER_LIMIT -1160
#define TOP_BORDER_LIMIT -590
#define BOTTOM_BORDER_LIMIT 890
#define RIGHT_BORDER_LIMIT 325

// game status
#define NOT_STARTED 0
#define STARTED 1

// directions
#define NOT_DEFINED 0
#define UP 1 
#define LEFT 2 
#define RIGHT 3 
#define DOWN 4 

// bullets
#define BULLETS_DIRECTIONS_COUNT 8
#define MAX_BULLETS BULLETS_DIRECTIONS_COUNT*20
#define FIRED 1
#define NOT_FIRED 0

// health
#define HEALTH_BARS 7
#define FULL 6

// game view
#define MENU 0
#define GAME 1
#define GAME_OVER 2
#define WIN 3

// actions
#define NONE 0
#define SHOOT 1


typedef struct GameEngine {
	int frames;
	int fpsTimer;
	int fps;
	int quit;
	int worldTime;
};

typedef struct Point {
	int x;
	int y;
};

typedef struct Outlines {
	int Up;
	int Left;
	int Right;
	int Down;
};

typedef struct Bullet {
	int x;
	int y;
	int status;
	int direction;
	Outlines Hitbox;
	Point ShotCord;
};

typedef struct PlayerInfo {
	Point Position;
	Bullet BulletsInfo[MAX_BULLETS];
	SDL_Surface* BulletTexture;
	SDL_Surface* Texture;
	Outlines Hitbox;
	int Direction;
	int BulletDirection;
	int BulletsFired;
	int Health;
	int ImmortalityTime;
};

typedef struct EnemyInfo {
	int Health;
	int DamageAnimationTime;
	Outlines Hitbox;
	SDL_Surface* HealthBar;
};

typedef struct ScoreInfo {
	int CurrentScore;
	int Adding;
	int HighScore;
	int ShowTime;
};