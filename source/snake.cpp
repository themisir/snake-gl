#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <windows.h>
#include <GL/glut.h>
#include <chrono>
#include <string>

using namespace std::chrono;

#include "snake.h"

static void AI()
{
	float x = snake->x;
	float y = snake->y;

	bool goV = false;
	bool goH = false;

#ifndef BORDERLESS
	if ((x == 49 && vx == 1) || (x == 0 && vx == -1)) {
		goH = true;
	}
	else
	if ((y == 49 && vy == 1) || (y == 0 && vy == -1)) {
		goV = true;
	}
	else
#endif
	if (x == food->x) {
		goH = true;
	}
	else
	if (y == food->y) {
		goV = true;
	}


	if (goH)
	{
		bool go;

		if (food->y > y) {
			if (InTrails(x, y + 1) == 0) {
				go = true;
			}
			else
			{
				go = false;
			}
		}
		else
		{
			if (InTrails(x, y - 1) == 0) {
				go = false;
			}
			else
			{
				go = true;
			}
		}

		Rotate(go ? bottom : top);
	}
	else
	if (goV)
	{
		bool go;

		if (food->x > x) {
			if (IsSafe(x + 1, y)) {
				go = true;
			}
			else
			{
				go = false;
			}
		}
		else
		{
			if (IsSafe(x - 1, y)) {
				go = false;
			}
			else
			{
				go = true;
			}
		}

		Rotate(go ? right : left);
	}


	if (int i = InTrails(x + vx, y + vy)) {

		try {
			
			Vector2* p1 = trail[i];
			Vector2* p0 = trail[i - 1];

			float nvx = p1->x - p0->x;
			float nvy = p1->y - p0->y;
			if (vx == 0 && nvx != 0) {
				vy = 0;
				vx = nvx;
			}
			else
			if (vy == 0 && nvy != 0) {
				vx = 0;
				vy = nvy;
			}


		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
		}

	}

	if (!IsSafe(x + vx, y + vy))
	{
		if (vx == 0) {
			vy = 0;
			vx = IsSafe(x + 1, y) ? 1 : -1;
		}
		else
		if (vy == 0) {
			vx = 0;
			vy = IsSafe(x, y + 1) ? 1 : -1;
		}
	}
}

static void GameLoop()
{
	long now = TimeStamp();
	long delta = now - lasttime;
	
	if (delta > FRAMESIZE) {
		lasttime = now;

		AI();
		Move();

		if (!IsSafe(snake->x, snake->y)) {
			std::string title = "score: " + std::to_string(trailLength - 3);
			std::cout << title << std::endl;

			Sleep(5000);
			ResetSnake();
		}

		glutPostRedisplay();
	}


	std::string title = "score: " + std::to_string(trailLength - 3);
	glutSetWindowTitle(title.c_str());
}

static long TimeStamp()
{
	milliseconds ms = duration_cast< milliseconds >(
		system_clock::now().time_since_epoch()
	);
	return ms.count();
}

static bool IsSafe(float x, float y)
{
#ifdef BORDERLESS
	if (x < 0) x += 50;
	if (y < 0) y += 50;
	if (x > 49) x -= 50;
	if (y > 49) y -= 50;

	return InTrails(x, y) == 0;
#else
	return (InTrails(x, y) == 0) && (x >= 0) && (y >= 0) &&
		(x < 50) && (y < 50);
#endif
}

static int InTrails(float x, float y)
{

	for (int i = 0; i < trailLength; i++) {
		if (trail[i]->x == x && trail[i]->y == y) {
			return i;
		}
	}

	return false;
}

static void RespawnFood()
{
	bool repeat = true;

	while (repeat) {
		food->x = rand() % 49 + 1;
		food->y = rand() % 49 + 1;
		repeat = InTrails(food->x, food->y);
	}
}

static void Eat(float x, float y)
{
	Vector2** newTrail = new Vector2*[trailLength = trailLength + 1];

	for (int i = 0; i < trailLength - 1; i++) {
		newTrail[i] = new Vector2(trail[i]->x, trail[i]->y);
	}
	newTrail[trailLength - 1] = new Vector2(x, y);

	free(trail);
	trail = newTrail;

	RespawnFood();

}

static void ResetSnake()
{
	free(snake);
	snake = new Vector2(25.0f, 25.0f);

	free(trail);
	trail = new Vector2*[trailLength = 3];
	trail[0] = new Vector2(snake->x - 1, snake->y);
	trail[1] = new Vector2(snake->x - 2, snake->y);
	trail[2] = new Vector2(snake->x - 3, snake->y);

	vx = 1;
	vy = 0;
}

static void Move()
{
	if (vx == 0 && vy == 0) return;

	float lx = trail[trailLength - 1]->x;
	float ly = trail[trailLength - 1]->y;

	for (int i = trailLength-1; i > 0; i--) {
		trail[i]->set(trail[i - 1]->x, trail[i - 1]->y);
	}
	trail[0]->set(snake->x, snake->y);

	snake->x += vx;
	snake->y += vy;

#ifdef BORDERLESS
	if (snake->x < 0) snake->x += 50;
	if (snake->y < 0) snake->y += 50;
	if (snake->x > 49) snake->x -= 50;
	if (snake->y > 49) snake->y -= 50;
#endif

	if (food->x == snake->x && food->y == snake->y) {
		Eat(lx, ly);
	}
}

static void Rotate(LookAt look)
{
	float nvx = look == left ? -1 : (look == right ? 1 : 0);
	float nvy = look == top ? -1 : (look == bottom ? 1 : 0);

	if (vx == 0 && nvx != 0) {
		vy = 0;
		vx = nvx;
	}
	else
	if (vy == 0 && nvy != 0) {
		vx = 0;
		vy = nvy;
	}
}


static void Init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	ResetSnake();
}

static void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -10, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT or GL_DEPTH_BUFFER_BIT);
}


static void DrawRect(float x, float y, float w, float h)
{
	GLfloat left = x * CELLSIZE;
	GLfloat top = y * CELLSIZE;
	GLfloat right = left + CELLSIZE*w;
	GLfloat bottom = top + CELLSIZE*h;

	glBegin(GL_QUADS);
	{
		glVertex2f(left, top);
		glVertex2f(right, top);
		glVertex2f(right, bottom);
		glVertex2f(left, bottom);
	}
	glEnd();
}


static void Draw(void)
{
	glPushMatrix();
	glTranslatef(0, 0, 0.0f);

	glColor3f(0, 0, 0);
	DrawRect(0, 0, 50, 50);

	glColor3f(0, 1, 0);
	DrawRect(snake->x, snake->y, 1, 1);

	glColor3f(0, 0.7, 0);
	for (int i = 0; i < trailLength; i++) {
		DrawRect(trail[i]->x, trail[i]->y, 1, 1);
	}

	glColor3f(1, 0, 0);
	DrawRect(food->x, food->y, 1, 1);

	glPopMatrix();
	glFlush();
}

int main(int argc, char **argv)
{
	srand(time(0));

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_ACCUM | GLUT_SINGLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Snike");

	Init();

	glutReshapeFunc(Reshape);
	glutIdleFunc(GameLoop);
	glutDisplayFunc(Draw);

	glutMainLoop();
}