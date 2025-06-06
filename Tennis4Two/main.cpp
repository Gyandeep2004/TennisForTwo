#include<iostream>
#include<raylib.h>
#include<ctime>
#include<cstdlib>
#include<cmath>
#include<algorithm>
#include<vector>

enum GameScreen { MAIN_MENU, PLAY_MENU, GAMEPLAY, GAMEPLAY_CPU, PAUSE, GAME_OVER, OPTIONS, EXIT };

int lastWindowOpened;
int gameplayType;

int player1_score=0, player2_score=0;

class Ball {
public:
	float x, y;
	int speed_x, speed_y;
	int radius;
	Color ballColor = WHITE;


	void Draw() {
		DrawCircle(x, y, radius, ballColor);
	}
	void reset() {
		x = GetScreenWidth() / 2;
		y = GetScreenHeight() / 2;
		int speed_choices[2] = { -1, 1 };
		speed_x *= speed_choices[GetRandomValue(0, 1)];
		speed_y *= speed_choices[GetRandomValue(0, 1)];
	}
	void Update() {
		x += speed_x;
		y += speed_y;

		//Border
		if (y + radius >= GetScreenHeight() || y - radius <= 0) {
			speed_y *= -1;
		}
		if (x + radius >= GetScreenWidth())
		{
			if (IsKeyDown(KEY_SPACE)) {
				reset();
				player1_score++;
			}
		}
		if (x - radius <= 0)
		{
			if (IsKeyDown(KEY_SPACE)) {
				reset();
				player2_score++;
			}
		}
	}
	
};

class Padle {
protected:
	void LimitMovement() {
		if (y <= 0) {
			y = 0;
		}
		if (y + height >= GetScreenHeight()) {
			y = GetScreenHeight() - height;
		}
	}
public:
	float x, y;
	int speed;
	float height, width;
	Color padleColor = WHITE;

	void Draw() {
		DrawRectangle(x, y, width, height, padleColor);
	}
	void Update1() {
		if (IsKeyDown(KEY_W)) {
			y = y - speed;
		}
		if (IsKeyDown(KEY_S)) {
			y = y + speed;
		}
		LimitMovement();
	}
	void Update2() {
		if (IsKeyDown(KEY_UP)) {
			y = y - speed;
		}
		if (IsKeyDown(KEY_DOWN)) {
			y = y + speed;
		}
		LimitMovement();
	}
};

class cpuPaddle : public Padle {
public:
	void Update(int ball_y) {
		if (y + height / 2 > ball_y) {
			y = y - speed;
		}
		else if(y + height / 2 < ball_y) {
			y = y + speed;
		}
	}
};

Ball ball;
Padle player1;
Padle player2;
cpuPaddle cpu;
Padle testPadle;
Ball testBall;

int main() {
	
	//Game rules
	const int window_height = 800;
	const int window_width = 1280;
	InitWindow(window_width, window_height, "Tennis for Two!");

	Color bgColor = RED;

	ToggleFullscreen();

	bool shouldClose = false;

	SetExitKey(0);
	SetTargetFPS(60);

	InitAudioDevice();
	Sound racketSound = LoadSound("soundeffects/racketsound.wav");
	Sound clickSound = LoadSound("soundeffects/buttonClick.wav");
	Sound winnerSound = LoadSound("soundeffects/winSound.wav");
	Sound loserSound = LoadSound("soundeffects/loserSound.wav");
	Sound gameOverSound = LoadSound("soundeffects/gameOverSound.wav");

	bool winnerSoundPlayed = false;
	bool loserSoundPlayer = false;
	bool gameOverSoundPlayed = false;

	GameScreen currentScreen = MAIN_MENU;

	//Ball Initial Position
	ball.x = GetScreenWidth() / 2;
	ball.y = GetScreenHeight() / 2;
	ball.radius = 20;
	ball.speed_x = 8;
	ball.speed_y = 8;

	//Player1 initial position
	player1.speed = 6;
	player1.height = 120;
	player1.width = 25;
	player1.x = 10;
	player1.y = GetScreenHeight() / 2 - 60;

	//player2 initial position
	player2.speed = 6;
	player2.height = 120;
	player2.width = 25;
	player2.x = GetScreenWidth() - (player2.width + 10);
	player2.y = GetScreenHeight() / 2 - 60;

	//cpu initial position
	cpu.speed = 6;
	cpu.height = 120;
	cpu.width = 25;
	cpu.x = 10;
	cpu.y = GetScreenHeight() / 2 - 60;
	
	//test padle and ball
	testPadle.height = 120;
	testPadle.width = 25;
	testPadle.x = GetScreenWidth() - (testPadle.width + 150);
	testPadle.y = 500;
	testBall.radius = 20;
	testBall.x = 150;
	testBall.y = 560;

	//Main Menu Buttons
	Rectangle playBtn = { window_width / 2 - 100,270,200,50 };
	Rectangle optionsBtn = { window_width / 2 - 100,340,200,50 };
	Rectangle exitBtn = { window_width / 2 - 100,410,200,50 };

	//Play Menu Buttons
	Rectangle co_opBtn = { window_width / 2 - 100,window_height / 2 - 60,200,50 };
	Rectangle multiplayerBtn = { window_width / 2 - 100,window_height / 2 + 10,200,50 };

	//Pause Menu Buttons
	Rectangle continueBtn = { window_width / 2 - 100,200,200,50 };
	Rectangle newGameBtn = { window_width / 2 - 100,270,200,50 };

	//Game Over button
	Rectangle retryBtn = { window_width / 2 - 100,270,200,50 };
	Rectangle menuBtn = { window_width / 2 - 100,340,200,50 };

	//Options menu button
	Rectangle bgColorGreenBtn = { window_width / 2 - 100, 150,200,50 };
	Rectangle bgColorRedBtn = { window_width / 2 - 100,220,200,50 };
	Rectangle bgColorBlueBtn = { window_width / 2 - 100,290,200,50 };
	Rectangle ballColorBlackBtn = { 220,500,200,50 };
	Rectangle ballColorWhiteBtn = { 220,570,200,50 };
	Rectangle padleColorWhiteBtn = { 860,500,200,50 };
	Rectangle padleColorBlackBtn = { 860,570,200,50 };
	
	//Game loop
	while (WindowShouldClose() == false && !shouldClose) {
		Vector2 mouse = GetMousePosition();
		BeginDrawing();
		ClearBackground(bgColor);

		switch (currentScreen) {
		case MAIN_MENU: {
			lastWindowOpened = 0;
			DrawText("tennis4two", window_width / 2 - MeasureText("tennis4two", 60) / 2, 100, 60, WHITE);
			
			// Draw buttons
			DrawRectangleRec(playBtn, CheckCollisionPointRec(mouse, playBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(optionsBtn, CheckCollisionPointRec(mouse, optionsBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(exitBtn, CheckCollisionPointRec(mouse, exitBtn) ? LIGHTGRAY : GRAY);

			DrawText("Start Game", playBtn.x + 40, playBtn.y + 10, 20, BLACK);
			DrawText("Options", optionsBtn.x + 55, optionsBtn.y + 10, 20, BLACK);
			DrawText("Exit", exitBtn.x + 75, exitBtn.y + 10, 20, BLACK);

			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				if (CheckCollisionPointRec(mouse, playBtn)) {
					currentScreen = PLAY_MENU;
					PlaySound(clickSound);
				}
				else if (CheckCollisionPointRec(mouse, optionsBtn)) {
					currentScreen = OPTIONS;
					PlaySound(clickSound);
				}
				else if (CheckCollisionPointRec(mouse, exitBtn)) {
					currentScreen = EXIT;
					PlaySound(clickSound);
					shouldClose = true;
				}
			}
			
		}break;

		case PLAY_MENU: {
			DrawRectangleRec(co_opBtn, CheckCollisionPointRec(mouse, co_opBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(multiplayerBtn, CheckCollisionPointRec(mouse, multiplayerBtn) ? LIGHTGRAY : GRAY);

			DrawText("CPU vs PLAYER", co_opBtn.x + 20, co_opBtn.y + 10, 20, BLACK);
			DrawText("MULTIPLAYER", multiplayerBtn.x + 30, multiplayerBtn.y + 10, 20, BLACK);

			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				if (CheckCollisionPointRec(mouse, co_opBtn)) {
					currentScreen =	GAMEPLAY_CPU;
					PlaySound(clickSound);
					cpu.y = GetScreenHeight() / 2 - 60;
					player2.y = GetScreenHeight() / 2 - 60;
					ball.x = GetScreenWidth() / 2;
					ball.y = GetScreenHeight() / 2;
					player1_score = 0;
					player2_score = 0;
				}
				else if (CheckCollisionPointRec(mouse, multiplayerBtn)) {
					currentScreen = GAMEPLAY;
					PlaySound(clickSound);
					player1.y = GetScreenHeight() / 2 - 60;
					player2.y = GetScreenHeight() / 2 - 60;
					ball.x = GetScreenWidth() / 2;
					ball.y = GetScreenHeight() / 2;
					player1_score = 0;
					player2_score = 0;
				}
			}
			if (IsKeyPressed(KEY_ESCAPE)) {
				if (lastWindowOpened == 0) {
					currentScreen = MAIN_MENU;
				}
				else if (lastWindowOpened == 1) {
					currentScreen = PAUSE;
				}
			}
		}break;
		
		case GAMEPLAY_CPU: {
			gameplayType = 0;
			//Updating
			if (IsKeyPressed) {
				ball.Update();
			}
			player2.Update2();
			cpu.Update(ball.y);

			if (player1_score == 11) {
				currentScreen = GAME_OVER;
			}
			else if (player2_score == 11) {
				currentScreen = GAME_OVER;
			}

			//Check Collison
			if (CheckCollisionCircleRec(Vector2{ ball.x,ball.y }, ball.radius, Rectangle{ player2.x,player2.y,player2.width,player2.height }))
			{
				ball.speed_x *= -1;
				PlaySound(racketSound);
			}
			if (CheckCollisionCircleRec(Vector2{ ball.x,ball.y }, ball.radius, Rectangle{ cpu.x,cpu.y,cpu.width,cpu.height }))
			{
				ball.speed_x *= -1;
				PlaySound(racketSound);
			}

			//Draw
			ball.Draw();
			cpu.Draw();
			player2.Draw();
			DrawLine(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), WHITE);
			DrawText(TextFormat("%i", player1_score), window_width / 4 - 20, 20, 80, WHITE);
			DrawText(TextFormat("%i", player2_score), 3 * window_width / 4 - 20, 20, 80, WHITE);

			//pause
			if (IsKeyPressed(KEY_ESCAPE)) {
				currentScreen = PAUSE;

			}
		}break;

		case GAMEPLAY: {
			gameplayType = 1;
			//Updating
			ball.Update();
			player1.Update1();
			player2.Update2();
			if (player1_score == 11) {
				currentScreen = GAME_OVER;
			}
			else if (player2_score == 11) {
				currentScreen = GAME_OVER;
			}


			//Check Collison
			if (CheckCollisionCircleRec(Vector2{ ball.x,ball.y }, ball.radius, Rectangle{ player2.x,player2.y,player2.width,player2.height }))
			{
				ball.speed_x *= -1;
				PlaySound(racketSound);
			}
			if (CheckCollisionCircleRec(Vector2{ ball.x,ball.y }, ball.radius, Rectangle{ player1.x,player1.y,player1.width,player1.height }))
			{
				ball.speed_x *= -1;
				PlaySound(racketSound);
			}

			//Drawing
			ball.Draw();
			player1.Draw();
			player2.Draw();
			DrawLine(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), WHITE);
			DrawText(TextFormat("%i", player1_score), window_width / 4 - 20, 20, 80, WHITE);
			DrawText(TextFormat("%i", player2_score), 3 * window_width / 4 - 20, 20, 80, WHITE);

			//pause
			if (IsKeyPressed(KEY_ESCAPE)) {
				currentScreen = PAUSE;
				
			}
		}break;

		case PAUSE: {
			lastWindowOpened = 1;
			DrawText("PAUSED", window_width / 2 - MeasureText("PAUSED", 60) / 2, 100, 60, WHITE);

			// Draw buttons
			DrawRectangleRec(continueBtn, CheckCollisionPointRec(mouse, continueBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(newGameBtn, CheckCollisionPointRec(mouse, newGameBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(optionsBtn, CheckCollisionPointRec(mouse, optionsBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(exitBtn, CheckCollisionPointRec(mouse, exitBtn) ? LIGHTGRAY : GRAY);

			DrawText("Continue", continueBtn.x + 55, continueBtn.y + 10, 20, BLACK);
			DrawText("New Game", newGameBtn.x + 55, newGameBtn.y + 10, 20, BLACK);
			DrawText("Options", optionsBtn.x + 60, optionsBtn.y + 10, 20, BLACK);
			DrawText("Exit to Main Menu", exitBtn.x + 10, exitBtn.y + 10, 20, BLACK);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				if (CheckCollisionPointRec(mouse, continueBtn)) {
					if (gameplayType == 0) {
						currentScreen = GAMEPLAY_CPU;
					}
					if (gameplayType == 1) {
						currentScreen = GAMEPLAY;
					}
				}
				else if (CheckCollisionPointRec(mouse, newGameBtn)) {
					player1_score = 0;
					player2_score = 0;
					ball.x = GetScreenWidth() / 2;
					ball.y = GetScreenHeight() / 2;
					currentScreen = PLAY_MENU;
				}
				else if (CheckCollisionPointRec(mouse, optionsBtn)) {
					currentScreen = OPTIONS;
				}
				else if (CheckCollisionPointRec(mouse, exitBtn)) {
					currentScreen = MAIN_MENU;
				}
			}
			
		}break;

		case GAME_OVER: {
			
			if (gameplayType == 0) {
				if (player1_score == 11) {
					DrawText("HEHEHEHEHE....", GetScreenWidth() / 2 - MeasureText("GAME OVER", 70)/2, 100, 70, WHITE);
					DrawText("YOU LOST", GetScreenWidth() / 2 - MeasureText("GAME OVER", 70)/2, 190, 50, WHITE);
					if (!loserSoundPlayer) {
						PlaySound(loserSound);
						loserSoundPlayer = true;
					}
				}
				else if (player2_score == 11) {
					DrawText("CONGRATULATIONS", GetScreenWidth() / 2 - MeasureText("GAME OVER", 70)/2, 100, 70, WHITE);
					DrawText("YOU WIN", GetScreenWidth() / 2 - MeasureText("GAME OVER", 70)/2, 190, 50, WHITE);
					if (!winnerSoundPlayed) {
						PlaySound(winnerSound);
						winnerSoundPlayed = true;
					}
				}

			}
			if (gameplayType == 1) {
				if (!gameOverSoundPlayed) {
					PlaySound(gameOverSound);
					gameOverSoundPlayed = true;
				}
				if (player1_score == 11) {
				DrawText("WINNER- PLAYER 1", GetScreenWidth() / 2 - MeasureText("GAME OVER", 70)/2, 100, 70, WHITE);
				}
				else if (player2_score == 11) {
				DrawText("WINNER- PLAYER 2", GetScreenWidth() / 2 - MeasureText("GAME OVER", 70)/2, 100, 70, WHITE);
				}
			}
			
			

			DrawRectangleRec(retryBtn, CheckCollisionPointRec(mouse, retryBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(menuBtn, CheckCollisionPointRec(mouse, menuBtn) ? LIGHTGRAY : GRAY);

			DrawText("Retry", retryBtn.x + 75, retryBtn.y + 10, 20, BLACK);
			DrawText("Main Menu", menuBtn.x + 55, menuBtn.y + 10, 20, BLACK);

			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				if (CheckCollisionPointRec(mouse, retryBtn)) {
					PlaySound(clickSound);
					player1_score = 0;
					player2_score = 0;
					ball.x = GetScreenWidth() / 2;
					ball.y = GetScreenHeight() / 2;
					if (gameplayType == 0) {
						currentScreen = GAMEPLAY_CPU;
					}
					if (gameplayType == 1) {
						currentScreen = GAMEPLAY;
					}
				}
				else if (CheckCollisionPointRec(mouse, menuBtn)) {
					PlaySound(clickSound);
					currentScreen = MAIN_MENU;
				}
				
			}
			
		}break;

		case OPTIONS: {
			DrawText("Options Screen - Press ESC to return", 30, 30, 20, DARKPURPLE);
			DrawText("Change Background Color", GetScreenWidth() / 2 - MeasureText("Change Background Color", 40) / 2, 70, 40, WHITE);
			DrawText("Change Ball Color", 165, 440, 40, WHITE);
			DrawText("Change Padle Color", GetScreenWidth() - (125 + MeasureText("Change Padle Color", 40)), 440, 40, WHITE);
			//Bg color change
			DrawRectangleRec(bgColorBlueBtn, CheckCollisionPointRec(mouse, bgColorBlueBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(bgColorRedBtn, CheckCollisionPointRec(mouse, bgColorRedBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(bgColorGreenBtn, CheckCollisionPointRec(mouse, bgColorGreenBtn) ? LIGHTGRAY : GRAY);
			
			//Ball color change
			DrawRectangleRec(ballColorBlackBtn, CheckCollisionPointRec(mouse, ballColorBlackBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(ballColorWhiteBtn, CheckCollisionPointRec(mouse, ballColorWhiteBtn) ? LIGHTGRAY : GRAY);

			//Padle color change
			DrawRectangleRec(padleColorBlackBtn, CheckCollisionPointRec(mouse, padleColorBlackBtn) ? LIGHTGRAY : GRAY);
			DrawRectangleRec(padleColorWhiteBtn, CheckCollisionPointRec(mouse, padleColorWhiteBtn) ? LIGHTGRAY : GRAY);


			DrawText("BLUE", bgColorBlueBtn.x + 75, bgColorBlueBtn.y + 15, 20, BLUE);
			DrawText("RED", bgColorRedBtn.x + 80, bgColorRedBtn.y + 15, 20, RED);
			DrawText("GREEN", bgColorGreenBtn.x + 70, bgColorGreenBtn.y + 15, 20, GREEN);
			DrawText("BLACK", ballColorBlackBtn.x + 70, ballColorBlackBtn.y + 15, 20, BLACK);
			DrawText("WHITE", ballColorWhiteBtn.x + 70, ballColorWhiteBtn.y + 15, 20, WHITE);
			DrawText("BLACK", padleColorBlackBtn.x + 70, padleColorBlackBtn.y + 15, 20, BLACK);
			DrawText("WHITE", padleColorWhiteBtn.x + 70, padleColorWhiteBtn.y + 15, 20, WHITE);

			testBall.Draw();
			testPadle.Draw();

			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				if (CheckCollisionPointRec(mouse,bgColorBlueBtn)) {
					PlaySound(clickSound);
					bgColor = BLUE;
				}
				else if (CheckCollisionPointRec(mouse, bgColorGreenBtn)) {
					PlaySound(clickSound);
					bgColor = GREEN;
				}
				else if (CheckCollisionPointRec(mouse, bgColorRedBtn)) {
					PlaySound(clickSound);
					bgColor = RED;
				}
				else if (CheckCollisionPointRec(mouse, ballColorBlackBtn)) {
					PlaySound(clickSound);
					ball.ballColor = BLACK;
					testBall.ballColor = BLACK;
				}
				else if (CheckCollisionPointRec(mouse, ballColorWhiteBtn)) {
					PlaySound(clickSound);
					ball.ballColor = WHITE;
					testBall.ballColor = WHITE;
				}
				else if (CheckCollisionPointRec(mouse, padleColorBlackBtn)) {
					PlaySound(clickSound);
					player1.padleColor = BLACK;
					player2.padleColor = BLACK;
					cpu.padleColor = BLACK;
					testPadle.padleColor = BLACK;
				}
				else if (CheckCollisionPointRec(mouse, padleColorWhiteBtn)) {
					PlaySound(clickSound);
					player1.padleColor = WHITE;
					player2.padleColor = WHITE;
					cpu.padleColor = WHITE;
					testPadle.padleColor = WHITE;
				}

			}

			if (IsKeyPressed(KEY_ESCAPE)) {
				if (lastWindowOpened == 0) {
					currentScreen = MAIN_MENU;
				}
				else if (lastWindowOpened == 1) {
					currentScreen = PAUSE;
				}
			}
			
			
		} break;

		case EXIT: {
			CloseWindow(); // End program
		} break;
		}

		EndDrawing();
	}

	UnloadSound(clickSound);
	UnloadSound(racketSound);
	UnloadSound(winnerSound);
	UnloadSound(loserSound);
	UnloadSound(gameOverSound);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}