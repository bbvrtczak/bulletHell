#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

#include "drawing_func.h"
#include "game_func.h"
#include "structs.h"

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}


// display
#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define SURFACES_COUNT 24

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


// main
#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char **argv) {

	// textures
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface *screen, *map, *menu, *menuOptions, *gameOver, *win, *charset, *icon;
	SDL_Surface *currentPlayer, *player_left, *player_right, *player_up, *player_down, *enemy, *enemyDamage;
	SDL_Surface *bullet, *playerBullet, *immortalityFrame, *damage;
	SDL_Surface *health_1, * health_2, * health_3, * health_4, * health_5, * health_full;
	SDL_Surface *bullets[MAX_BULLETS], *health[HEALTH_BARS], *surfaces[SURFACES_COUNT], *playerBullets[MAX_BULLETS];


	// loading bitmaps
	charset = SDL_LoadBMP("./cs8x8.bmp");
	surfaces[0] = charset;

	map = SDL_LoadBMP("./map.bmp");
	surfaces[1] = map;

	player_left = SDL_LoadBMP("./player_left.bmp");
	surfaces[3] = player_left;

	player_right = SDL_LoadBMP("./player_right.bmp");
	surfaces[4] = player_right;

	player_up = SDL_LoadBMP("./player_up.bmp");
	surfaces[5] = player_up;

	player_down = SDL_LoadBMP("./player_down.bmp");
	surfaces[6] = player_down;

	enemy = SDL_LoadBMP("./enemy.bmp");
	surfaces[7] = enemy;

	bullet = SDL_LoadBMP("./bullet.bmp");
	surfaces[8] = bullet;
	SDL_SetColorKey(bullet, true, 0x777777);

	health[FULL] = SDL_LoadBMP("./health_bar_full.bmp");
	surfaces[9] = health[FULL];

	health[5] = SDL_LoadBMP("./health_bar_5.bmp");
	surfaces[10] = health[5];

	health[4] = SDL_LoadBMP("./health_bar_4.bmp");
	surfaces[11] = health[4];

	health[3] = SDL_LoadBMP("./health_bar_3.bmp");
	surfaces[12] = health[3];

	health[2] = SDL_LoadBMP("./health_bar_2.bmp");
	surfaces[13] = health[2];

	health[1] = SDL_LoadBMP("./health_bar_1.bmp");
	surfaces[14] = health[1];

	health[0] = SDL_LoadBMP("./health_bar_0.bmp");
	surfaces[15] = health[0];
	
	menu = SDL_LoadBMP("./menu.bmp");
	surfaces[16] = menu;

	menuOptions = SDL_LoadBMP("./menu_options.bmp");
	SDL_SetColorKey(menuOptions, true, 0xFFFFFFFF);
	surfaces[17] = menuOptions;

	gameOver = SDL_LoadBMP("./game_over.bmp");
	SDL_SetColorKey(gameOver, true, 0xFFFFFFFF);
	surfaces[18] = gameOver;

	playerBullet = SDL_LoadBMP("./bullet_player.bmp");
	SDL_SetColorKey(playerBullet, true, 0xFF777777);
	surfaces[19] = playerBullet;

	icon = SDL_LoadBMP("./icon.bmp");
	SDL_SetColorKey(playerBullet, true, 0xFFFFFFFF);
	surfaces[20] = icon;

	immortalityFrame = SDL_LoadBMP("./immortality_frame.bmp");
	SDL_SetColorKey(immortalityFrame, true, 0xFFFFFFFF);
	surfaces[21] = immortalityFrame;

	win = SDL_LoadBMP("./win.bmp");
	SDL_SetColorKey(win, true, 0xFFFFFFFF);
	surfaces[22] = win;

	enemyDamage = SDL_LoadBMP("./enemy_damage.bmp");
	surfaces[23] = enemyDamage;


	SDL_SetColorKey(charset, true, 0x000000);
	SetTransparency(health);


	// game variables
	int t1, t2, quit, frames, rc;
	double delta, worldTime, fpsTimer, fps;
	long long int bulletTick = 0;

		// config
	SDL_Event event;
	Point MapPosition{ 1050,850 };
	Bullet bulletsInfo[MAX_BULLETS];
	int bulletCount = NULL;
	int view = MENU;
	int action = NONE;
	const Uint8* state = SDL_GetKeyboardState(NULL);

	ScoreInfo Score{
		NULL,
		1,
		NULL,
		NULL
	};

	PlayerInfo Player{
		{0,0},
		{},
		playerBullet,
		player_down,
		SCREEN_HEIGHT/2 -25,
		SCREEN_WIDTH/2 - 20,
		SCREEN_WIDTH/2 + 20,
		SCREEN_HEIGHT/2 + 25,
		DOWN,
		DOWN,
		NULL,
		FULL,
		0
	};

	EnemyInfo Enemy{
		6,
		0,
		MapPosition.y + VIEW_Y - 40,
		MapPosition.x + VIEW_X - 40,
		MapPosition.x + VIEW_X + 40,
		MapPosition.y + VIEW_Y + 40
	};

	// SDL things
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

		// fullscreen mode
	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
	//rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Bullet Hell");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_ShowCursor(SDL_DISABLE);

	CheckSurfaces(*surfaces, screen, scrtex, window, renderer, 0);
	SDL_SetWindowIcon(window, icon);


	//colors
	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int bialy = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int szary = SDL_MapRGB(screen->format, 0x50, 0x50, 0x50);
	int ciemnoszary = SDL_MapRGB(screen->format, 0x25, 0x25, 0x25);


	// initializing game
	InitNewGame(&frames, &fpsTimer, &fps, &quit, &worldTime, &MapPosition, &Player, &bulletCount, &Score, &Enemy);
	AssignBulletsInfo(&Player, bulletsInfo);
		// loading bullets
	for (int i = 0; i < MAX_BULLETS; i++) {
		bullets[i] = bullet;
	}

	t1 = SDL_GetTicks();


	// gameloop
	while(!quit) {
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;
		worldTime += delta;


		// menu
		if (view == MENU) {

			DrawSurface(screen, menu, 450, 300);
			DrawSurface(screen, menuOptions, 450, 300);

			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_n) {
						InitNewGame(&frames, &fpsTimer, &fps, &quit, &worldTime, &MapPosition, &Player, &bulletCount, &Score, &Enemy);
						bulletTick = 0;
						view = GAME;
					}
					break;
				}
			}
		}


		// game over or win
		if (view == GAME_OVER || view == WIN) {

			SDL_FillRect(screen, NULL, ciemnoszary);
			DrawSurface(screen, menu, 450, 300);

			if (view == GAME_OVER)
				DrawSurface(screen, gameOver, 450, 300);
			else {
				DrawSurface(screen, win, 450, 300);
				CheckHighScore(&Score);
			}

			sprintf(text, "Score: %d", Score.CurrentScore);
			DrawString(screen, SCREEN_WIDTH/2 - strlen(text) * 8 / 2, SCREEN_HEIGHT/2 - 30, text, charset);

			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_n) {
						InitNewGame(&frames, &fpsTimer, &fps, &quit, &worldTime, &MapPosition, &Player, &bulletCount, &Score, &Enemy);
						view = GAME;
					}
					break;
				}
			}
		}


		// game
		if (view == GAME) {
			bulletTick++;


			// drawing map and player
			SDL_FillRect(screen, NULL, ciemnoszary);
			DrawSurface(screen, map, MapPosition.x, MapPosition.y);
			DrawSurface(screen, Player.Texture, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);


			// enemy bullets initialization
			if (bulletTick % 40 == 0 && bulletCount < MAX_BULLETS) {
				RenderEnemyBullets(bullets, MapPosition, bulletsInfo, &bulletCount, screen);
				bulletCount += BULLETS_DIRECTIONS_COUNT;
			}


			// moving enemy hitbox
			MoveEnemyHitbox(&Enemy, MapPosition);


			// fps timer
			CountFPS(&fpsTimer, &frames, &fps, delta);


			// logic

				// player movement
			Move(state, &MapPosition, &Player, player_up, player_left, player_right, player_down);

				// game ending condition
			if (Player.Health == 0) {
				view = GAME_OVER;
			}

				// player shooting
			if (action == SHOOT) {
				PlayerShoot(&Player, MapPosition);
				action = NONE;
			}
			MovePlayerBullets(&Player, screen);
			CheckAndResetPlayerBullets(&Player);

				// enemy shooting
			for (int i = 0; i < bulletCount; i += BULLETS_DIRECTIONS_COUNT) {
				RenderEnemyBullets(bullets, MapPosition, bulletsInfo, &i, screen);

				// checking map borders and moving bullets
				CheckEnemyBulletsBorders(bulletsInfo, i);
				MoveEnemyBullets(bulletsInfo, i);
				CheckCollision(Player.Hitbox, bulletsInfo[i].Hitbox);
			}

				// resetting enemy bullets
			if (bulletTick % 1600 == 0) {
				ResetBullets(bulletsInfo, &bulletCount);
			}

				// hitbox collision
					
					// enemy shooting
			for (int i = 0; i < bulletCount; i++) {
				if (CheckCollision(Player.Hitbox, bulletsInfo[i].Hitbox) && Player.ImmortalityTime == 0) {
					PlayerHit(&Player, bulletsInfo, &Score, i);
					
				}
			}

			if (Player.ImmortalityTime > 0) {
				Player.ImmortalityTime -= 1;
				DrawSurface(screen, immortalityFrame, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			}

					// player shooting
			for (int i = 0; i < Player.BulletsFired; i++)
			{
				if (CheckCollision(Player.BulletsInfo[i].Hitbox, Enemy.Hitbox)) {
					EnemyHit(&Player, &Score, &Enemy, i);
					Enemy.DamageAnimationTime = 20;
					if (Enemy.Health == 0)
						view = WIN;
				}
			}

			if (Enemy.DamageAnimationTime > 0)
				Enemy.DamageAnimationTime--;

			// drawing enemy
			DrawSurface(screen, health[Enemy.Health], MapPosition.x + VIEW_X, MapPosition.y + VIEW_Y - 70);
			if (Enemy.DamageAnimationTime>0)
				DrawSurface(screen, enemyDamage, MapPosition.x + VIEW_X, MapPosition.y + VIEW_Y);
			else
				DrawSurface(screen, enemy, MapPosition.x + VIEW_X, MapPosition.y + VIEW_Y);

			
			// HUD
			DrawSurface(screen, health[Player.Health], 80, 40);

			sprintf(text, "%.1lfs  %.0lf fps", worldTime, fps);
			DrawString(screen, screen->w - 60 - strlen(text) * 8 / 2, 25, text, charset);

			sprintf(text, "Score: %d", Score.CurrentScore );
			DrawString(screen, screen->w - 60 - strlen(text) * 8 / 2, 45, text, charset);

			sprintf(text, "High score: %d", Score.HighScore);
			DrawString(screen, screen->w - 60 - strlen(text) * 8 / 2, 65, text, charset);

				// score message
			if (Score.ShowTime > 0) {
				sprintf(text, "+ %d", Score.Adding);
				DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - 40, text, charset);
				Score.ShowTime--;
			}

				// "animated" immortality message
			if (Player.ImmortalityTime > 0) {
				sprintf(text, "IMMORTAL");
				if (Player.ImmortalityTime%90>45)
					DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - 55, text, charset);
				else
					DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - 60, text, charset);
				Score.ShowTime--;
			}


			// handling events
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_n) {
						InitNewGame(&frames, &fpsTimer, &fps, &quit, &worldTime, &MapPosition, &Player, &bulletCount, &Score, &Enemy);
						ResetBullets(bulletsInfo, &bulletCount);
						ResetPlayerBullets(&Player);
					}
					else if (event.key.keysym.sym == SDLK_p) {
						view = MENU;
					}
					else if (event.key.keysym.sym == SDLK_SPACE) {
						action = SHOOT;
					}
					break;
				case SDL_KEYUP:
					Player.Direction = NOT_DEFINED;
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};


			// updating textures
			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

			// fps
			frames++;
			SDL_Delay(SPEED);
		}
	};

	// freeing all surfaces
	SDL_FreeSurface(charset);
	CheckSurfaces(*surfaces, screen, scrtex, window, renderer, 1);

	// quitting
	SDL_Quit();
	return 0;
	};