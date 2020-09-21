// Standard C++ library imports
#include <assert.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Import mathLib2D library
#include "mathLib2D.h"

// Global variables for passed/failed testcases
static int testsPassed = 0;
static int testsFailed = 0;
static int totalTests = 0;

// Display name and reset count at start of test batch
void beginTestBatch(const char *name)
{
	printf("Testing %s: ", (char *)name);
	testsPassed = 0;
	totalTests = 0;
}

// Check for expected result and update count
void runTest(float result, float expected)
{
	if (result == expected)
		testsPassed++;
	else
		testsFailed++;
	totalTests++;
}

// Display summary at end of test batch
void endTestBatch()
{
	printf("%d out of %d tests passed.\n", testsPassed, totalTests);
}

// Main method for testing and demoing mathLib2D library
int main()
{
	// Initialize variables allocated on stack (except arrPoint2D)
	Point2D p1 = Point2D(2, 3);
	Point2D p2 = Point2D(5, 7);
	Point2D p3 = Point2D();
	Vec2D v1 = Vec2D(3, 4);
	Vec2D v2 = Vec2D(7, 24);
	Vec2D v3 = Vec2D();
	int numPoint2D;
	int numVec2D;
	Point2D * arrPoint2D;
	std::vector<Vec2D> vecVec2D;

	// Test Point2D::Point2D
	beginTestBatch("Point2D::Point2D");
	runTest(p1.mX, 2);
	runTest(p1.mY, 3);
	runTest(p2.mX, 5);
	runTest(p2.mY, 7);
	runTest(p3.mX, 0);
	runTest(p3.mY, 0);
	endTestBatch();

	// Test Point2D::distanceTo
	beginTestBatch("Point2D::distanceTo");
	runTest(p1.distanceTo(p2), 5);
	runTest(p2.distanceTo(p3), (float)sqrt(74));
	runTest(p3.distanceTo(p1), (float)sqrt(13));
	endTestBatch();

	// Test Point2D::fastDistanceTo
	beginTestBatch("Point2D::fastDistanceTo");
	runTest(p1.fastDistanceTo(p2), 25);
	runTest(p2.fastDistanceTo(p3), 74);
	runTest(p3.fastDistanceTo(p1), 13);
	endTestBatch();

	// Test Vec2D::Vec2D
	beginTestBatch("Vec2D::Vec2D");
	runTest(v1.mX, 3);
	runTest(v1.mY, 4);
	runTest(v2.mX, 7);
	runTest(v2.mY, 24);
	runTest(v3.mX, 0);
	runTest(v3.mY, 0);
	endTestBatch();

	// Test Vec2D::length
	beginTestBatch("Vec2D::length");
	runTest(v1.length(), 5);
	runTest(v2.length(), 25);
	runTest(v3.length(), 0);
	endTestBatch();

	// Test Vec2D::normalize
	beginTestBatch("Vec2D::normalize");
	runTest(v1.normalize().mX, (float)0.6);
	runTest(v1.normalize().mY, (float)0.8);
	runTest(v2.normalize().mX, (float)0.28);
	runTest(v2.normalize().mY, (float)0.96);
	runTest(v3.normalize().mX, (float)0);
	runTest(v3.normalize().mY, (float)0);
	endTestBatch();

	// Test Vec2D::multiply
	beginTestBatch("Vec2D::multiply");
	runTest(v1.multiply(2).mX, 6);
	runTest(v1.multiply(2).mY, 8);
	runTest(v2.multiply(3).mX, 21);
	runTest(v2.multiply(3).mY, 72);
	runTest(v3.multiply(42).mX, 0);
	runTest(v3.multiply(42).mY, 0);
	endTestBatch();

	// Test Vec2D::movePoint
	beginTestBatch("Vec2D::movePoint");
	runTest(v1.movePoint(p1).mX, 5);
	runTest(v1.movePoint(p1).mY, 7);
	runTest(v2.movePoint(p1).mX, 9);
	runTest(v2.movePoint(p1).mY, 27);
	runTest(v3.movePoint(p1).mX, 2);
	runTest(v3.movePoint(p1).mY, 3);
	runTest(v1.movePoint(p2).mX, 8);
	runTest(v1.movePoint(p2).mY, 11);
	runTest(v2.movePoint(p2).mX, 12);
	runTest(v2.movePoint(p2).mY, 31);
	runTest(v3.movePoint(p2).mX, 5);
	runTest(v3.movePoint(p2).mY, 7);
	runTest(v1.movePoint(p3).mX, 3);
	runTest(v1.movePoint(p3).mY, 4);
	runTest(v2.movePoint(p3).mX, 7);
	runTest(v2.movePoint(p3).mY, 24);
	runTest(v3.movePoint(p3).mX, 0);
	runTest(v3.movePoint(p3).mY, 0);
	endTestBatch();

	// Test Vec2D::createVector
	beginTestBatch("Vec2D::createVector");
	runTest(Vec2D::createVector(p1, p2).mX, 3);
	runTest(Vec2D::createVector(p1, p2).mY, 4);
	runTest(Vec2D::createVector(p2, p3).mX, -5);
	runTest(Vec2D::createVector(p2, p3).mY, -7);
	runTest(Vec2D::createVector(p3, p1).mX, 2);
	runTest(Vec2D::createVector(p3, p1).mY, 3);
	endTestBatch();

	// Display confirmation that assertions pass
	if (testsFailed)
		printf("%d TEST(S) FAILED.\n", testsFailed);
	else
		printf("ALL UNIT TESTS PASS.\n");

	// Populate Point2D array with user-given size
	std::cout << "Enter size of Point2D array: ";
	std::cin >> numPoint2D;
	arrPoint2D = new Point2D[numPoint2D];
	for (int i = 0; i < numPoint2D; i++)
		arrPoint2D[i] =
			Point2D((float)rand() / (float)RAND_MAX * 100,
					(float)rand() / (float)RAND_MAX * 100);
	for (int i = 0; i < numPoint2D; i++)
		printf("Point2D(%.3f, %.3f)\n",
			arrPoint2D[i].mX, arrPoint2D[i].mY);

	// Populate Vec2D array with user-given size
	std::cout << "Enter size of Vec2D vector: ";
	std::cin >> numVec2D;
	for (int i = 0; i < numVec2D; i++)
		vecVec2D.push_back(
			Vec2D((float)rand() / (float)RAND_MAX * 100,
				  (float)rand() / (float)RAND_MAX * 100));
	for (int i = 0; i < numVec2D; i++)
		printf("Vec2D(%.3f, %.3f)\n",
			vecVec2D.at(i).mX, vecVec2D.at(i).mY);

	// De-allocate memory allocated dynamically
	delete [] arrPoint2D;

	// Exit normally
	return 0;
}