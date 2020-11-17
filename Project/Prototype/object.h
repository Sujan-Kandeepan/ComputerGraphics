#ifndef OBJECT_H
#define OBJECT_H

// Enum for easier/safer polymorphic type check
enum ObjectType { ROCKET, COIN, OBSTACLE, BOMB };

// Generic object (coordinates, type specfier, obj loader logic)
class Object
{
public:
	float positionX;
	float positionY;
	float positionZ;
	ObjectType type;
	Object(float x = 0, float y = 0, float z = 0);
	void render(float r, float g, float b, float s);
};

// Rocket (always in foreground)
class Rocket : public Object
{
public:
	Rocket(float x = 0, float y = 0, float z = 0);
	void render();
};

// Coin (boost player score)
class Coin : public Object
{
public:
	Coin(float x = 0, float y = 0, float z = 0);
	void render();
};

// Obstacle (reduce player score)
class Obstacle : public Object
{
public:
	Obstacle(float x = 0, float y = 0, float z = 0);
	void render();
};

// Bomb (game over immediately)
class Bomb : public Object
{
public:
	Bomb(float x = 0, float y = 0, float z = 0);
	void render();
};

#endif