/*****************************************************************************
		FILE : submit.c (Assignment 1)
		NOTE : you have to implement functions in this file
*****************************************************************************/
/*****************************************************************************
		Student Information
		Student ID:nan
		Student Name:ValKmjolnir
*****************************************************************************/

#include <stdlib.h>
#include <ctime>
#include <GL/glut.h>

struct point
{
	GLdouble x;
	GLdouble y;
	GLdouble z;
};
point camera_place = { 0.0,0.0,-30.0 }; // camera place
point item_place = { 0.0,0.0,100.0 }; // item place that we look at
point headtop_orient = { 0.0,1.0,0.0 };

// set light
GLfloat light_position[4] = { 0.0,99.0,100.0,1 };
GLfloat ambientlight[4] = { 0.6,0.6,0.6,0.6 };
GLfloat diffuselight[4] = { 0.6,0.6,0.6,0.6 };
GLfloat specularlight[4] = { 0.6,0.6,0.6,0.6 };

// set material
GLfloat mat_ambient[4] = { 0.0,0.0,0.0,1.0 };
GLfloat mat_diffuse[4] = { 0.6,0.6,0.6,0.6 };
GLfloat mat_specular[4] = { 0.2,0.2,0.2,0.8 };
GLfloat mat_shininess[1] = { 5.0 };

// rotate the lamp
bool set_rotate_positive = false;
bool set_rotate_negative = false;
GLfloat lamp_angle = 0;

// move the teapot
bool move_teapot = false;
bool inc_teapot_place = true;
GLfloat teapot_place = 0;

// set model size on the desk
bool change_size = false;
bool change_larger = false;
bool change_smaller = false;
GLfloat model_scale = 1.0;

// generate the drawing on the wall
class game_of_life
{
private:
	bool map[60][80];
	bool get_map(int x, int y)
	{
		if (x < 0) x = 59;
		if (x > 59)x = 0;
		if (y < 0)y = 79;
		if (y > 79)y = 0;
		return map[x][y];
	}
public:
	game_of_life()
	{
		srand(unsigned(time(NULL)));
		for(int i=0; i<60; ++i)
			for(int j=0; j<80; ++j)
				map[i][j] = (rand()%100)<30;
		return;
	}
	void reset()
	{
		for (int i = 0; i < 60; ++i)
			for (int j = 0; j < 80; ++j)
				map[i][j] = (rand()%100)<30;
		return;
	}
	void draw()
	{
		for(int i=0; i<60; ++i)
			for(int j=0; j<80; ++j)
				if (map[i][j])
				{
					glBegin(GL_QUADS);
					glColor3f(0.0, 0.5, 0.4);
					glVertex3f(-99.8, 80-j*2.0, 40+i*2.0);
					glVertex3f(-99.8, 80-j*2.0, 42+i*2.0);
					glVertex3f(-99.8, 78-j*2.0, 42+i*2.0);
					glVertex3f(-99.8, 78-j*2.0, 40+i*2.0);
					glEnd();
				}
		return;
	}
	void next_step()
	{
		int oprt[8][2] = {{0,1},{0,-1},{1,0},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1}};
		bool tmp[60][80];
		for(int i=0; i<60; ++i)
			for (int j=0; j<80; ++j)
			{
				int cnt = 0;
				for (int k = 0; k < 8; ++k)
					cnt += get_map(i + oprt[k][0], j + oprt[k][1]);
				if (cnt == 3)
					tmp[i][j] = true;
				else if (cnt == 2)
					tmp[i][j] = map[i][j];
				else
					tmp[i][j] = false;
			}
		for (int i=0; i<60; ++i)
			for (int j=0; j<80; ++j)
				map[i][j] = tmp[i][j];
		return;
	}
};
game_of_life game;

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

void light_source()
{
	// 光位置
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	// 模糊光
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientlight);
	// 漫反射光
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuselight);
	// 镜面光
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularlight);

	// 材质模糊光
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	// 材质反射光
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	// 材质镜面光
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	// 材质镜面指数
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
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
		glColor3f(0.0, 0.9, 0.95);
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

void generate_solar_lamp()
{
	GLUquadric* pobj=gluNewQuadric();

	// solar
	glPushMatrix();
	glTranslatef(0.0, 100.0, 100.0);
	glColor3f(0.0, 0.0, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	gluCylinder(pobj, 1.0, 0.2, 20.0, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 80.0, 100.0);
	glColor3f(1.0, 1.0, 0.0);
	glutSolidSphere(6, 100, 100);
	glPopMatrix();

	// first planet
	glPushMatrix();
	glTranslatef(0.0, 80.0, 100.0);
	glRotatef(4*lamp_angle, 0.0, 1.0, 0.0);
	glColor3f(0.0, 0.0, 0.0);
	glTranslatef(-10.0, 0.0, 0.0);
	glRotatef(90, 0.0, 1.0, 0.0);
	gluCylinder(pobj,0.2,0.2,10.0,32,5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 80.0, 100.0);
	glRotatef(4*lamp_angle, 0.0, 1.0, 0.0);
	glColor3f(0.0, 0.6, 0.6);
	glTranslatef(-10.0, 0.0, 0.0);
	glutSolidSphere(2, 100, 100);
	glPopMatrix();
	
	// second planet
	glPushMatrix();
	glTranslatef(0.0, 80.0, 100.0);
	glRotatef(2*lamp_angle, 0.0, 1.0, 0.0);
	glColor3f(0.5, 0.0, 0.0);
	glTranslatef(25.0, 0.0, 0.0);
	glRotatef(-90, 0.0, 1.0, 0.0);
	gluCylinder(pobj, 0.2, 0.2, 25.0, 32, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 80.0, 100.0);
	glRotatef(2*lamp_angle, 0.0, 1.0, 0.0);
	glColor3f(0.5, 0.0, 0.0);
	glTranslatef(25.0, 0.0, 0.0);
	glutSolidSphere(3, 100, 100);
	glPopMatrix();

	// third planet
	glPushMatrix();
	glTranslatef(0.0, 80.0, 100.0);
	glRotatef(2.5*lamp_angle, 0.0, 1.0, 0.0);
	glColor3f(0.5, 0.5, 0.5);
	glTranslatef(20.0, 0.0, 0.0);
	glRotatef(-90, 0.0, 1.0, 0.0);
	gluCylinder(pobj, 0.2, 0.2, 20.0, 32, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 80.0, 100.0);
	glRotatef(2.5*lamp_angle, 0.0, 1.0, 0.0);
	glColor3f(0.5, 0.5, 0.5);
	glTranslatef(20.0, 0.0, 0.0);
	glutSolidTeapot(2);
	glPopMatrix();

	// forth planet
	glPushMatrix();
	glTranslatef(0.0, 80.0, 100.0);
	glRotatef(lamp_angle/2.0, 0.0, 1.0, 0.0);
	glColor3f(0.5, 0.0, 0.5);
	glTranslatef(-30.0, 0.0, 0.0);
	glRotatef(90, 0.0, 1.0, 0.0);
	gluCylinder(pobj, 0.2, 0.2, 30.0, 32, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 80.0, 100.0);
	glRotatef(lamp_angle/2.0, 0.0, 1.0, 0.0);
	glColor3f(0.5, 0.0, 0.5);
	glTranslatef(-30.0, 0.0, 0.0);
	glutSolidSphere(2, 100, 100);
	glPopMatrix();

	gluDeleteQuadric(pobj);
	return;
}

void generate_bed()
{
	glPushMatrix();
	glTranslatef(60.0, -90.0, 130.0);
	glScalef(8.0, 0.5, 14.0);
	glColor3f(0.5, 0.5, 0.5);
	glutSolidCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(22.5, -95.0, 62.5);
	glScalef(1.0, 2.0, 1.0);
	glColor3f(0.5, 0.5, 0.5);
	glutSolidCube(5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(22.5, -95.0, 197.5);
	glScalef(1.0, 2.0, 1.0);
	glColor3f(0.5, 0.5, 0.5);
	glutSolidCube(5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(97.5, -95.0, 197.5);
	glScalef(1.0, 2.0, 1.0);
	glColor3f(0.5, 0.5, 0.5);
	glutSolidCube(5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(97.5, -95.0, 62.5);
	glScalef(1.0, 2.0, 1.0);
	glColor3f(0.5, 0.5, 0.5);
	glutSolidCube(5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(60.0, -87.5, 130.0);
	glScalef(8.0, 0.5, 14.0);
	glColor3f(0.6, 0.3, 0.1);
	glutSolidCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(60.0, -80, 180.0);
	glScalef(5.0, 1.0, 1.5);
	glColor3f(0.7, 0.7, 0.7);
	glutSolidCube(10);
	glPopMatrix();
	return;
}

void generate_bookshelf()
{
	for (GLfloat y = -10; y < 80; y += 30)
	{
		glPushMatrix();
		glTranslatef(85.0, y, 130.0);
		glScalef(3.0, 0.5, 14.0);
		glColor3f(0.3, 0.3, 0.2);
		glutSolidCube(10);
		glPopMatrix();
	}

	for (GLfloat y = -10; y < 50; y += 30)
	{
		glPushMatrix();
		glTranslatef(30.0, y, 185.0);
		glScalef(14.0, 0.5, 3.0);
		glColor3f(0.3, 0.3, 0.2);
		glutSolidCube(10);
		glPopMatrix();
	}
	// put cone
	for (GLfloat x = -20.0; x <= 80; x += 20)
	{
		glPushMatrix();
		glTranslatef(x, 20.0, 185.0);
		glRotatef(-90, 1.0, 0.0, 0.0);
		glColor3f(0.0, 0.3+(x/100.0), 0.3);
		glutSolidCone(10, 20, 50, 50);
		glPopMatrix();
	}

	// put books
	GLfloat book_height0[20] = {15,20,18.5,19,17,20,15,14,20,19,18,16,17,17,19,16,20,18,15,14};
	for (int i = 0; i < 20; ++i)
	{
		glPushMatrix();
		glTranslatef(90.0, 22+book_height0[i]/2.0, 62+i*2.0);
		glScalef(2.0, book_height0[i]/10.0, 0.2);
		glColor3f(0.0+book_height0[i]/25.0, 0.3+book_height0[i]/25.0, 0.2+book_height0[i]/25.0);
		glutSolidCube(10);
		glPopMatrix();
	}
	GLfloat book_height1[25] = { 16,21,19.5,18,18,19,14,15,20,19.4,18.2,16.5,17.6,15,17,15,15,19,20,18.4,13.2,14.0,15.6,16.5,17.1};
	for (int i = 0; i < 25; ++i)
	{
		glPushMatrix();
		glTranslatef(90.0, 52+book_height1[i]/2.0, 92+i*2.0);
		glScalef(2.0, book_height1[i]/10.0, 0.2);
		glColor3f(0.3+book_height1[i]/25.0, 0.2+book_height1[i]/25.0, book_height1[i]/25.0);
		glutSolidCube(10);
		glPopMatrix();
	}

	// put teapot
	glPushMatrix();
	glTranslatef(85.0, 0, 195.0-teapot_place);
	glColor3f(0.6, 0.6, 0.2);
	glutSolidTeapot(8);
	glPopMatrix();
	return;
}

void generate_drawing()
{
	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(-99.9, -80, 40);
	glVertex3f(-99.9, 80, 40);
	glVertex3f(-99.9, 80, 160);
	glVertex3f(-99.9, -80, 160);
	glEnd();
	glPopMatrix();

	game.draw();
	return;
}

void generate_desk()
{
	GLUquadric* pobj = gluNewQuadric();

	glPushMatrix();
	glTranslatef(-25.0, -100.0, 150.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glColor3f(0.2, 0.5, 0.4);
	gluCylinder(pobj, 20.0, 5.0, 5, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-25.0, -95.0, 150.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glColor3f(0.1, 0.4, 0.4);
	gluCylinder(pobj, 5.0, 5.0, 40, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-25.0, -55.0, 150.0);
	glColor3f(0.4, 0.4, 0.4);
	glScalef(30, 1, 30);
	glutSolidCube(2);
	glPopMatrix();

	// model
	glPushMatrix();
	glTranslatef(-35.0, -53.0, 155.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glColor3f(0.0, 0.4, 0.4);
	glScalef(model_scale,model_scale,model_scale);
	glutSolidCone(10,10,50,50);
	glPopMatrix();

	gluDeleteQuadric(pobj);
	return;
}

void display(void) // Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// TODO:
	// Place light source here
	light_source();

	// TODO:
	// Draw walls and objects here
	generate_wall();
	generate_floor_board();
	generate_solar_lamp();
	generate_bed();
	generate_bookshelf();
	generate_drawing();
	generate_desk();

	glutSwapBuffers();
	glFlush();
	glutPostRedisplay();
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
	int i;
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
			camera_place.z = -30.0;
			break;
		case '-':
			if(ambientlight[0]>0)
				for (i = 0; i < 4; ++i)
				{
					ambientlight[i] -= 0.1;
					diffuselight[i] -= 0.1;
					specularlight[i] -= 0.1;
				}
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHTING);
			break;
		case '=':
			if(ambientlight[0]<0.8)
				for (i = 0; i < 4; ++i)
				{
					ambientlight[i] += 0.1;
					diffuselight[i] += 0.1;
					specularlight[i] += 0.1;
				}
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHTING);
			break;
		case '1':
			set_rotate_positive = true;
			set_rotate_negative = false;
			break;
		case '2':
			set_rotate_positive = false;
			set_rotate_negative = true;
			break;
		case '3':
			set_rotate_positive = false;
			set_rotate_negative = false;
			break;
		case 'l':
			if (ambientlight[0] > 0)
				for (i = 0; i < 4; ++i)
				{
					ambientlight[i] = 0;
					diffuselight[i] = 0;
					specularlight[i] = 0;
				}
			else
				for (i = 0; i < 4; ++i)
				{
					ambientlight[i] = 0.8;
					diffuselight[i] = 0.8;
					specularlight[i] = 0.8;
				}
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHTING);
			break;
		case 'm':
			move_teapot = !move_teapot;
			break;
		case 'n':
			game.next_step();
			break;
		case 'b':
			game.reset();
			break;
		case 'c':
			change_size = !change_size;
			break;
	}
	reshape(wd.w, wd.h);// change camera place by x and y
	glutPostRedisplay();// redisplay things after moving camera
	return;
}

void idle()
{
	if (set_rotate_positive)
		lamp_angle-=1;
	if (set_rotate_negative)
		lamp_angle+=1;
	if (move_teapot)
	{
		if (inc_teapot_place)
			teapot_place -= 1;
		else
			teapot_place += 1;
		if (teapot_place >= 140)
			inc_teapot_place = true;
		if (teapot_place <= 0)
			inc_teapot_place = false;
	}
	if (change_size)
	{
		if (model_scale <= 1.0)
		{
			change_smaller = false;
			change_larger = true;
		}
		if (model_scale >= 2.0)
		{
			change_smaller = true;
			change_larger = false;
		}
		if (change_smaller)
			model_scale -= 0.025;
		if (change_larger)
			model_scale += 0.025;
	}
	return;
}


int main(int argc, char** argv)
{

	/*Initialization of GLUT Library */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	/*Create a window with title specified */
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Assignment 1");
	
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
