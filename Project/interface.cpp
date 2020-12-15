// Standard C++ library imports
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

// OpenGL and GLUT imports
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

// Local imports
#include "material.cpp"
#include "object.h"

// Static global values for display dimensions
static int DISPLAY_WIDTH = 500;
static int DISPLAY_HEIGHT = 500;
static int DISPLAY_OFFSET_WIDTH;
static int DISPLAY_OFFSET_HEIGHT;

// Static Boolean flags for content to show within pages
static int showInstructions = 0;
static int quitConfirmation = 0;

// Static values for score calculation and display
static char scoreMessage[64] = { '\0' };
static int scoreFlashTime = 0;
enum ScoreState { SCORE_NORMAL, SCORE_INCREASE, SCORE_DECREASE };
static ScoreState scoreState;
static const int highScoresToDisplay = 5;

// Static values for fade on game over
static int gameOverFadeTime = 0;
static const int GAME_OVER_FADE_LENGTH = 2500;
static const int GAME_OVER_SHOW_MSG_TIME = 1500;

// Set perspective projection where needed
static void setPerspectiveProjection(float distance)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 0, distance);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Set orthographic projection where needed
static void setOrthographicProjection()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, DISPLAY_WIDTH, 0, DISPLAY_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Display full-screen overlay with a given color
static void displayOverlay(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f(DISPLAY_WIDTH, 0);
		glVertex2f(DISPLAY_WIDTH, DISPLAY_HEIGHT);
		glVertex2f(0, DISPLAY_HEIGHT);
	glEnd();
}

// Display text with given buffer and coordinates
static void displayText(char *buf, const float *color,
	float width, float height, float depth, float override = 0)
{
	// Disable fog temporarily
	glDisable(GL_FOG);

	// Set to given color
	glColor3fv(color);

	// Center text unless explicit width or override specified
	glRasterPos3f(override ? width :
		(DISPLAY_WIDTH / 2 - 4.5 * (width ? width : strlen(buf))),
		(override ? 1 : DISPLAY_HEIGHT) * height, depth);

	// Populate raster image to display text
	for (int i = 0; i < strlen(buf); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, buf[i]);

	// Re-enable fog
	glEnable(GL_FOG);
}

// Specific logic for main menu
static void mainMenu(std::vector<int> highScores)
{
	// Translucent black overlay for background
	displayOverlay(0, 0, 0, 0.5);

	// Display instructions
	if (showInstructions)
	{
		// Define array of lines to display
		char instructions[12][64] =
		{
			"Controls:",
			" - Arrow keys to steer the rocket.",
			" - Hold Spacebar to burst forward.",
			" - Press `P' to pause the game.",
			"",
			"Instructions:",
			" - Keep travelling to build up your score.",
			" - Collect coins to increase your score.",
			" - Avoid obstacles which lower your score.",
			" - Game over when a bomb has been hit.",
			"",
			"Press Spacebar to return to main menu."
		};

		// Display lines with descending height, group using color
		for (int i = 0; i < 12; i++)
			displayText((char*)instructions[i],
				i <= 4 ? orange :
					(i <= 10 ? lightgreen : grey),
				43, 0.8 - i * 0.05, 0);
	}

	// Display main menu interface
	else
	{
		// Display instruction to begin game
		char begin[32] = "Press Spacebar to begin.";
		displayText(begin, lightblue, 0, 0.8, 0);

		// Display high scores if at least one exists
		if (!highScores.empty())
			displayText((char*)"High scores:", offwhite,
				strlen(begin), 0.7, 0);

		// Iterate through high scores to display
		for (int i = 1; i <= highScoresToDisplay; i++)
		{
			if (highScores.size() >= i)
			{
				memset(scoreMessage, '\0', sizeof(scoreMessage));
				snprintf(scoreMessage, sizeof(scoreMessage),
					"%d: %d", i, highScores.at(i - 1));
				displayText(scoreMessage, offwhite,
					ceil(log(highScores.at(0)) / log(10)) + 3,
					0.675 - 0.05 * i, 0);
			}
		}

		// Display instruction for instructions
		char instructionsMessage[48] =
			"Press Shift + Spacebar for instructions.";
		displayText(instructionsMessage, orange,
			0, 0.55 - 0.05 * highScoresToDisplay, 0);

		// Display instruction for quit game
		char quitMessage[48] =
			"Press `Q' or Esc to quit the game.";
		displayText(quitMessage, grey,
			0, 0.45 - 0.05 * highScoresToDisplay, 0);
	}
}

// Specific logic for pause menu
static void pauseMenu()
{
	// Translucent black overlay for background
	displayOverlay(0, 0, 0, 0.5);

	// Display quit confirmation dialog
	if (quitConfirmation)
	{
		// Display confirmation dialog for quitting game
		char message[32] = "Are you sure you want to quit?";
		displayText(message, orange, 0, 0.7, 0);

		// Display instructions to go back
		char goback[32] =
			"Press Spacebar to go back.";
		displayText(goback, grey, 0, 0.4, 0);

		// Display instructions to confirm
		char confirm[48] =
			"Press Shift + Spacebar to confirm.";
		displayText(confirm, lightred, 0, 0.3, 0);
	}

	// Display pause menu interface
	else
	{
		// Display instructions to resume game
		char resume[32] = "Press Spacebar to resume.";
		displayText(resume, grey, 0, 0.7, 0);

		// Display instructions to quit game
		char quit[32] = "Press Shift + Spacebar to quit.";
		displayText(quit, lightred, 0, 0.3, 0);
	}
}

// Specific logic for game over
static void gameOver(int score, std::vector<int> highScores)
{
	// Show gradual fade from red to black
	float redness =
		std::max(0.5 * (GAME_OVER_FADE_LENGTH - gameOverFadeTime)
			/ GAME_OVER_FADE_LENGTH, 0.0);
	float alpha =
		std::min(0.5 + 0.5 * gameOverFadeTime
			/ GAME_OVER_FADE_LENGTH, 1.0);
	displayOverlay(redness, 0, 0, alpha);

	// Display text shortly after game over
	if (gameOverFadeTime >= GAME_OVER_SHOW_MSG_TIME)
	{
		// Display score message (already populated)
		displayText(scoreMessage, offwhite, 0, 0.6, 0);

		// Display message if new personal best achieved
		if (score == highScores.at(0))
		{
			char bestScore[32] = "New personal best score!";
			displayText(bestScore, lightgreen, 0, 0.5, 0);
		}

		// Display message if new score added to high scores list
		else if (score >=
			highScores.at(std::min(highScoresToDisplay,
				(int)(highScores.size())) - 1))
		{
			char topScore[32];
			memset(topScore, '\0', sizeof(topScore));
			snprintf(topScore, sizeof(topScore),
				"New top %d score!", highScoresToDisplay);
			displayText(topScore, lightgreen, 0, 0.5, 0);
		}

		// Display prompt to start new game
		char cont[32] = "Press Spacebar to continue.";
		displayText(cont, offwhite, 0, 0.4, 0);
	}
}

// Calculate and show score
static void showScore(Rocket *rocket, int score)
{
	// Set score color depending on score flash
	if (scoreFlashTime == 0)
		scoreState = SCORE_NORMAL;
	const float *scoreColor =
		scoreState == SCORE_NORMAL
			? offwhite
			: (scoreState == SCORE_INCREASE
				? green
				: red);

	// Populate score display message
	memset(scoreMessage, '\0', sizeof(scoreMessage));
	snprintf(scoreMessage, sizeof(scoreMessage),
		"Score: %d", score);
	displayText(scoreMessage, scoreColor,
		rocket->positionX * 0.6 - 0.15 * strlen(scoreMessage),
		rocket->positionY * 0.625 - 3.85, rocket->positionZ + 5, 1);
}
