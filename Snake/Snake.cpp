#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE

#include <iostream>
#include <deque>
#include <thread>
#include <chrono>
#include <Windows.h>

int screenWidth = 120;
int screenHeight = 30;

enum Direction { Up, Down, Left, Right };

int main()
{
	//Create screen buffer
	wchar_t* screen = new wchar_t[screenWidth * screenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	while (true) {
		bool bDead = false;
		int SPEED = 1;
		int score = 0;
		
		//Intitial snake starting position
		int snakeXPos = 50;
		int snakeYPos = 20;

		int appleXPos = 0;
		int appleYPos = 0;
		bool appleExist = false;

		//Initialize the snake size and its start position
		int snakeSize = 4;
		std::deque<int> snake = { snakeYPos * screenWidth + snakeXPos - 1 ,
		snakeYPos * screenWidth + snakeXPos - 1,
		snakeYPos * screenWidth + snakeXPos - 1,
		snakeYPos * screenWidth + snakeXPos };

		//Initialize the direction
		Direction direction = Right;
		Direction prevDirection = Right;

		while (!bDead)
		{
			//This while loop is for getting the input even while not rendering a frame
			auto time1 = std::chrono::system_clock::now();
			while ((std::chrono::system_clock::now() - time1) < std::chrono::milliseconds(120))
			{
				//Gets direction pressed
				if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
					direction = Left;

				else if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
					direction = Right;

				else if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
					direction = Up;

				else if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
					direction = Down;
			}

			//Clears the screen
			for (int i = 0; i < screenWidth * screenHeight; i++)
				screen[i] = L' ';

			//Draws the score and border
			for (int i = 0; i < screenWidth; i++)
			{
				screen[i] = L'=';
				screen[2 * screenWidth + i] = L'=';
			}
			wsprintf(&screen[screenWidth + 5], L"SCORE: %d", score);
			
			//Changes the X,Y of the snake head depending on the direction
			switch (direction)
			{
			case Left:
				snakeXPos -= SPEED;
				break;

			case Right:
				snakeXPos += SPEED;
				break;

			case Up:
				snakeYPos -= SPEED;
				break;

			case Down:
				snakeYPos += SPEED;
				break;
			}

			//Adjusts the Snake Body based on where it is moving
			int prevPos{};
			int currentPos{};
			for (int i = snakeSize - 1; i >= 0; i--)
			{
				if (i == snakeSize - 1)
				{
					prevPos = snake[i];
					snake[i] = snakeYPos * screenWidth + snakeXPos;
				}
				else
				{
					currentPos = snake[i];
					snake[i] = prevPos;
					prevPos = currentPos;
				}
			}

			//Spawns the apple if it doesn't exist already
			if (!appleExist)
			{
				appleXPos = rand() % screenWidth;
				appleYPos = rand() % (screenHeight - 3) + 3;
				appleExist = true;
			}
			screen[appleYPos * screenWidth + appleXPos] = L'G';


			//Checks if the snake hits a wall
			if (snakeXPos < 0 || snakeXPos >= screenWidth || snakeYPos < 3 || snakeYPos >= screenHeight)
			{
				bDead = true;
			}

			//Checks if the snake hits itself
			else
			{
				for (int i = 0; i < snakeSize - 1; i++)
				{
					if (snake[i] == snake[snakeSize - 1])
					{
						bDead = true;
						break;
					}
				}
			}

			//Checks if the snake Hits an apple
			if (snake[snakeSize - 1] == (appleYPos * screenWidth + appleXPos))
			{
				appleExist = false;
				snakeSize++;
				score++;

				snake.push_front(snake.front());
			
			}

			//Place the snake into the screen buffer to be drawn
			for (int i = 0; i < snakeSize; i++)
			{
				if (i == snakeSize - 1 )
					screen[snake[i]] = bDead? L'X':L'@';
				else
					screen[snake[i]] = bDead ? L'-' : L'#';
			}

			//Saves the previous direction to add snake parts
			prevDirection = direction;

			//Displays Death Message
			if (bDead)
				wsprintf(&screen[15 * screenWidth + 40], L"    PRESS 'SPACE' TO PLAY AGAIN    ");

			// Display Frame
			screen[screenWidth * screenHeight - 1] = '\0';
			WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
		}

		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
	}
}
