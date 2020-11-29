// Standard C++ library imports
#include <float.h>
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

// Local library imports
#include "material.cpp"
#include "object.cpp"

// Enum for direction (used for keyboard actions)
enum Direction { LEFT, RIGHT, DOWN, UP, BACK, FRONT };
Direction direction;

// Enum for selections which are not object indices
enum NonIndexSelections { NONE = -1, LIGHT1 = -2, LIGHT2 = -3 };

// Index of selected object
static int selected;

// Currently selected material
static Material material;

// Constant values for display
const float axisLength = 100;
const float initialZoom = 10;

// Camera position
float cameraPos[3];

// Lighting values with variable positions
float lightPos1[] = { 0, 0, 0, 1 };
float lightPos2[] = { 0, 0, 0, 1 };
float lightAmb[] = { 0.35, 0.35, 0.35, 1 };
float lightDif[] = { 0.35, 0.35, 0.35, 1 };
float lightSpc[] = { 0.35, 0.35, 0.35, 1 };
float lightSphereRadius = 0.1;

// Mouse ray start and end
double* rayStart = new double[3];
double* rayEnd = new double[3];

// Filename for loading/saving scene
char filename[64];

// Linear scene graph of objects
std::vector<Object> objects;

// Initialize global variables;
void initialSettings()
{
	objects.clear();
	cameraPos[X] = initialZoom;
	cameraPos[Y] = initialZoom / 2;
	cameraPos[Z] = initialZoom;
	lightPos1[X] = 1;
	lightPos1[Y] = initialZoom;
	lightPos1[Z] = initialZoom;
	lightPos2[X] = initialZoom;
	lightPos2[Y] = initialZoom;
	lightPos2[Z] = 1;
	material = TURQUOISE;
	selected = NONE;
}

// Nudge value in vector by given direction/amount with constraints
void applyChange(float * vec, Direction direction,
	float amount, float min = -FLT_MAX, float max = FLT_MAX)
{
	switch (direction)
	{
	case LEFT:
		if (vec[X] >= min + amount)
			vec[X] -= amount;
		break;
	case RIGHT:
		if (vec[X] <= max - amount)
			vec[X] += amount;
		break;
	case DOWN:
		if (vec[Y] >= min + amount)
			vec[Y] -= amount;
		break;
	case UP:
		if (vec[Y] <= max - amount)
			vec[Y] += amount;
		break;
	case BACK:
		if (vec[Z] >= min + amount)
			vec[Z] -= amount;
		break;
	case FRONT:
		if (vec[Z] <= max - amount)
			vec[Z] += amount;
		break;
	}
}

// Compute ray and store globally
void computeRay(int x, int y)
{
	// Get model view and model projection
	double matModelView[16], matProjection[16];
	glGetDoublev( GL_MODELVIEW_MATRIX, matModelView );
	glGetDoublev( GL_PROJECTION_MATRIX, matProjection );

	// Get viewport
	int viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );

	// Get window x and y coordinates (y is inverted)
	double winX = (double)x, winY = viewport[3] - (double)y;

	// Get coordinates at beginning of mouse ray
	gluUnProject(winX, winY, 0.0, matModelView, matProjection,
			viewport, &rayStart[X], &rayStart[Y], &rayStart[Z]);

	// Get coordinates at end of mouse ray
	gluUnProject(winX, winY, 1.0, matModelView, matProjection,
			viewport, &rayEnd[X], &rayEnd[Y], &rayEnd[Z]);
}

// Check if object intersects ray and return distance if so
double raySphereIntersection(double *rayStart, double *rayEnd,
	float *position, float radius)
{
	// Vector toward object from ray origin
	double toObject[3] =
	{
		position[X] - rayStart[X],
		position[Y] - rayStart[Y],
		position[Z] - rayStart[Z]
	};

	// Ray vector from start to end
	double ray[3] =
	{
		rayEnd[X] - rayStart[X],
		rayEnd[Y] - rayStart[Y],
		rayEnd[Z] - rayStart[Z]
	};

	// Dot product of vector to object with ray
	double toObjectDotRay = toObject[X] * ray[X]
		+ toObject[Y] * ray[Y] + toObject[Z] * ray[Z];

	// Ray magnitude squared
	double rayMagnitudeSquared = pow(ray[X], 2)
		+ pow(ray[Y], 2) + pow(ray[Z], 2);

	// Projection of vector to object onto ray
	double projection[3] =
	{
		toObjectDotRay / rayMagnitudeSquared * ray[X],
		toObjectDotRay / rayMagnitudeSquared * ray[Y],
		toObjectDotRay / rayMagnitudeSquared * ray[Z],
	};

	// Length of projection (distance to object from ray origin)
	double projectionLength = sqrt(pow(projection[X], 2)
		+ pow(projection[Y], 2) + pow(projection[Z], 2));

	// Point on ray nearest to object
	double rayNearest[3] =
	{
		rayStart[X] + projection[X],
		rayStart[Y] + projection[Y],
		rayStart[Z] + projection[Z]
	};

	// Distance from object to nearest point on ray
	double distance = fabs(sqrt(pow(position[X] - rayNearest[X], 2)
		+ pow(position[Y] - rayNearest[Y], 2)
		+ pow(position[Z] - rayNearest[Z], 2)));

	// Return distance to object if intersects, or indicator otherwise
	return distance <= radius ? projectionLength : -1;
}

// Load scene from file
void loadScene(char *filename)
{
	// Open file for read, warn if not found
	FILE *file = fopen(filename, "r");
	if (!file)
	{
		printf("File not found.\n");
		return;
	}

	// First revert to initial settings
	initialSettings();

	// Temporary variables for selection/material set last
	int tempSelected, tempMaterial;

	// Get saved settings from first row
	fscanf(file, "%f %f %f %f %f %f %f %f %f %d %d\n",
		&cameraPos[X], &cameraPos[Y], &cameraPos[Z],
		&lightPos1[X], &lightPos1[Y], &lightPos1[Z],
		&lightPos2[X], &lightPos2[Y], &lightPos2[Z],
		&tempSelected, &tempMaterial);

	// Get objects and populate vector from remaining rows
	while (!feof(file))
	{
		Object o = Object(TURQUOISE, CUBE);
		fscanf(file, "%f %f %f %f %f %f %f %f %f %d %d\n",
			&o.position[X], &o.position[Y], &o.position[Z],
			&o.rotation[X], &o.rotation[Y], &o.rotation[Z],
			&o.scale[X], &o.scale[Y], &o.scale[Z],
			(int*)&o.material, (int*)&o.objectType);
		objects.push_back(o);
	}

	// Set selection/material last
	selected = tempSelected;
	material = (Material)tempMaterial;

	// Close file
	fclose(file);

	// Print confirmation message to console
	printf("Successfully loaded scene from file '%s'\n", filename);
}

// Save scene from file
void saveScene(char *filename)
{
	// Open file for write
	FILE *file = fopen(filename, "w");

	// Write current settings to first row
	fprintf(file, "%f %f %f %f %f %f %f %f %f %d %d\n",
		cameraPos[X], cameraPos[Y], cameraPos[Z],
		lightPos1[X], lightPos1[Y], lightPos1[Z],
		lightPos2[X], lightPos2[Y], lightPos2[Z],
		selected, material);

	// Write object metadata to remaining rows
	for (int i = 0; i < objects.size(); i++)
	{
		Object o = objects.at(i);
		fprintf(file, "%f %f %f %f %f %f %f %f %f %d %d\n",
			o.position[X], o.position[Y], o.position[Z],
			o.rotation[X], o.rotation[Y], o.rotation[Z],
			o.scale[X], o.scale[Y], o.scale[Z],
			o.material, o.objectType);
	}

	// Close file
	fclose(file);

	// Print confirmation message to console
	printf("Successfully saved scene to file '%s'\n", filename);
}

// Draw planes along 3 axes
void drawAxisBounds()
{
	// y/z plane on boundary of display area
	plainColorMaterial(0.45, 0.5, 0.5);
	glNormal3f(1, 0, 0);
	glBegin(GL_QUADS);
		glVertex3f(0, 0, 0);
		glVertex3f(0, axisLength, 0);
		glVertex3f(0, axisLength, axisLength);
		glVertex3f(0, 0, axisLength);
	glEnd();

	// x/y plane on boundary of display area
	plainColorMaterial(0.5, 0.5, 0.45);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
		glVertex3f(0, 0, 0);
		glVertex3f(axisLength, 0, 0);
		glVertex3f(axisLength, axisLength, 0);
		glVertex3f(0, axisLength, 0);
	glEnd();

	// x/z plane on boundary of display area
	plainColorMaterial(0.5, 0.45, 0.5);
	glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, axisLength);
		glVertex3f(axisLength, 0, axisLength);
		glVertex3f(axisLength, 0, 0);
	glEnd();

	// x axis line separating planes
	plainColorMaterial(0.35, 0, 0);
	glBegin(GL_LINES);
		glVertex3f(0.1, 0.1, 0.1);
		glVertex3f(axisLength, 0.1, 0.1);
	glEnd();

	// y axis line separating planes
	plainColorMaterial(0, 0.35, 0);
	glBegin(GL_LINES);
		glVertex3f(0.1, 0.1, 0.1);
		glVertex3f(0.1, axisLength, 0.1);
	glEnd();

	// z axis line separating planes
	plainColorMaterial(0, 0, 0.35);
	glBegin(GL_LINES);
		glVertex3f(0.1, 0.1, 0.1);
		glVertex3f(0.1, 0.1, axisLength);
	glEnd();
}

// Display function: renders display area
void display()
{
	// Reset display before rendering display area
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Position camera away from and looking at origin
	gluLookAt(
		cameraPos[X], cameraPos[Y], cameraPos[Z],
		0, 0, 0,
		0, 1, 0
	);

	// Draw planes along 3 axes
	drawAxisBounds();

	// Set light values for first light source
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpc);

	// Set light values for second light source
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpc);

	// Depict first light source as tiny bright red sphere
	if (selected == LIGHT1)
		plainColorMaterial(0, 1, 0);
	else
		plainColorMaterial(1, 0.75, 0.75);
	glPushMatrix();
		glTranslatef(lightPos1[X], lightPos1[Y], lightPos1[Z]);
		glutSolidSphere(lightSphereRadius, 100, 100);
	glPopMatrix();

	// Depict second light source as tiny bright blue sphere
	if (selected == LIGHT2)
		plainColorMaterial(0, 1, 0);
	else
		plainColorMaterial(0.75, 0.75, 1);
	glPushMatrix();
		glTranslatef(lightPos2[X], lightPos2[Y], lightPos2[Z]);
		glutSolidSphere(lightSphereRadius, 100, 100);
	glPopMatrix();

	// Iterate through linear scene graph and draw objects
	for (int i = 0; i < objects.size(); i++)
		objects.at(i).drawObject(selected == i);

	// Flushes buffered commands to display
	glFlush();

	// Redraw display area continuously
	glutPostRedisplay();
}

// Keyboard function: handles standard keyboard controls
void keyboard(unsigned char key, int x, int y)
{
	// Perform action depending on character received
	switch (key)
	{
	// Add objects or choose material/texture
	case '1':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, CUBE));
		else
			material = TURQUOISE;
		break;
	case '2':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, SPHERE));
		else
			material = RED_RUBBER;
		break;
	case '3':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, CONE));
		else
			material = GREEN_PLASTIC;
		break;
	case '4':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, CYLINDER));
		else
			material = PERL;
		break;
	case '5':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, TORUS));
		else
			material = OBSIDIAN;
		break;
	case '6':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, TEAPOT));
		else
			material = TIN;
		break;
	case '7':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, TETRAHEDRON));
		else
			material = POLISHED_BRONZE;
		break;
	case '8':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, OCTAHEDRON));
		else
			material = CARPET;
		break;
	case '9':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, DODECAHEDRON));
		else
			material = MARBLE;
		break;
	case '0':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, ICOSAHEDRON));
		else
			material = ROCK;
		break;

	// Load scene from file
	case 'l':
	case 'L':
		memset(filename, '\0', sizeof(filename));
		printf("Enter a filename ('cancel' to cancel): ");
		scanf("%63s", filename);
		if (!strstr(filename, "cancel"))
			loadScene(filename);
		break;

	// Change material of selected object
	case 'm':
	case 'M':
		if (selected >= 0)
			objects.at(selected).material = material;
		break;

	// Quit program
	case 'q':
	case 'Q':
	case 27:
		exit(0);
		break;

	// Clear the scene graph of objects
	case 'r':
	case 'R':
		initialSettings();
		break;

	// Save scene to file
	case 's':
	case 'S':
		memset(filename, '\0', sizeof(filename));
		printf("Enter a filename ('cancel' to cancel): ");
		scanf("%63s", filename);
		if (!strstr(filename, "cancel"))
			saveScene(filename);
		break;
	}
}

// Special function: handle special keyboard controls
void special(int key, int x, int y)
{
	// Assign direction from arrow key pressed
	switch (key)
	{
	case GLUT_KEY_LEFT:
		direction = LEFT;
		break;
	case GLUT_KEY_RIGHT:
		direction = RIGHT;
		break;
	case GLUT_KEY_DOWN:
		direction = DOWN;
		break;
	case GLUT_KEY_UP:
		direction = UP;
		break;
	case GLUT_KEY_PAGE_DOWN:
		direction = BACK;
		break;
	case GLUT_KEY_PAGE_UP:
		direction = FRONT;
		break;
	default:
		return;
	}

	// Perform action depending on direction and key modifiers
	if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
	{
		if (selected >= 0)
			applyChange(objects.at(selected).rotation, direction, 1);
	}
	else if (glutGetModifiers() == GLUT_ACTIVE_ALT)
	{
		if (selected >= 0)
			applyChange(objects.at(selected).scale,
				direction, 0.5, 0.5, axisLength);
	}
	else if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
	{
		if (selected >= 0)
			applyChange(objects.at(selected).position,
				direction, 1, 0, axisLength);
	}
	else if (glutGetModifiers() == (GLUT_ACTIVE_CTRL|GLUT_ACTIVE_SHIFT)
		|| selected == LIGHT1)
	{
		applyChange(lightPos1, direction, 1, 1, axisLength);
	}
	else if (glutGetModifiers() == (GLUT_ACTIVE_ALT|GLUT_ACTIVE_SHIFT)
		|| selected == LIGHT2)
	{
		applyChange(lightPos2, direction, 1, 1, axisLength);
	}
	else
	{
		applyChange(cameraPos, direction, 1, 2, axisLength);
	}
}

// Mouse function: handles mouse click actions
void mouse(int btn, int state, int x, int y){
	// Only handle actions for left and right click (key down)
	if ((btn != GLUT_LEFT_BUTTON && btn != GLUT_RIGHT_BUTTON)
		|| state != GLUT_DOWN)
		return;

	// Compute ray from mouse x/y
	computeRay(x, y);

	// Find object with closest intersect with mouse ray
	int nearest = NONE;
	double closestIntersect = DBL_MAX;
	for (int i = 0; i < objects.size(); i++)
	{
		double intersect = raySphereIntersection(rayStart, rayEnd,
			objects.at(i).position, objects.at(i).boundRadius());
		if (intersect != -1 && intersect < closestIntersect)
		{
			closestIntersect = intersect;
			nearest = i;
		}
	}

	// Select nearest object/light source on left click
	if (btn == GLUT_LEFT_BUTTON)
	{
		// Select first light source if clicked
		double intersect = raySphereIntersection(rayStart, rayEnd,
			lightPos1, lightSphereRadius);
		if (intersect != -1 && intersect < closestIntersect)
		{
			closestIntersect = intersect;
			nearest = LIGHT1;
		}

		// Select second light source if clicked
		intersect = raySphereIntersection(rayStart, rayEnd,
			lightPos2, lightSphereRadius);
		if (intersect != -1 && intersect < closestIntersect)
		{
			closestIntersect = intersect;
			nearest = LIGHT2;
		}

		selected = nearest;
	}

	// Delete nearest object on right click
	if (btn == GLUT_RIGHT_BUTTON && nearest >= 0)
	{
		objects.erase(objects.begin() + nearest);
		if (selected == nearest)
			selected = NONE;
	}
}

// Motion function: pivot camera on mouse drag
void motion(int x, int y)
{
	// Get inverted mouse ray before mouse drag
	double prevRay[3] =
	{
		rayStart[X] - rayEnd[X],
		rayStart[Y] - rayEnd[Y],
		rayStart[Z] - rayEnd[Z]
	};

	// Normalize mouse ray before mouse drag
	double prevRayDistance = fabs(sqrt(pow(prevRay[X], 2)
		+ pow(prevRay[Y], 2) + pow(prevRay[Z], 2)));
	double prevRayNormal[] =
	{
		prevRay[X] / prevRayDistance,
		prevRay[Y] / prevRayDistance,
		prevRay[Z] / prevRayDistance
	};

	// Normalize camera position before mouse drag
	double prevCamDistance = fabs(sqrt(pow(cameraPos[X], 2)
		+ pow(cameraPos[Y], 2) + pow(cameraPos[Z], 2)));
	double prevCamNormal[] =
	{
		cameraPos[X] / prevCamDistance,
		cameraPos[Y] / prevCamDistance,
		cameraPos[Z] / prevCamDistance
	};

	// Compute new ray from mouse x/y
	computeRay(x, y);

	// Get inverted mouse ray after mouse drag
	double ray[3] =
	{
		rayStart[X] - rayEnd[X],
		rayStart[Y] - rayEnd[Y],
		rayStart[Z] - rayEnd[Z]
	};

	// Normalize mouse ray after mouse drag
	double rayDistance = fabs(sqrt(pow(prevRay[X], 2)
		+ pow(prevRay[Y], 2) + pow(prevRay[Z], 2)));
	double rayNormal[] =
	{
		ray[X] / rayDistance,
		ray[Y] / rayDistance,
		ray[Z] / rayDistance
	};

	// Adjust camera within bounds and with reduced sensitivity
	cameraPos[X] = fmax(1, prevCamDistance * (prevCamNormal[X]
		+ 0.5 * (rayNormal[X] - prevRayNormal[X])));
	cameraPos[Y] = fmax(1, prevCamDistance * (prevCamNormal[Y]
		+ 0.5 * (rayNormal[Y] - prevRayNormal[Y])));
	cameraPos[Z] = fmax(1, prevCamDistance * (prevCamNormal[Z]
		+ 0.5 * (rayNormal[Z] - prevRayNormal[Z])));
}

// Reshape function: adjusts view upon resize of window
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, axisLength * 2);
	glMatrixMode(GL_MODELVIEW);
}

// Main function: entry point and initialization
int main(int argc, char ** argv)
{
	// Display camera controls to command line
	printf("Direction controls (using keyboard) referenced below:\n"
		"  Left/Right Arrows -> Perform action along x axis (left/right)\n"
		"     Down/Up Arrows -> Perform action along y axis (down/up)\n"
		"       Page Down/Up -> Perform action along z axis (back/front)\n"
		"\nSimple keyboard controls:\n"
		"        Number -> Choose material/texture for new objects\n"
		"  Alt + Number -> New object at world origin\n"
		"        L      -> Load previously saved scene from file\n"
		"        M      -> Apply material/texture to selected object\n"
		"        Q/Esc  -> Quit the program\n"
		"        R      -> Reset scene (delete all objects)\n"
		"        S      -> Save current scene to file\n"
		"\nDirectional keyboard controls:\n"
		"                 Direction -> Move camera/selected light source\n"
		"         Ctrl  + Direction -> Rotate selected object\n"
		"         Alt   + Direction -> Scale selected object\n"
		"         Shift + Direction -> Translate selected object\n"
		"  Ctrl + Shift + Direction -> Move first light source\n"
		"  Alt  + Shift + Direction -> Move second light source\n"
		"\nMouse controls (anywhere on display window):\n"
		"  Left mouse click  -> Select object/light source under cursor\n"
		"  Right mouse click -> Delete object under cursor\n"
		"         Mouse drag -> Swivel camera at current distance\n");

	// Initialize global settings
	initialSettings();

	// GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);

	// Set dimensions and position of window
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - 500) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - 500) / 2);

	// Create window for displaying modeler
	glutCreateWindow("3GC3 - Assignment 4");

	// I/O function bindings
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);

	// Enable backface culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Initialize texturing
	initTexture();

	// Main program loop
	glutMainLoop();

	// Exit normally
	return 0;
}