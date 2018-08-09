#pragma once

struct Vector2 {
	GLfloat x;
	GLfloat y;

	void set(GLfloat nx, GLfloat ny) {
		x = nx;
		y = ny;
	}
	Vector2(GLfloat nx, GLfloat ny) {
		x = nx;
		y = ny;
	}
	Vector2() {}
};

enum LookAt { left, top, right, bottom };

Vector2 **trail;
int trailLength = 0;
float vx = 1;
float vy = 0;
Vector2 *snake = new Vector2(0, 0);
Vector2 *food = new Vector2(0, 0);

const GLfloat CELLSIZE = 10.0f;

static void ResetSnake();
static void Move();
static void Rotate(LookAt look);
static long TimeStamp();
static int InTrails(float x, float y);
static void RespawnFood();
static bool IsSafe(float x, float y);

static long lasttime = TimeStamp();

#define FRAMESIZE 10
// #define BORDERLESS  /* To play borderless mode uncomment this line */