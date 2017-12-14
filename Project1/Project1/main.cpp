#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <string>

//Computer Graphics Final Project: Precipitation Simulation
//Enoch Huang, Ben Gruber, Igor Carvalho

//sets the definition for all
#define MAX_PARTICLES	1000
#define MAX_VELOCITY	-150
#define WIDTH	1024
#define HEIGHT	768
#define RAIN	0
#define SNOW	1
#define HAIL	2

float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
float density = 2.0;
float gravity = -0.8;
int loop;
int fall;
bool leftWind = false;
bool rightWind = false;
bool noWind = true;
float windfactor = 0.0f;


//Bounce
bool bounce = false;

//floor colors
float r = 0.0;
float g = 0.2;
float b = 0.5;
float ground_points[50][50][3];
float ground_colors[50][50][4];
float accum = -10.0;




//A particle
typedef struct
{
	// Lifespan
	bool alive;
	float life;
	float size;

	float red;
	float green;
	float blue;

	// Pos
	float xpos;
	float ypos;
	float zpos;

	// Y velocity
	float vel;
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
	//Hail
	if (key == 'h')
	{
		fall = HAIL;
		glutPostRedisplay();
	}
	//Slow down
	if (key == ',')
	{
		if (slowdown < 4.0) slowdown += 0.1;
		printf("%f\n", slowdown);
	}
	//Speed up
	if (key == '.')
	{
		if (slowdown > 1.0) slowdown -= 0.1;
		printf("%f\n", slowdown);
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

	//change wind direction
	if (key == 'j')
	{
		leftWind = true;
		rightWind = false;
		noWind = false;
	}
	if (key == 'k')
	{
		leftWind = false;
		rightWind = false;
		noWind = true;
	}
	if (key == 'l')
	{
		leftWind = false;
		rightWind = true;
		noWind = false;
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
void initParticles(int i, int par_velocity)
{
	par_sys[i].alive = true;
	par_sys[i].life = 1.3;
	par_sys[i].size = .45 + float(rand() % 100) / 100.0f;

	par_sys[i].xpos = (float)(rand() % 21) - 10;
	par_sys[i].ypos = (float)(rand() % 31) + 20;
	par_sys[i].zpos = (float)(rand() % 21) - 10;

	par_sys[i].red = 0.5;
	par_sys[i].green = 0.5;
	par_sys[i].blue = 1.0;

	par_sys[i].vel = par_velocity;
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
	for (z = 0; z < 50; z++)
	{
		for (x = 0; x < 50; x++)
		{
			//change the - 10.0 to move the plane along x axis
			ground_points[x][z][0] = x - 20.0;
			ground_points[x][z][1] = accum;
			//change the -10.0 to move the plane along z axis
			ground_points[x][z][2] = z - 20.0;

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
		initParticles(loop, 0);
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
			glVertex3f(x, y + par_sys[loop].size, z);
			glEnd();


			//If particle reaches floor
			if ((par_sys[loop].ypos <= -10) && (par_sys[loop].xpos >= -20) && (par_sys[loop].zpos >= -20))
			{
				int zi = z - zoom + 20;
				int xi = x + 20;

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
			//add controls to change wind direction
			//calculate wind

			//let the user change this later
			float windspeed = .005;

			//the lower the particle the stronger the wind
			float yFactor = 1 - (par_sys[loop].ypos / 21);
			yFactor = yFactor / 10;

			//if the size of the particle >.5 then sizeFactor will be negative
			//if the size of the particle ==.5 then sizeFactor will be 0
			//if the size of the particle <.5 then sizeFactor will be positive
			float sizeFactor = (-.01)*(par_sys[loop].size - .5);

			if (rightWind)
			{
				windfactor = (.5)*(windspeed + yFactor + sizeFactor);
			}
			else if (leftWind)
			{
				windfactor = (-.5)*(windspeed + yFactor + sizeFactor);
			}
			else {
				windfactor = 0.0f;
			}
			par_sys[loop].zpos += windfactor / slowdown;
			par_sys[loop].xpos += windfactor / slowdown;
			// Update pos and vel of particles as they fall, can be slower if needed
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			if (par_sys[loop].vel > MAX_VELOCITY - gravity) 
			{
				par_sys[loop].vel += gravity;
			}

			//create another particle if a particle is removed
			if (par_sys[loop].life < 0.0)
			{
				initParticles(loop, par_sys[loop].vel);
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
			glutSolidSphere(par_sys[loop].size / 6, 16, 16);
			glPopMatrix();

			// Update pos and vel of particles as they fall, can be slower if needed
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			//let the user change this later
			float windspeed = .005;

			//the lower the particle the stronger the wind
			float yFactor = 1 - (par_sys[loop].ypos / 21);
			yFactor = yFactor / 10;

			//if the size of the particle >.5 then sizeFactor will be negative
			//if the size of the particle ==.5 then sizeFactor will be 0
			//if the size of the particle <.5 then sizeFactor will be positive
			float sizeFactor = (-.01)*(par_sys[loop].size - .5);

			if (rightWind)
			{
				windfactor = windspeed + yFactor + sizeFactor;
			}
			else if (leftWind)
			{
				windfactor = (-1)*(windspeed + yFactor + sizeFactor);
			}
			else {
				windfactor = 0.0f;
			}
			//float windfactor = windspeed + yFactor + sizeFactor;
			par_sys[loop].zpos += windfactor / slowdown;
			par_sys[loop].xpos += windfactor / slowdown;
			// Update pos and vel of particles as they fall, can be slower if needed
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			if (par_sys[loop].vel > MAX_VELOCITY - gravity)
			{
				par_sys[loop].vel += gravity;
			}

			//If the snow hits the ground
			if ((par_sys[loop].ypos <= -10) && (par_sys[loop].xpos >= -20) && (par_sys[loop].zpos >= -20))
			{

				int zi = z - zoom + 20;
				int xi = x + 20;

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
				initParticles(loop, par_sys[loop].vel);
			}
		}
	}
}
//Hail
void drawHail()
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
			glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();
			glTranslatef(x, y, z);
			glutSolidSphere(par_sys[loop].size / 12, 16, 16);
			glPopMatrix();

			//If the snow hits the ground
			if ((par_sys[loop].ypos <= -10) && (par_sys[loop].xpos >= -20) && (par_sys[loop].zpos >= -20))
			{

				int zi = z - zoom + 20;
				int xi = x + 20;

				ground_colors[zi][xi][0] = 1.0;
				ground_colors[zi][xi][1] = 1.0;
				ground_colors[zi][xi][3] += 0.0;

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
			//add controls to change wind direction
			//calculate wind

			//let the user change this later
			float windspeed = .005;

			//the lower the particle the stronger the wind
			float yFactor = 1 - (par_sys[loop].ypos / 21);
			yFactor = yFactor / 10;

			//if the size of the particle >.5 then sizeFactor will be negative
			//if the size of the particle ==.5 then sizeFactor will be 0
			//if the size of the particle <.5 then sizeFactor will be positive
			float sizeFactor = (-.01)*(par_sys[loop].size - .5);

			if (rightWind)
			{
				windfactor = windspeed + yFactor + sizeFactor;
			}
			else if (leftWind)
			{
				windfactor = (-1)*(windspeed + yFactor + sizeFactor);
			}
			else {
				windfactor = 0.0f;
			}
			par_sys[loop].zpos += windfactor / slowdown;
			par_sys[loop].xpos += windfactor / slowdown;
			// Update pos and vel of particles as they fall, can be slower if needed
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			if (par_sys[loop].vel > MAX_VELOCITY - gravity)
			{
				par_sys[loop].vel += gravity;
			}

			//create another particle if a particle is removed
			if (par_sys[loop].life < 0.0)
			{
				initParticles(loop, par_sys[loop].vel);
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
	//increase upper bound to increase size of plane
	for (i = -10; i + 1 < 31; i++)
	{
		//Loop along xy plane
		for (j = -10; j + 1 < 31; j++)
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
	else if (fall == HAIL) {
		drawHail();
	}
	//Control instructions
	std::string text;
	text = "Controls:";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7);
	text = "R - Rain";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 15);
	text = "S - Snow";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 30);
	text = "H - Hail";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 45);
	text = "B - Bounce";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 60);
	text = ", - Slow Down";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 75);
	text = ". - Speed Up";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 90);
	text = "O - Lower Density";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 115);
	text = "P - Increase Density";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 130);
	text = "RIGHT - Pan Right";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 145);
	text = "LEFT - Pan left";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 160);
	text = "UP - Pan up";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 175);
	text = "DOWN - Pan Down";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 190);
	text = "PgUp - Zoom In";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 215);
	text = "PgDn - Zoom Out";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 230);
	text = "Q - Quit";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 245);
	text = "J - Wind Blowing to the Left";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 260);
	text = "K - No Wind Blowing";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 275);
	text = "L - Wind Blowing to the Right";
	glColor3f(0, 1, 1);
	drawText(text.data(), text.size(), 30, HEIGHT*.7 - 290);
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
	glutCreateWindow("CG Final Project Enoch Huang, Igor Carvalho, Ben Gruber");
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