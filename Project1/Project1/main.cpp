#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <string>

//Game Architecture Final Project - Rain Simulation
//Enoch Huang

#define MAX_PARTICLES 1000
#define WIDTH		1024
#define HEIGHT		768
#define RAIN	0
#define SNOW	1

float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
float density = 2.0;
int loop;
int fall;

//Bounce
bool bounce = false;

//floor colors
float r = 0.0;
float g = 0.7;
float b = 1.0;
float ground_points[21][21][3];
float ground_colors[21][21][4];
float accum = -10.0;

//A particle
typedef struct 
{
	// Lifespan and decay
	bool alive;
	float life;
	float fade;
				
	float red;
	float green;
	float blue;

	// Pos
	float xpos;
	float ypos;
	float zpos;

	// Y velocity
	float vel;

	// Gravity
	float gravity;
}particles;

// Particle system array of particles
particles par_sys[MAX_PARTICLES];

void controls(unsigned char key, int x, int y) 
{
	//Rain
	if (key == 'r') 
	{
		fall = RAIN;
		glutPostRedisplay();
	}
	//Snow
	if (key == 's') 
	{
		fall = SNOW;
		glutPostRedisplay();
	}
	//Slow down
	if (key == ',') 
	{
		if (slowdown > 4.0) slowdown += 0.1;
	}
	//Speed up
	if (key == '.') 
	{
		if (slowdown > 1.0) slowdown -= 0.1;
	}
	//Bounce
	if (key == 'b')
	{
		if (bounce == false)
		{
			bounce = true;
		}
		else if (bounce == true)
		{
			bounce = false;
		}
	}
	//Lower Density
	if (key == 'o')
	{
			density += 1;
	}
	//Increase Density
	if (key == 'p')
	{
		if (density >= 2) 
		{
			density -= 1;
		}
	}
	//Exit
	if (key == 'q') 
	{
		exit(0);
	}
}

void cam_control(int key, int x, int y) 
{
	if (key == GLUT_KEY_UP) 
	{
		tilt -= 1.0;
	}
	if (key == GLUT_KEY_DOWN) 
	{
		tilt += 1.0;
	}
	if (key == GLUT_KEY_RIGHT) 
	{
		pan += 1.0;
	}
	if (key == GLUT_KEY_LEFT) 
	{
		pan -= 1.0;
	}
	if (key == GLUT_KEY_PAGE_UP) 
	{
		zoom += 1.0;
	}
	if (key == GLUT_KEY_PAGE_DOWN) 
	{
		zoom -= 1.0;
	}
}

// Initialize the particles
void initParticles(int i) 
{
	par_sys[i].alive = true;
	par_sys[i].life = 1.3;
	par_sys[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

	par_sys[i].xpos = (float)(rand() % 21) - 10;
	par_sys[i].ypos = 20.0;
	par_sys[i].zpos = (float)(rand() % 21) - 10;

	par_sys[i].red = 0.5;
	par_sys[i].green = 0.5;
	par_sys[i].blue = 1.0;

	par_sys[i].vel = velocity;
	par_sys[i].gravity = -0.8;
}

//Initialize the ground and particle system
void init() 
{
	int x, z;

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);

	// Ground verts and colors
	for (z = 0; z < 21; z++) 
	{
		for (x = 0; x < 21; x++) 
		{
			ground_points[x][z][0] = x - 10.0;
			ground_points[x][z][1] = accum;
			ground_points[x][z][2] = z - 10.0;

			ground_colors[z][x][0] = r;
			ground_colors[z][x][1] = g;
			ground_colors[z][x][2] = b;
			//For accumulation
			ground_colors[z][x][3] = 0.0;
		}
	}

	// Initialize particles
	for (loop = 0; loop < MAX_PARTICLES; loop++) 
	{
		initParticles(loop);
	}
}

//Rain
void drawRain() 
{
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + density) 
	{
		//If there is a particle in the array
		if (par_sys[loop].alive == true) 
		{
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(0.5, 0.5, 1.0);
			glBegin(GL_LINES);
			glVertex3f(x, y, z);
			glVertex3f(x, y + 0.5, z);
			glEnd();

			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			//If particle reaches floor
			if (par_sys[loop].ypos <= -10) 
			{
				int zi = z - zoom + 10;
				int xi = x + 10;
				ground_colors[zi][xi][0] = 0.0;
				ground_colors[zi][xi][1] = 0.50;
				ground_colors[zi][xi][3] += 1.0;
				if (ground_colors[zi][xi][3] > 1.0) {
					ground_points[xi][zi][2] += 0.3;
				}
				if (bounce == true)
				{
					par_sys[loop].vel = par_sys[loop].vel * -1.0;
				}
				else {
					par_sys[loop].life = -1.0;
				}
			}

			// Update pos and vel of particles as they fall, can be slower if needed
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;

			//create another particle if a particle is removed
			if (par_sys[loop].life < 0.0) 
			{
				initParticles(loop);
			}
		}
	}
}
//Snow
void drawSnow() 
{
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + density) 
	{
		//If there is a particle at current position
		if (par_sys[loop].alive == true) 
		{
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();
			glTranslatef(x, y, z);
			glutSolidSphere(0.2, 16, 16);
			glPopMatrix();

			// Update pos and vel of particles as they fall, can be slower if needed
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;
			//If the snow hits the ground
			if (par_sys[loop].ypos <= -10) 
			{
				int zi = z - zoom + 10;
				int xi = x + 10;
				
				ground_colors[zi][xi][0] = 1.0;
				ground_colors[zi][xi][1] = 1.0;
				ground_colors[zi][xi][3] += 1.0;
				if (ground_colors[zi][xi][3] > 1.0) {
					ground_points[xi][zi][2] -= 0.3;
				}
				if (bounce == false)
				{
					par_sys[loop].life = -1.0;
				}
				else
				{
					par_sys[loop].vel = par_sys[loop].vel * -1.0;
				}
			}

			//create another particle if a particle is removed
			if (par_sys[loop].life < 0.0) 
			{
				initParticles(loop);
			}
		}
	}
}

void drawText(const char *text, int length, int x, int y)
{
	glMatrixMode(GL_PROJECTION);
	double *matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	for (int i = 0; i < length; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
}

// Draw Particles
void drawScene() 
{
	int i, j;
	float x, y, z;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(pan, 0.0, 1.0, 0.0);
	glRotatef(tilt, 1.0, 0.0, 0.0);

	// GROUND
	glBegin(GL_QUADS);
	//Loop along zy plane
	for (i = -10; i + 1 < 11; i++) 
	{
		//Loop along xy plane
		for (j = -10; j + 1 < 11; j++)
		{
			glColor3fv(ground_colors[i + 10][j + 10]);
			glVertex3f(ground_points[j + 10][i + 10][0],
				ground_points[j + 10][i + 10][1],
				ground_points[j + 10][i + 10][2] + zoom);
			glColor3fv(ground_colors[i + 10][j + 1 + 10]);
			glVertex3f(ground_points[j + 1 + 10][i + 10][0],
				ground_points[j + 1 + 10][i + 10][1],
				ground_points[j + 1 + 10][i + 10][2] + zoom);
			glColor3fv(ground_colors[i + 1 + 10][j + 1 + 10]);
			glVertex3f(ground_points[j + 1 + 10][i + 1 + 10][0],
				ground_points[j + 1 + 10][i + 1 + 10][1],
				ground_points[j + 1 + 10][i + 1 + 10][2] + zoom);
			glColor3fv(ground_colors[i + 1 + 10][j + 10]);
			glVertex3f(ground_points[j + 10][i + 1 + 10][0],
				ground_points[j + 10][i + 1 + 10][1],
				ground_points[j + 10][i + 1 + 10][2] + zoom);
		}

	}
	glEnd();
	//Determine what to draw
	if (fall == RAIN) {
		drawRain();
	}
	else if (fall == SNOW) {
		drawSnow();
	}
	//Control instructions
	std::string text;
	text = "Controls:";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7);
	text = "R - Rain";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7-15);
	text = "S - Snow";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 30);
	text = "B - Bounce";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 45);
	text = ", - Slow Down";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 60);
	text = ". - Speed Up";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 75);
	text = "O - Lower Density";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 100);
	text = "P - Increase Density";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 115);
	text = "RIGHT - Pan Right";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 130);
	text = "LEFT - Pan left";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 145);
	text = "UP - Pan up";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 160);
	text = "DOWN - Pan Down";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 175);
	text = "PgUp - Zoom In";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 200);
	text = "PgDn - Zoom Out";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 215);
	text = "Q - Quit";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 230);

	glutSwapBuffers();
}

//Resize to fit window
void reshape(int w, int h) 
{
	if (h == 0) h = 1;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45, (float)w / (float)h, .1, 200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void idle() {
	//marks current window as to be redisplayed
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("GA Final Project Enoch Huang");
	init();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(controls);
	glutSpecialFunc(cam_control);
	//mark as to be redisplayed for next iteration of the loop
	glutIdleFunc(idle);
	//enter event processing loop
	glutMainLoop();
	return 0;
}