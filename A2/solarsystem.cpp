// Standard C++ library imports
#include <stdio.h>
#include <stdlib.h>
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

// Struct for moon with various attributes
struct moon
{
	float color[3];
	float orbitPosition;
	float orbitRadius;
	float orbitSpeed;
	float size;
	moon(){}
	moon(float cr, float cg, float cb,
		float p, float r, float sp, float sz)
	{
		color[0] = cr;
		color[1] = cg;
		color[2] = cb;
		orbitPosition = p;
		orbitRadius = r;
		orbitSpeed = sp;
		size = sz;
	}
};

// Struct for planet with various attributes
struct planet
{
	float color[3];
	float orbitPosition;
	float orbitRadius;
	float orbitSpeed;
	float size;
	std::vector<moon> moons;
	planet(){}
	planet(float cr, float cg, float cb,
		float p, float r, float sp, float sz)
	{
		color[0] = cr;
		color[1] = cg;
		color[2] = cb;
		orbitPosition = p;
		orbitRadius = r;
		orbitSpeed = sp;
		size = sz;
	}
};

// Global vector of planets
std::vector<planet> planets;

// Global indexes of selected planet and moon
int selectedPlanet = -1;
int selectedMoon = -1;

// Global state variable for play/pause
int pauseState = 0;

// Global variable for zoom level
float zoomLevel = 1;

// Global variable for camera position
float camPos[] = {0, 0, 80};

// Draws a sphere (DO NOT CHANGE THIS FUNCTION)
void drawSphere()
{
	glutSolidSphere(1,10,10);
}

// Function for generating random number within given range
float randomFloat(float min, float max)
{
	return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

// Function to add new planet
void addPlanet()
{
	planets.push_back(planet(
		randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1),
		randomFloat(0, 360), randomFloat(5, 30),
		randomFloat(0.1, 5), randomFloat(0.5, 1.5)
	));
}

// Function to add new moon to a given planet
void addMoon(int index)
{
	planets.at(index).moons.push_back(moon(
		randomFloat(0, 0.5), randomFloat(0, 0.5),
		randomFloat(0, 0.5), randomFloat(0, 360),
		planets.at(index).size + randomFloat(1, 2),
		randomFloat(0.1, 5), randomFloat(0.05, 0.8)
	));
}

// Function to delete a planet
void deletePlanet(int index)
{
	// Return immediately if index out of bounds
	if (index < 0 || index >= planets.size())
		return;

	// Remove selected planet from planets vector
	planets.erase(planets.begin() + index);

	// Deselect planet
	selectedPlanet = -1;
}

// Function to delete a moon from a given planet
void deleteMoon(int pIndex, int mIndex)
{
	// Return immediately if either index out of bounds
	if (pIndex < 0 || pIndex >= planets.size() ||
		mIndex < 0 || mIndex >= planets.at(pIndex).moons.size())
		return;

	// Remove selected moon from moons vector of its planet
	planets.at(pIndex).moons.erase(
		planets.at(pIndex).moons.begin() + mIndex);

	// Move selection back to planet
	selectedMoon = -1;
}

// Function to advance rotation of planets/moons
void rotate(int val)
{
	// Do nothing if simulation is paused
	if (pauseState)
		goto end;

	// Iterate over all planets in planets vector
	for (int i = 0; i < planets.size(); i++)
	{
		// Advance rotation of current planet
		planets.at(i).orbitPosition
			+= planets.at(i).orbitSpeed / 2;

		// Iterate all moons in moons vector of current planet
		for (int j = 0; j < planets.at(i).moons.size(); j++)
		{
			// Advance rotation of current moon
			planets.at(i).moons.at(j).orbitPosition
				+= planets.at(i).moons.at(j).orbitSpeed / 2;
		}
	}
end:
	glutTimerFunc(1000 / 60, rotate, 0);
}

// Function to zoom into solar system
void zoomIn()
{
	if (zoomLevel < 2.0f)
		zoomLevel += 0.1f;
}

// Function to zoom out from solar system
void zoomOut()
{
	if (zoomLevel > 0.1f)
		zoomLevel -= 0.1f;
}

// Function to increase speed of specific planet
void increaseSpeedPlanet(int index)
{
	if (planets.at(index).orbitSpeed < 5)
			planets.at(index).orbitSpeed += 0.1;
}

// Function to increase speed of specific moon
void increaseSpeedMoon(int pIndex, int mIndex)
{
	if (planets.at(pIndex).moons.at(mIndex).orbitSpeed < 5)
		planets.at(pIndex).moons.at(mIndex).orbitSpeed += 0.1;
}

// Function to increase speed of every/selected planet/moon
void increaseSpeed()
{
	// Increase speed of all planets and moons if none selected
	if (selectedPlanet == -1)
	{
		for (int i = 0; i < planets.size(); i++)
		{
			increaseSpeedPlanet(i);
			for (int j = 0; j < planets.at(i).moons.size(); j++)
				increaseSpeedMoon(i, j);
		}
	}

	// Increase speed of selected planet
	if (selectedPlanet != -1 && selectedMoon == -1)
		increaseSpeedPlanet(selectedPlanet);

	// Increase speed of selected moon
	if (selectedPlanet != -1 && selectedMoon != -1)
		increaseSpeedMoon(selectedPlanet, selectedMoon);
}

// Function to decrease speed of specific planet
void decreaseSpeedPlanet(int index)
{
	if (planets.at(index).orbitSpeed > 0.1)
			planets.at(index).orbitSpeed -= 0.1;
}

// Function to decrease speed of specific moon
void decreaseSpeedMoon(int pIndex, int mIndex)
{
	if (planets.at(pIndex).moons.at(mIndex).orbitSpeed > 0.1)
			planets.at(pIndex).moons.at(mIndex).orbitSpeed -= 0.1;
}

// Function to decrease speed of every/selected planet/moon
void decreaseSpeed()
{
	// Decrease speed of all planets and moons if none selected
	if (selectedPlanet == -1)
	{
		for (int i = 0; i < planets.size(); i++)
		{
			decreaseSpeedPlanet(i);
			for (int j = 0; j < planets.at(i).moons.size(); j++)
				decreaseSpeedMoon(i, j);
		}
	}

	// Decrease speed of selected planet
	if (selectedPlanet != -1 && selectedMoon == -1)
		decreaseSpeedPlanet(selectedPlanet);

	// Decrease speed of selected moon
	if (selectedPlanet != -1 && selectedMoon != -1)
		decreaseSpeedMoon(selectedPlanet, selectedMoon);
}

// Function to increase size of specific planet
void increaseSizePlanet(int index)
{
	if (planets.at(index).size < 1.5)
			planets.at(index).size += 0.05;
}

// Function to increase size of specific moon
void increaseSizeMoon(int pIndex, int mIndex)
{
	if (planets.at(pIndex).moons.at(mIndex).size < 0.8)
			planets.at(pIndex).moons.at(mIndex).size += 0.05;
}

// Function to increase size of every/selected planet/moon
void increaseSize()
{
	// Increase size of all planets and moons if none selected
	if (selectedPlanet == -1)
	{
		for (int i = 0; i < planets.size(); i++)
		{
			increaseSizePlanet(i);
			for (int j = 0; j < planets.at(i).moons.size(); j++)
				increaseSizeMoon(i, j);
		}
	}

	// Increase size of selected planet
	if (selectedPlanet != -1 && selectedMoon == -1)
		increaseSizePlanet(selectedPlanet);

	// Increase size of selected moon
	if (selectedPlanet != -1 && selectedMoon != -1)
		increaseSizeMoon(selectedPlanet, selectedMoon);
}

// Function to decrease size of specific planet
void decreaseSizePlanet(int index)
{
	if (planets.at(index).size > 0.5)
			planets.at(index).size -= 0.05;
}

// Function to decrease size of specific moon
void decreaseSizeMoon(int pIndex, int mIndex)
{
	if (planets.at(pIndex).moons.at(mIndex).size > 0.05)
			planets.at(pIndex).moons.at(mIndex).size -= 0.05;
}

// Function to decrease size of every/selected planet/moon
void decreaseSize()
{
	// Decrease size of all planets and moons if none selected
	if (selectedPlanet == -1)
	{
		for (int i = 0; i < planets.size(); i++)
		{
			decreaseSizePlanet(i);
			for (int j = 0; j < planets.at(i).moons.size(); j++)
				decreaseSizeMoon(i, j);
		}
	}

	// Decrease size of selected planet
	if (selectedPlanet != -1 && selectedMoon == -1)
		decreaseSizePlanet(selectedPlanet);

	// Increase size of selected moon
	if (selectedPlanet != -1 && selectedMoon != -1)
		decreaseSizeMoon(selectedPlanet, selectedMoon);
}

// Initialization function
void init()
{
	// Initial black background and white color
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	// Load identity matrix for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Sets projection to perspective
	gluPerspective(45, 1, 1, 100);

	// Initiate planet rotation
	glutTimerFunc(1000 / 60, rotate, 0);
}

// Display function: GLUT display callback function
// clears the screen, sets the camera position,
// draws the groundplane and movable box
void display()
{
	// Clear OpenGL color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Load identity matrix for projection
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set camera position and initial color
	gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0, 1, 0);
	glColor3f(1, 1, 1);

	// Zoom in or out depending on current zoom level
	glScalef(zoomLevel, zoomLevel, 1);

	// Draw the sun!
	glPushMatrix();
		glColor3f(1, 1, 0);
		glScalef(2, 2, 1);
		drawSphere();
	glPopMatrix();

	// Draw planets in planets vector
	for (int i = 0; i < planets.size(); i++)
	{
		glPushMatrix();
			
			// Set position for planet and surrounding moons
			glRotatef(planets.at(i).orbitPosition, 0, 0, -1);
			glTranslatef(0, planets.at(i).orbitRadius, 0);

			// Draw moons in moons vector
			for (int j = 0; j < planets.at(i).moons.size(); j++)
			{
				glPushMatrix();
					if (selectedPlanet == i && selectedMoon == j)
						glColor3f(0.0f, 1.0f, 0.0f);
					else
						glColor3fv(planets.at(i).moons
							.at(j).color);
					glRotatef(planets.at(i).moons
						.at(j).orbitPosition, 0, 0, -1);
					glTranslatef(0, planets.at(i).moons
						.at(j).orbitRadius, 0);
					glScalef(planets.at(i).moons.at(j).size,
						planets.at(i).moons.at(j).size, 1);
					drawSphere();
				glPopMatrix();
			}

			// Set color and size of planet and draw
			if (selectedPlanet == i && selectedMoon == -1)
				glColor3f(0.0f, 1.0f, 0.0f);
			else
				glColor3fv(planets.at(i).color);
			glScalef(planets.at(i).size, planets.at(i).size, 1);
			drawSphere();

		glPopMatrix();
	}

	// Flush out to single buffer
	glFlush();

	// Redraw grid and moving robot continuously
	glutPostRedisplay();
}

// Keyboard function: handles standard keyboard controls
void keyboard(unsigned char key, int x, int y)
{
	// Perform action depending on character received
	switch (key)
	{
		// Add new planet/moon
		case 'a':
		case 'A':
			// Add planet to end of planets vector
			if (selectedPlanet == -1)
				addPlanet();

			// Add moon to selected planet
			else
				addMoon(selectedPlanet);

			break;

		// Delete planet/moon
		case 'd':
		case 'D':
			// Delete last planet if nothing selected
			if (selectedPlanet == -1)
				deletePlanet(planets.size() - 1);

			// Delete planet if selected
			else if (selectedMoon == -1)
				deletePlanet(selectedPlanet);

			// Delete moon if selected
			else
				deleteMoon(selectedPlanet, selectedMoon);

			break;

		// Pause/resume simulation
		case 'p':
		case 'P':
			pauseState = !pauseState;
			break;

		// Exit program
		case 'q':
		case 'Q':
			exit(0);
			break;

		// Reset solar system
		case 'r':
		case 'R':
			planets.clear();
			break;

		// Start/stop selection mode
		case ' ':
			// No selection to make with no planets
			if (planets.size() == 0)
				break;

			// Select first planet on start selection mode
			if (selectedPlanet == -1)
			{
				selectedPlanet = 0;
			}

			// Unselect planets/moons on stop selection mode
			else
			{
				selectedPlanet = -1;
				selectedMoon = -1;
			}
			break;

		// Increase/decrease speed of selected planet/moon
		case '+':
			increaseSpeed();
			break;
		case '-':
			decreaseSpeed();
			break;

		// Increase/decrease size of selected planet/moon
		case '>':
			increaseSize();
			break;
		case '<':
			decreaseSize();
			break;
	}
}

// Special function: handle special keyboard controls
void special(int key, int x, int y)
{
	// Perform action depending on which key pressed
	switch (key)
	{
		// Cycle backward through moons and planets
		case GLUT_KEY_LEFT:

			// Do nothing if not in selection mode
			if (selectedPlanet == -1)
				break;

			// Cycle backward through current planet and its moons
			if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
			{
				// Move to previous moon or back to planet
				if (selectedMoon > -1)
					selectedMoon--;

				// Move from planet to last moon
				else
					selectedMoon = planets
						.at(selectedPlanet).moons.size() - 1;
			}

			// Cycle backward through planets
			else
			{
				// Select previous planet and not its moons
				selectedMoon = -1;

				// Move to previous planet
				if (selectedPlanet > 0)
					selectedPlanet--;

				// Move from first to last planet
				else
					selectedPlanet = planets.size() - 1;
			}

			break;

		/// Cycle forward through moons and planets
		case GLUT_KEY_RIGHT:

			// Do nothing if not in selection mode
			if (selectedPlanet == -1)
				break;

			// Cycle forward through current planet and its moons
			if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
			{
				// Move from last moon to planet
				if (selectedMoon == planets
					.at(selectedPlanet).moons.size() - 1)
					selectedMoon = -1;

				// Move to first or next moon
				else
					selectedMoon++;
			}

			// Cycle forward through planets
			else
			{
				// Select next planet and not its moons
				selectedMoon = -1;

				// Move from last to first planet
				if (selectedPlanet == planets.size() - 1)
					selectedPlanet = 0;

				// Move to next planet
				else
					selectedPlanet++;
			}

			break;

		// Zoom into or out from solar system
		case GLUT_KEY_PAGE_UP:
			zoomIn();
			break;
		case GLUT_KEY_PAGE_DOWN:
			zoomOut();
			break;
	}
}

// Main function: program entry point and initialization
int main(int argc, char** argv)
{
	// GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);

	// Set initial window size
	glutInitWindowSize(600, 600);

	// Create window for displaying solar system
	glutCreateWindow("3GC3 - Assignment 2 (Part B)");

	// I/O function bindings
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	// Call initialization logic
	init();

	// Main program loop
	glutMainLoop();

	// Exit normally
	return(0);
}
