﻿/*****************************************************************************
		FILE : submit.c (Assignment 1)
		NOTE : you have to implement functions in this file
*****************************************************************************/
/*****************************************************************************
		Student Information
		Student ID:nan
		Student Name:ValKmjolnir
*****************************************************************************/

#include <stdlib.h>
#include <GL/glut.h>

struct point
{
	GLdouble x;
	GLdouble y;
	GLdouble z;
};
point camera_place = { 0.0,0.0,0.0 }; // camera place
point item_place = { 0.0,0.0,100.0 }; // item place that we look at
point headtop_orient = { 0.0,1.0,0.0 };

// window_size used in reshape
struct window_size
{
	int w;
	int h;
};
window_size wd = { 800,600 };

void init(void) // All Setup For OpenGL Goes Here
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL); // output correct color
	glEnable(GLUT_MULTISAMPLE);  // use multisampling
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	return;
}

void generate_floor_board()
{
	glPushMatrix();
	for (double z=0;z<200;z+=20)
	{
		glBegin(GL_QUADS);
		glColor3f(0.6, 0.2, 0.0);
		glVertex3f(100, -100+0.1, z);
		glVertex3f(-100, -100+0.1, z);
		glVertex3f(-100, -100+0.1, z+0.8);
		glVertex3f(100, -100+0.1, z+0.8);
		glEnd();
	}
	glPopMatrix();
	return;
}

void generate_wall()
{
	glPushMatrix();
	// when using glVertex3f,we must use it in order,to make sure that the curve we'll draw is correct.
	// back wall
	glBegin(GL_QUADS);
		glColor3f(0.9, 0.8, 0.7);
		glVertex3f(100, 100, 200);
		glVertex3f(100, -100, 200);
		glVertex3f(-100,-100, 200);
		glVertex3f(-100, 100, 200);
	glEnd();
	// ceiling
	glBegin(GL_QUADS);
		glColor3f(0.0, 10.0, 10.5);
		glVertex3f(100, 100, 200);
		glVertex3f(100, 100, 0);
		glVertex3f(-100, 100, 0);
		glVertex3f(-100, 100, 200);
	glEnd();
	// floor
	glBegin(GL_QUADS);
		glColor3f(0.9, 0.5, 0.0);
		glVertex3f(100, -100, 200);
		glColor3f(0.8, 0.4, 0.0);
		glVertex3f(100, -100, 0);
		glColor3f(0.7, 0.3, 0.0);
		glVertex3f(-100, -100, 0);
		glColor3f(0.6, 0.2, 0.0);
		glVertex3f(-100, -100, 200);
	glEnd();

	// left and right wall
	glBegin(GL_QUADS);
		glColor3f(0.2, 0.4, 0.5);
		glVertex3f(100, -100, 0);
		glVertex3f(100, 100, 0);
		glVertex3f(100, 100, 200);
		glVertex3f(100, -100, 200);
	glEnd();
	glBegin(GL_QUADS);
		glColor3f(0.2, 0.4, 0.5);
		glVertex3f(-100, -100, 0);
		glVertex3f(-100, 100, 0);
		glVertex3f(-100, 100, 200);
		glVertex3f(-100, -100, 200);
	glEnd();

	glPopMatrix();
	return;
}

void generate_teaport()
{
	glPushMatrix();
	glColor3f(10.0, 10.0, 0.0);
	glTranslatef(0.0, 0.0, 100.0);
	glutWireTeapot(10);
	glPopMatrix();
	return;
}

void display(void) // Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// TODO:
	// Place light source here

	// TODO:
	// Draw walls and objects here
	generate_wall();
	generate_floor_board();
	generate_teaport();

	// TODO:
	// Add animation here

	glutSwapBuffers();
	glFlush();
	return;
}


void reshape(int w, int h) // Resize the GL Window. w=width, h=height
{
	wd.w = w;
	wd.h = (h ? h : 1);
	glViewport(0, 0, (GLsizei)w, (GLsizei)(h ? h : 1));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// set camera
	// w/h use float to avoid SIGFPE
	gluPerspective(100.0, (GLfloat)w/(GLfloat)h, 0.1, 1000.0);
	gluLookAt(
		camera_place.x, camera_place.y, camera_place.z,
		item_place.x, item_place.y, item_place.z,
		headtop_orient.x, headtop_orient.y, headtop_orient.z
	);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return;
}

void keyboard(unsigned char key, int x, int y) // Handle the keyboard events here
{
	// wasd move camera place
	switch (key)
	{
		case'\033'://press 'esc' to quit
			exit(0);
			break;
		case 'w':
			camera_place.z += 1;
			break;
		case 's':
			camera_place.z -= 1;
			break;
		case 'a':
			camera_place.x += 1;
			break;
		case 'd':
			camera_place.x -= 1;
			break;
		case 'r':
			camera_place.x = 0.0;
			camera_place.y = 0.0;
			camera_place.z = 0.0;
			break;

		// TODO:
		// Add keyboard control here

	}
	reshape(wd.w, wd.h);// change camera place by x and y
	glutPostRedisplay();// redisplay things after moving camera
	return;
}

void idle()
{
	return;
}


int main(int argc, char** argv)
{

	/*Initialization of GLUT Library */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	/*Create a window with title specified */
	glutCreateWindow("Assignment 1");
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);

	init();	/*not GLUT call, initialize several parameters */

	/*Register different CALLBACK function for GLUT to response
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	/*Enter the GLUT event processing loop which never returns.
	it will call different registered CALLBACK according
	to different events. */
	glutMainLoop();
	return 0;
}