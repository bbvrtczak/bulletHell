#pragma once
#include "structs.h"

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
#define MISSED 0
#define HIT 1

// health
#define HEALTH_BARS 7
#define FULL 6

// game view
#define MENU 0
#define GAME 1
#define GAME_OVER 2

// actions
#define NONE 0
#define SHOOT 1

void InitNewGame(int* frames, double* fpsTimer, double* fps, int* quit, double* worldTime, Point* MapPosition, PlayerInfo* Player, int* bulletCount, ScoreInfo* Score, EnemyInfo* Enemy) {
	*frames = 0;
	*fpsTimer = 0;
	*fps = 0;
	*quit = 0;
	*worldTime = 0;
	MapPosition->x = 1050;
	MapPosition->y = 850;
	Player->Position.x = 0;
	Player->Position.y = 0;
	Player->BulletsFired = 0;
	Player->Health = FULL;
	Player->ImmortalityTime = 0;
	for (int i = 0; i < MAX_BULLETS; i++) {
		Player->BulletsInfo[i].x = 0;
		Player->BulletsInfo[i].y = 0;
	}
	*bulletCount = 0;
	Score->CurrentScore = 0;
	Enemy->Health = 6;
	Score->Adding = 1;
};

void InitBullets(Bullet bullets[], Point PlayerPosition) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		bullets[i].x = PlayerPosition.x + 420;
		bullets[i].y = PlayerPosition.y - 150;
	}
};

void ResetBullets(Bullet bulletsInfo[], int* bulletCount) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		bulletsInfo[i].x = 0;
		bulletsInfo[i].y = 0;
		bulletsInfo[i].status = 0;
		bulletsInfo[i].direction = 0;
	}
	*bulletCount = 0;
};

void ResetPlayerBullets(PlayerInfo *Player) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		Player->BulletsInfo[i].x = 0;
		Player->BulletsInfo[i].y = 0;
		Player->BulletsInfo[i].status = 0;
		Player->BulletsInfo[i].direction = 0;
	}
	Player->BulletsFired= 0;
};

void AssignBulletsInfo (PlayerInfo *Player, Bullet bulletsInfo[]){
	for (int i = 0; i < MAX_BULLETS; i++) {
		Player->BulletsInfo[i].x = 0;
		Player->BulletsInfo[i].y = 0;
	}

	for (int i = 0; i < MAX_BULLETS; i++) {
		bulletsInfo[i].x = 0;
		bulletsInfo[i].y = 0;
	}
}

void RenderEnemyBullets(SDL_Surface **bullets, Point PlayerPosition, Bullet bulletsInfo[], int *bulletCount, SDL_Surface *screen){
	DrawSurface(screen, bullets[*bulletCount], PlayerPosition.x + bulletsInfo[*bulletCount].x + VIEW_X, PlayerPosition.y + VIEW_Y);
	DrawSurface(screen, bullets[*bulletCount + 1], PlayerPosition.x + VIEW_X, PlayerPosition.y + bulletsInfo[*bulletCount + 1].y + VIEW_Y);
	DrawSurface(screen, bullets[*bulletCount + 2], PlayerPosition.x + VIEW_X, PlayerPosition.y + bulletsInfo[*bulletCount + 2].y + VIEW_Y);
	DrawSurface(screen, bullets[*bulletCount + 3], PlayerPosition.x + bulletsInfo[*bulletCount + 3].x + VIEW_X, PlayerPosition.y + VIEW_Y);
	for (int j = 4; j < BULLETS_DIRECTIONS_COUNT; j++) {
		DrawSurface(screen, bullets[*bulletCount + j], PlayerPosition.x + bulletsInfo[*bulletCount + j].x + VIEW_X, PlayerPosition.y + bulletsInfo[*bulletCount + j].y + VIEW_Y);
	}
	for (int k = 0; k < BULLETS_DIRECTIONS_COUNT; k++) {
		bulletsInfo[*bulletCount + k].Hitbox.Up = PlayerPosition.y + bulletsInfo[*bulletCount + k].y + VIEW_Y - 10;
		bulletsInfo[*bulletCount + k].Hitbox.Down = PlayerPosition.y + bulletsInfo[*bulletCount + k].y + VIEW_Y + 10;
		bulletsInfo[*bulletCount + k].Hitbox.Left = PlayerPosition.x + bulletsInfo[*bulletCount + k].x + VIEW_X - 10;
		bulletsInfo[*bulletCount + k].Hitbox.Right = PlayerPosition.x + bulletsInfo[*bulletCount + k].x + VIEW_X + 10;
	}
}

void PlayerShoot(PlayerInfo *Player, Point PlayerPosition) {
	Player->BulletsInfo[Player->BulletsFired].y = 300 - (PlayerPosition.y - 850);
	Player->BulletsInfo[Player->BulletsFired].x = 520 - (PlayerPosition.x - 520);
	Player->BulletsInfo[Player->BulletsFired].direction = Player->BulletDirection;
	Player->BulletsInfo[Player->BulletsFired].Hitbox.Up = SCREEN_HEIGHT / 2 - 10;
	Player->BulletsInfo[Player->BulletsFired].Hitbox.Left = SCREEN_WIDTH / 2 - 10;
	Player->BulletsInfo[Player->BulletsFired].Hitbox.Right = SCREEN_WIDTH / 2 + 10;
	Player->BulletsInfo[Player->BulletsFired].Hitbox.Down = SCREEN_HEIGHT / 2 + 10;
	Player->BulletsInfo[Player->BulletsFired].ShotCord.x = Player->Position.x;
	Player->BulletsInfo[Player->BulletsFired].ShotCord.y = Player->Position.y;
	Player->BulletsFired += 1;
}

void MovePlayerBullets(PlayerInfo *Player, SDL_Surface *screen){
	for (int i = 0; i < Player->BulletsFired; i++) {
		if (Player->BulletsInfo[i].direction == RIGHT) {
			Player->BulletsInfo[i].x += 2;
			Player->BulletsInfo[i].Hitbox.Left += 2;
			Player->BulletsInfo[i].Hitbox.Right += 2;
		}
		else if (Player->BulletsInfo[i].direction == LEFT) {
			Player->BulletsInfo[i].x -= 2;
			Player->BulletsInfo[i].Hitbox.Left -= 2;
			Player->BulletsInfo[i].Hitbox.Right -= 2;
		}
		else if (Player->BulletsInfo[i].direction == UP) {
			Player->BulletsInfo[i].y -= 2;
			Player->BulletsInfo[i].Hitbox.Up -= 2;
			Player->BulletsInfo[i].Hitbox.Down -= 2;
		}
		else if (Player->BulletsInfo[i].direction == DOWN) {
			Player->BulletsInfo[i].y += 2;
			Player->BulletsInfo[i].Hitbox.Up += 2;
			Player->BulletsInfo[i].Hitbox.Down += 2;
		}
		DrawSurface(screen, Player->BulletTexture, 450 + Player->Position.x + Player->BulletsInfo[i].x, Player->Position.y + Player->BulletsInfo[i].y);
	}
}

void CheckAndResetPlayerBullets(PlayerInfo *Player){
	if (Player->BulletsFired == MAX_BULLETS) {
		ResetPlayerBullets(Player);
	}
}

void MoveEnemyBullets(Bullet bulletsInfo[], int index) {
	bulletsInfo[index].x -= 2;

	bulletsInfo[index + 1].y -= 2;

	bulletsInfo[index + 2].y += 2;

	bulletsInfo[index + 3].x += 2;

	bulletsInfo[index + 4].x -= 1;
	bulletsInfo[index + 4].y += 1;

	bulletsInfo[index + 5].x -= 1;
	bulletsInfo[index + 5].y -= 1;

	bulletsInfo[index + 6].x += 1;
	bulletsInfo[index + 6].y += 1;

	bulletsInfo[index + 7].x += 1;
	bulletsInfo[index + 7].y -= 1;
};

void CheckEnemyBulletsBorders(Bullet bulletsInfo[], int i) {
	if (bulletsInfo[i].x < LEFT_BORDER_LIMIT) { // left
		bulletsInfo[i].x -= 5000;
	}
	if (bulletsInfo[i + 1].y < TOP_BORDER_LIMIT) { // top
		bulletsInfo[i + 1].y -= 5000;
	}
	if (bulletsInfo[i + 5].y < TOP_BORDER_LIMIT) {
		bulletsInfo[i + 5].y -= 5000;
	}
	if (bulletsInfo[i + 2].y > BOTTOM_BORDER_LIMIT) { // bottom
		bulletsInfo[i + 2].y -= 5000;
	}
	if (bulletsInfo[i + 4].y > BOTTOM_BORDER_LIMIT) {
		bulletsInfo[i + 4].y -= 5000;
	}
	if (bulletsInfo[i + 3].x > RIGHT_BORDER_LIMIT) { // right
		bulletsInfo[i + 3].x -= 5000;
	}
	if (bulletsInfo[i + 6].x > RIGHT_BORDER_LIMIT) {
		bulletsInfo[i + 6].x -= 5000;
	}
	if (bulletsInfo[i + 7].x > RIGHT_BORDER_LIMIT) {
		bulletsInfo[i + 7].x -= 5000;
	}
}

bool checkMoveLeft(int x) {
	if (x >= 1177)
		return false;
	else return true;
};

bool checkMoveRight(int x) {
	if (x <= -281)
		return false;
	else return true;
};

bool checkMoveUp(int y) {
	if (y >= 1023)
		return false;
	else return true;
};

bool checkMoveDown(int y) {
	if (y <= -428)
		return false;
	else return true;
};

void Move(const Uint8* state, Point* PlayerPosition, PlayerInfo* Player, SDL_Surface* up, SDL_Surface* left, SDL_Surface* right, SDL_Surface* down) {
	if (state[SDL_SCANCODE_RIGHT]) {
		if (checkMoveRight(PlayerPosition->x)) {
			PlayerPosition->x -= 1;
			Player->Position.x -= 1;
			Player->Direction = RIGHT;
			Player->BulletDirection = RIGHT;
			Player->Texture = right;
		}
	}
	else if (state[SDL_SCANCODE_LEFT]) {
		if (checkMoveLeft(PlayerPosition->x)) {
			PlayerPosition->x += 1;
			Player->Position.x += 1;
			Player->Direction = LEFT;
			Player->BulletDirection = LEFT;
			Player->Texture = left;
		}
	}
	if (state[SDL_SCANCODE_UP]) {
		if (checkMoveUp(PlayerPosition->y)) {
			PlayerPosition->y += 1;
			Player->Position.y += 1;
			Player->Direction = UP;
			Player->BulletDirection = UP;
			Player->Texture = up;
		}
	}
	else if (state[SDL_SCANCODE_DOWN]) {
		if (checkMoveDown(PlayerPosition->y)) {
			PlayerPosition->y -= 1;
			Player->Position.y -= 1;
			Player->Direction = DOWN;
			Player->BulletDirection = DOWN;
			Player->Texture = down;
		}
	}
};

int CheckCollision (Outlines Hitbox1, Outlines Hitbox2){
	if (Hitbox1.Up < Hitbox2.Down && Hitbox1.Down > Hitbox2.Up && Hitbox1.Right > Hitbox2.Left && Hitbox1.Left < Hitbox2.Right)
		return HIT;
	else return MISSED;
}

void CheckHighScore (ScoreInfo *Score){
	if (Score->HighScore < Score->CurrentScore)
		Score->HighScore = Score->CurrentScore;
}

void MoveEnemyHitbox (EnemyInfo *Enemy, Point MapPosition){
	Enemy->Hitbox.Up = MapPosition.y + VIEW_Y - 40;
	Enemy->Hitbox.Left = MapPosition.x + VIEW_X - 40;
	Enemy->Hitbox.Right = MapPosition.x + VIEW_X + 40;
	Enemy->Hitbox.Down = MapPosition.y + VIEW_Y + 40;
}
void PlayerHit (PlayerInfo *Player, Bullet bulletsInfo[], ScoreInfo *Score, int i){
	Player->Health -= 1;
	bulletsInfo[i].x -= 5000;
	bulletsInfo[i].y -= 5000;
	Player->ImmortalityTime += 500;
	Score->CurrentScore--;
	Score->Adding = 1;
}

void EnemyHit (PlayerInfo *Player, ScoreInfo *Score, EnemyInfo *Enemy, int i) {
	Score->CurrentScore += Score->Adding;
	Score->Adding *= 2;
	Enemy->Health-=1;
	Player->BulletsInfo[i].x += 5000;
	Player->BulletsInfo[i].y += 5000;
	Player->BulletsInfo[i].Hitbox.Down += 5000;
	Player->BulletsInfo[i].Hitbox.Left += 5000;
	Player->BulletsInfo[i].Hitbox.Right += 5000;
	Player->BulletsInfo[i].Hitbox.Down += 5000;
	Score->ShowTime = 150;
}

void CountFPS(double* fpsTimer, int* frames, double* fps, double delta) {
	*fpsTimer += delta;
	if (*fpsTimer > 0.5) {
		*fps = *frames * 2;
		*frames = 0;
		*fpsTimer -= 0.5;
	}
}

