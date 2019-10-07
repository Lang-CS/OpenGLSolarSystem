#include <stdlib.h>
#include <stdio.h>
#include <glut.h>
#include <math.h>

#define numPlanets 11
#define numAsteroids 400
#define numStars 2000
#define MATH_PI 3.14159265358979323846264

// Planet data structure
// Holds information to draw a single planet

struct Planet {
	float x;
	float y;
	float z;
	float radius;
	float r;
	float g;
	float b;
	float a;
	float speed;
	struct Planet *parent; // What the planetoid is circling around
	float rotProgress;
	float orbitRadius;
};

struct Planet planets[numPlanets];
struct Planet asteroids[numAsteroids];
// Holds verticies for stars
float stars[3][numStars];

// Toggles
int starToggle;
int shieldToggle;
int sunCoronaToggle;
int pathToggle;
int asteroidToggle;

// arrow keys for camera movement
int leftToggle;
int rightToggle;
int upToggle;
int downToggle;
int pUpToggle;
int pDownToggle;

// current camera position
double cameraPos[3];

float windowWidth = 500;
float windowHeight = 500;

float *verts;
int *tris;
int numFaces;
struct Planet shield;

float shipOffsetX = 0;
float shipOffsetY = -6;
float shipOffsetZ = -20;

// difference added at each frame
GLfloat interpDifference = 0.010;

void drawSphere(struct Planet *point, int rings, int segments);
void drawPaths(struct Planet *point, int segments);

// Returns random number from 0 to 1
float makeRand(void) {
	return  (rand() / (float) RAND_MAX);
}

void drawSolarSystem(void) {
	int i;

	// Drawing Planet paths if applicable
	if (pathToggle == 1) {
		for (i = 1; i < numPlanets; i++) {
			drawPaths(&planets[i], 32 + 4*i);
		}
	}

	// draw planets
	for (i = 0; i < numPlanets; i++) {
		drawSphere(&planets[i], 12, 16);
	}
	glutPostRedisplay();
}

void drawAsteroids() {
	int i;
	for (i = 0; i < numAsteroids; i++) {
		drawSphere(&asteroids[i], 6, 8);
	}
	glutPostRedisplay();
}

void drawStars(void) {
	int i;
	
		for (i = 0; i < numStars; i++) {
			float r = makeRand() * 0.6 + 0.4;
			float g = makeRand() * 0.6 + 0.4;
			float b = makeRand() * 0.6 + 0.4;
			glColor4f(r, g, b, stars[2][i]);
			glBegin(GL_POINTS);
			glVertex3d(stars[0][i], stars[1][i], -5000);
			glEnd();
		}
		glutPostRedisplay();
}

void initSpaceShip(void) {
	// file used for logging errors
	//FILE *err;
	//err = freopen("out.txt", "w", stdout);

	FILE *fptr;
	fptr = fopen("enterprise.txt", "r");

	if (fptr == NULL) {
		printf("Error!");
		exit(1);
	}

	int vertSize = 1024;
	verts = (float *)malloc(sizeof(float) * vertSize * 3);

	char mode;
	float x, y, z;
	int i = 0;
	while (fscanf(fptr, "%c ", &mode) != EOF && mode == 'v') {

		if (i >= vertSize) {
			vertSize *= 2;
			verts = (float *)realloc(verts, sizeof(float) * vertSize * 3);
		}
		fscanf(fptr, " %f %f %f \n", &x, &y, &z);
		*(verts + i * 3 + 0) = x;
		*(verts + i * 3 + 1) = y;
		*(verts + i * 3 + 2) = z;
		//printf("v %f %f %f\n", *(verts + i * 3), *(verts + i * 3 + 1), *(verts + i * 3 + 2));
		i++;
	}
	int x2, y2, z2;
	// Number of triangles to initially allocate
	int triSize = 1024;
	// allocate memory for 3d int array
	tris = (int *)malloc(sizeof(int) * triSize * 3);

	// leftover cleanup from the vert loop
	fscanf(fptr, " %d %d %d \n", &x2, &y2, &z2);
	*(tris + 0) = x2;
	*(tris + 1) = y2;
	*(tris + 2) = z2;
	//printf("f %d %d %d\n", *(tris + 0), *(tris + 1), *(tris + 2));
	i = 1;
	// loop through lines until reaching end of file or non f char
	while (fscanf(fptr, "%c", &mode) != EOF && mode == 'f') {
		// If we've run out of memory, double it
		if (i >= triSize) {
			triSize *= 2;
			tris = (int *)realloc(tris, sizeof(int) * triSize * 3);
		}
		// scan and store integer values
		fscanf(fptr, " %d %d %d \n", &x2, &y2, &z2);
		*(tris + i * 3 + 0) = x2;
		*(tris + i * 3 + 1) = y2;
		*(tris + i * 3 + 2) = z2;

		//printf("f %d %d %d\n", *(tris + i * 3 + 0), *(tris + i * 3 + 1), *(tris + i * 3 + 2));
		i++;
	}
	//printf("%d %d %d\n", *tris, *(tris+1), *(tris+2));
	numFaces = i;
	//printf("numfaces: %d\n", numFaces);
	//fclose(err);
	fclose(fptr);
	
	// init the shield
	shield.radius = 3.5;

}

// draws the space ship
void drawSpaceShip() {
	int i;
	float shipSize = 5;

	float offsetX = cameraPos[0] + shipOffsetX;
	float offsetY = cameraPos[1] + shipOffsetY;
	float offsetZ = cameraPos[2] + shipOffsetZ;

	glColor4f(0.6, 0.6, 0.6, 1.0);
	glBegin(GL_TRIANGLES);
	for (i = 0; i < numFaces; i++) {
		int vert1, vert2, vert3;
		vert1 = *(tris + i * 3 + 0) - 1;
		vert2 = *(tris + i * 3 + 1) - 1;
		vert3 = *(tris + i * 3 + 2) - 1;

		glVertex3f(*(verts + vert1 * 3 + 0) * shipSize + offsetX, *(verts + vert1 * 3 + 1) * shipSize + offsetY, *(verts + vert1 * 3 + 2) * shipSize + offsetZ);
		glVertex3f(*(verts + vert2 * 3 + 0) * shipSize + offsetX, *(verts + vert2 * 3 + 1) * shipSize + offsetY, *(verts + vert2 * 3 + 2) * shipSize + offsetZ);
		glVertex3f(*(verts + vert3 * 3 + 0) * shipSize + offsetX, *(verts + vert3 * 3 + 1) * shipSize + offsetY, *(verts + vert3 * 3 + 2) * shipSize + offsetZ);
		
	}
	glEnd();

	if (upToggle == 1) {
		
		glBegin(GL_LINES);
		glColor4f(0.5, 0.5, 1.0, 1.0);
		glVertex3d(offsetX + 1.4, offsetY + 0.2, offsetZ + 0.1);
		glColor4f(1.0, 1.0, 1.0, 0.2);
		glVertex3d(offsetX + 1.4, offsetY + 0.2, offsetZ + 7);

		glColor4f(0.5, 0.5, 1.0, 1.0);
		glVertex3d(offsetX - 1.4, offsetY + 0.2, offsetZ + 0.1);
		glColor4f(1.0, 1.0, 1.0, 0.2);
		glVertex3d(offsetX - 1.4, offsetY + 0.2, offsetZ + 7);
		glEnd();
	}


	if (shieldToggle == 1) {
		drawSphere(&shield, 24, 32);
	}
	glutPostRedisplay();
}

// draws a firey circle around the sun
void drawSunCorona(void) {
	int i;
	glBegin(GL_LINES);
	for (i = 0; i < 700; i++) {
		float x, y, z;
		float c = 2 * MATH_PI;
		float phi = makeRand();
		float thet = makeRand();
		float coronaLength = 6;

		glColor4f(planets[0].r, planets[0].g, planets[0].b, 1.0);
		
		x = cos(c*phi)*cos(c*thet) * (planets[0].radius);
		y = sin(c*phi) * (planets[0].radius);
		z = 0; //cos(c*phi)*sin(c*thet) * (planets[0].radius);
		glVertex3d(x, y, z);
		glColor4f(planets[0].r, planets[0].g, planets[0].b, 0.1);
		x = cos(c*phi)*cos(c*thet) * (planets[0].radius + coronaLength);
		y = sin(c*phi) * (planets[0].radius + coronaLength);
		z = 0; //cos(c*phi)*sin(c*thet) * (planets[0].radius + coronaLength);
		glVertex3d(x, y, z);
	}
	glEnd();
	glutPostRedisplay();
}


void mouse(int btn, int state, int x, int y)
{

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 's') {
		starToggle = 1 - starToggle;
		glutPostRedisplay();
	}
	else if (key == 'c') {
		sunCoronaToggle = 1 - sunCoronaToggle;
		glutPostRedisplay();
	}
	else if (key == 'k') {
		shieldToggle = 1 - shieldToggle;
		glutPostRedisplay();
	}
	else if (key == 'r') {
		pathToggle = 1 - pathToggle;
		glutPostRedisplay();
	}
	else if (key == 'a') {
		asteroidToggle = 1 - asteroidToggle;
		glutPostRedisplay();
	}

	
}

void specialKeyboard(int key, int x, int y) {
	switch (key){
	case (GLUT_KEY_UP):
		upToggle = 1;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_DOWN):
		downToggle = 1;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_LEFT):
		leftToggle = 1;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_RIGHT):
		rightToggle = 1;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_PAGE_DOWN):
		pDownToggle = 1;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_PAGE_UP):
		pUpToggle = 1;
		glutPostRedisplay();
		break;
	}

}

void specialKeyboardUp(int key, int x, int y) {
	switch (key) {
	case (GLUT_KEY_UP):
		upToggle = 0;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_DOWN):
		downToggle = 0;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_LEFT):
		leftToggle = 0;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_RIGHT):
		rightToggle = 0;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_PAGE_DOWN):
		pDownToggle = 0;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_PAGE_UP):
		pUpToggle = 0;
		glutPostRedisplay();
		break;
	}

}

void initPlanets(void) {
	struct Planet sun = {
	.x = 0, .y = 0, .z = 0, .radius = 7,
	.r = 1.0, .g = 0.5, .b = 0, .a = 1,};
	planets[0] = sun;

	struct Planet mercury = {
	.x = 0, .y = 0, .z = -30, .radius = 1.0,
	.r = 1.0, .g = 0.1, .b = 0.1, .a = 1,
	.speed = 10.71, .parent = &planets[0], .rotProgress = 0, .orbitRadius = 30};
	planets[1] = mercury;

	struct Planet venus = {
	.x = 0,.y = 0, .z = -50, .radius = 2.0,
	.r = 1.0, .g = 0.3, .b = 0.3, .a = 1,
	.speed = 7.83, .parent = &planets[0], .rotProgress = 0, .orbitRadius = 50 };
	planets[2] = venus;

	struct Planet earth = {
	.x = 0,.y = 0,.z = -70,.radius = 2.2,
	.r = 0.3, .g = 0.3, .b = 1.0,.a = 1,
	.speed = 6.66,.parent = &planets[0],.rotProgress = 0,.orbitRadius = 70 };
	planets[3] = earth;

	struct Planet mars = {
	.x = 0,.y = 0,.z = -90,.radius = 1.9,
	.r = 0.8,.g = 0.1,.b = 0.0,.a = 1,
	.speed = 5.39,.parent = &planets[0],.rotProgress = 0,.orbitRadius = 90 };
	planets[4] = mars;

	struct Planet jupiter = {
	.x = 0,.y = 0,.z = -150,.radius = 4.3,
	.r = 1.0,.g = 0.6,.b = 0.1,.a = 1,
	.speed = 2.92,.parent = &planets[0],.rotProgress = 0,.orbitRadius = 150 };
	planets[5] = jupiter;

	struct Planet saturn = {
	.x = 0,.y = 0,.z = -170,.radius = 3.0,
	.r = 0.7,.g = 0.7,.b = 0.7,.a = 1,
	.speed = 2.17,.parent = &planets[0],.rotProgress = 0,.orbitRadius = 170 };
	planets[6] = saturn;

	struct Planet uranus = {
	.x = 0,.y = 0,.z = -190,.radius = 2.5,
	.r = 0.3,.g = 0.3,.b = 1.0,.a = 1,
	.speed = 1.52,.parent = &planets[0],.rotProgress = 0,.orbitRadius = 190 };
	planets[7] = uranus;

	struct Planet neptune = {
	.x = 0,.y = 0,.z = -210,.radius = 2.5,
	.r = 0.3,.g = 0.3,.b = 1.0,.a = 1,
	.speed = 1.21,.parent = &planets[0],.rotProgress = 0,.orbitRadius = 210 };
	planets[8] = neptune;

	struct Planet pluto = {
	.x = 0,.y = 0,.z = -230,.radius = 0.5,
	.r = 0.9,.g = 0.9,.b = 0.9,.a = 1,
	.speed = 0.8,.parent = &planets[0],.rotProgress = 0,.orbitRadius = 230 };
	planets[9] = pluto;

	struct Planet moon = {
	.x = 0,.y = 0,.z = -70,.radius = 0.6,
	.r = 0.9,.g = 0.9,.b = 0.9,.a = 1,
	.speed = 100,.parent = &planets[3],.rotProgress = 0,.orbitRadius = 6 };
	planets[10] = moon;
}

void initAsteroids(void) {
	int i;
	for (i = 0; i < numAsteroids; i++) {
		struct Planet asteroid = {
		.radius = 0.25 + makeRand() * 0.2,.r = 0.4 + makeRand() * 0.07,.g = 0.4 + makeRand() * 0.07,.b = 0.4 + makeRand() * 0.1,.a = 0.9,.x = 0,.y = 0,.z = -120,
		.speed = 3 + makeRand() * 0.5,.parent = &planets[0],.rotProgress = makeRand() * 360.0,
		.orbitRadius = 110 + makeRand() * 20 };
		asteroids[i] = asteroid;

	}
}

void initStars(void) {
	int i;
	for (i = 0; i < numStars; i++) {
		float x = makeRand() * 8000 - 4000;
		float y = makeRand() * 8000 - 5000;
		float a = makeRand() / 1.2; // alpha
		stars[0][i] = x;
		stars[1][i] = y;
		stars[2][i] = a;
	}
}

void init(void) {
	starToggle = 1;
	shieldToggle = 1;
	sunCoronaToggle = 1;
	pathToggle = 1;
	asteroidToggle = 1;
	cameraPos[0] = 0;
	cameraPos[1] = 120;
	cameraPos[2] = 600;
	initStars();
	initPlanets();
	initSpaceShip();
	initAsteroids();
}

void display(void)
{
	// Clearing screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Loading identity matrix
	glLoadIdentity();
	
	// look straight ahead of the camera
	gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], cameraPos[0], cameraPos[1], cameraPos[2] - 100, 0, 1, 0);

	if (starToggle == 1) {
		drawStars();
	}
	
	if (asteroidToggle == 1) {
		drawAsteroids();
	}

	drawSolarSystem();

	if (sunCoronaToggle == 1) {
		drawSunCorona();
	}

	drawSpaceShip();

	glutSwapBuffers();
}

void idle(void) {
	
	// see if we have reached the extremes (-1, +1) 
	if (upToggle == 1) {
		cameraPos[2] += -1;
	}
	if (downToggle == 1) {
		cameraPos[2] += 1;
	}
	if (leftToggle == 1) {
		cameraPos[0] += -1;
	}
	if (rightToggle == 1) {
		cameraPos[0] += 1;
	}
	if (pUpToggle == 1) {
		cameraPos[1] += 1;
	}
	if (pDownToggle == 1) {
		cameraPos[1] += -1;
	}

	
	int i;
	// Calculate movement of planets
	for (i = 1; i < numPlanets; i++) {
		double c = 3.14159265 / 180.0;
		planets[i].rotProgress = planets[i].rotProgress + 0.02 * planets[i].speed;

		planets[i].x = planets[i].orbitRadius * cos(c*planets[i].rotProgress) + planets[i].parent->x;
		planets[i].z = planets[i].orbitRadius * sin(c*planets[i].rotProgress) + planets[i].parent->z;
	}

	for (i = 0; i < numAsteroids; i++) {
		double c = 3.14159265 / 180.0;
		asteroids[i].rotProgress = asteroids[i].rotProgress + 0.02 * asteroids[i].speed;

		asteroids[i].x = asteroids[i].orbitRadius * cos(c*asteroids[i].rotProgress) + asteroids[i].parent->x;
		asteroids[i].z = asteroids[i].orbitRadius * sin(c*asteroids[i].rotProgress) + asteroids[i].parent->z;

	}

	if (shieldToggle == 1) {
		shield.x = cameraPos[0] + shipOffsetX;
		shield.y = cameraPos[1] + shipOffsetY;
		shield.z = cameraPos[2] + shipOffsetZ + 1;
		shield.r = 0.1 + makeRand() * 0.1;
		shield.g = 0.1 + makeRand() * 0.1;
		shield.b = 0.5 + makeRand() * 0.2;
		shield.a = 0.3 + makeRand() * 0.05;
	}
}

void printControls(void) {
	printf("Scene controls\n");
	printf("----------------\n");
	printf("r:	toggle Rings\n");
	printf("c:	toggle the suns corona\n");
	printf("s:	toggle stars\n");
	printf("k:	toggle shields\n");
	printf("a:	toggle asteroids\n");
	printf("\n");
	printf("Camera Controls\n");
	printf("----------------\n");
	printf("Up	Arrow:	move up\n");
	printf("down	arrow:	move down\n");
	printf("right	arrow:	move right\n");
	printf("left	arrow:	move left\n");
	printf("PAGE	UP:	ascend\n");
	printf("PAGE	DOWN:	descend\n");
}


void main(int argc, char **argv)
{
	printControls();
	glutInit(&argc, argv);

	srand(time(NULL));

	/* need both double buffering and z buffer */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Solar System");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutSpecialUpFunc(specialKeyboardUp);

	// Enable Transparancy
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	// GL LINE for wireframe, GL_Fill for solid
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// initialize variables and such
	init();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float) windowWidth / (float) windowHeight, 0.1, 10000);
	glMatrixMode(GL_MODELVIEW);

	glutMainLoop();

	free(tris);
	free(verts);
}

// Draws sphere at the given location, with the given resolution and radius
void drawSphere(struct Planet *point, int rings, int segments) {

	glColor4f(point->r, point->g, point->b, point->a);

	double thet, phi, c;
	c = 3.14159265 / 180.0;
	double x, y, z;

	double segmentDegrees = 360.0 / (double) segments;
	double ringDegrees = 180.0 / (double) (rings - 1);

	//Top cap
	x = point->x;
	y = point->y + point->radius;
	z = point->z;
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(x, y, z);
	y = sin(c*(90 - ringDegrees)) * point->radius + point->y;
	for (thet = 0.0; thet <= 360.0; thet += segmentDegrees) {
		x = cos(c*thet)*cos(c*(90 - ringDegrees)) * point->radius + point->x;
		z = sin(c*thet)*cos(c*(90 - ringDegrees)) * point->radius + point->z;
		glVertex3d(x, y, z);
	}
	glEnd();

	// Bottom cap
	x = point->x;
	y = point->y - point->radius;
	z = point->z;
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(x, y, z);
	y = sin(c*(270.0 - ringDegrees)) * point->radius + point->y;
	for (thet = 0.0; thet <= 360.0; thet += segmentDegrees) {
		x = cos(c*thet)*cos(c*(270.0 - ringDegrees)) * point->radius + point->x;
		z = sin(c*thet)*cos(c*(270.0 - ringDegrees)) * point->radius + point->z;
		glVertex3d(x, y, z);
	}
	glEnd();


	// Quad rings
	for (phi = ringDegrees; phi <= 360.0 - ringDegrees; phi += ringDegrees) {
		glBegin(GL_QUAD_STRIP);
		for (thet = 0.0; thet <= 360.0;thet += segmentDegrees) {	
			x = cos(c*phi)*cos(c*thet) * point->radius + point->x;
			y = sin(c*phi) * point->radius + point->y;
			z = cos(c*phi)*sin(c*thet) * point->radius + point->z;
			glVertex3d(x, y, z);

			x = cos(c*(phi + ringDegrees))*cos(c*thet) * point->radius + point->x;
			y = sin(c*(phi + ringDegrees)) * point->radius + point->y;
			z = cos(c*(phi + ringDegrees))*sin(c*thet) * point->radius + point->z;
			glVertex3d(x, y, z);
		}
		glEnd();
	}
}

// Drawing path of a single planet
void drawPaths(struct Planet *point, int segments) {
	glColor4f(1.0, 1.0, 1.0, 0.3);
	double x, y, z, c, thet;
	c = 3.14159265 / 180.0;
	double segmentDegrees = 360.0 / (double)segments;

	glBegin(GL_LINE_STRIP);
	for (thet = 0.0; thet <= 360.0 + segmentDegrees;thet += segmentDegrees) {
		x = point->orbitRadius * cos(c*thet) + point->parent->x;
		y = point->parent->y;
		z = point->orbitRadius * sin(c*thet) + point->parent->z;
		glVertex3d(x, y, z);
	}

	glEnd();
}