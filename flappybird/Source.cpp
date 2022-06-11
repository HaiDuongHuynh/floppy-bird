#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>
using namespace std;

//======================================================
// CONSTANTS 
//======================================================
// Pipe pipe_size
//#define size 50.0

//======================================================
// GLOBAL VARIABLES WRITTEN TO BY reshapeCallBack( )
//======================================================
// Window pipe_size
int w_height = 800;
int w_width = 800;

//======================================================
// GLOBAL VARIABLES WRITTEN TO BY displayCallBack( )
//======================================================
int display_count = 0; //keeps count of invocations
float pipe_size = 50.0;
//======================================================
// GLOBAL VARIABLES WRITTEN TO BY keyboardCallBack( )
//======================================================
float theta = 0.0; // Rotation angle of bird 
float theta1 = 0.0;
int timer_period_in_ms = 200; // time between timer callbacks in milliseconds

//====================================================================
// GLOBAL VARIABLES WRITTEN TO BY motionCallBack( )
//====================================================================
int bird_r = 20;
float bird_x = 40;
float bird_y = 40; // position of bird
float bird1_x = 45;
float bird1_y = 40;//position of bird's eye
float bird2_x = 50;
float bird2_y = 40;
double bird_vx = 0;
double bird_vy = 0;
double bird_ax = 0;
double bird_ay = 0;
int bird_dx = 1; // incremental change inbird_x
float dt = 0.005; // 0.15; //0.05;
double impulse = 0;
double impulseMax = 400;
double vyMax = 40;
double vxMax = 40;

double obj_x = 1000;
double obj_y = 200;

double pipe_x = 1000;
double pipe_y = 200;
double pipe_gap = 300;

int point_count = 0;
vector<pair <double, double>>position;
double wing_theta = 0;
double wing_delta = 0.05;

bool hasStarted = false;
int numUpdates = 0;
//======================================================
// IDLE CALLBACK ROUTINE 
//======================================================
void bounce() {
	//bird_ax += 0;
	//bird_ay += 20;
	impulse += impulseMax;
	bird_ax += 2;
}

void boundVelocityAcceleration(double& vx, double& vy, double& ax, double& ay) {
	if (vx > vxMax) vx = vxMax;
	if (vy > vyMax) vy = vyMax;
	if (ax > 20) ax = 20;
	if (ay < -10) ay = -9;
}

void boundImpulse() {
	if (impulse > impulseMax) impulse = 33;
	if (impulse < 0.5) impulse = 0;
}

void update()
{
	numUpdates++;
	if (numUpdates % 100 == 0) cout << "Num updates = " << numUpdates << "bird_x = " << bird_x << " bird_y = " << bird_y << " vx = " << bird_vx << " vy = " << bird_vy << " ax = " << bird_ax << " ay = " << bird_ay << "obj_x = " << obj_x << "obj_y = " << obj_y << endl;
	position.push_back(make_pair(bird_x, bird_y));
	//Wing flapping
	wing_theta += wing_delta;
	if (wing_theta > 30) {
		wing_delta *= -1;
		wing_theta += wing_delta;
	}
	if (wing_theta < 0) {
		wing_delta *= -1;
		wing_theta += wing_delta;
	}
	if (position.size() > 500) {
		position.erase(position.begin());
	}
	if (hasStarted) {
		//acceleration gets updated
		//bird_ay += (-9.8 + impulse) * dt;
		bird_ay += (-4.8 + impulse) * dt;
		//velocity gets updated
		bird_vx += bird_ax * dt;
		bird_vy += bird_ay * dt;
		//position gets updated
		bird_x += bird_vx * dt;
		bird_y += bird_vy * dt;
		boundVelocityAcceleration(bird_vx, bird_vy, bird_ax, bird_ay);
		//impulse *= 0.3;
		impulse *= 0.95;
		boundImpulse();
	}
	/*
	bird_x += bird_dx; //Increment x-position of bird
	if (bird_x > w_width || bird_x == 0) bird_dx *= -1; //Reverse direction if at edges
	  */

//===========================================================
//ANIMATION UP AND DOWN
//===========================================================
	if (bird_vy > 0) { 
		theta = 45;
	}

	else if (bird_vy < 20 && bird_vy > -20) {
		theta = 360;
	}

	else if (bird_vy < 0) {
		theta = 315;
	}

	else if (bird_vy < -50) {
		theta = 270;
	}

	if (bird_y - bird_r < 0) {
		bird_y = bird_r;
		bird_vy = 0;
		bird_ay = 0;
	}


	if (bird_y > w_height || bird_y == 20) {
		printf("Game Over!");
		//printf("Point: %d", point_count);
		exit(-1);
	}

	if (obj_x < (bird_x - w_width / 2.0)) {
		//random x
		obj_x = bird_x + w_width / 3.0 + (100.0 * rand()) / RAND_MAX;
		//random y
		obj_y = (rand()) % (w_height);
	}

	if (pipe_x < (bird_x - w_width / 2.0)) {
		//random x
		pipe_x = bird_x + w_width / 2.0 + (100.0 * rand()) / RAND_MAX;
		//random y
		pipe_y = (rand()) % (w_height / 2);
	}

	//Collision with the circle
	float d = (bird_x - obj_x) * (bird_x - obj_x) + (bird_y - obj_y) * (bird_y - obj_y);
	float rad = (bird_r + bird_r) * (bird_r + bird_r);
	if (d == rad || d < rad) {
		printf("Collision!");
		exit(-1);
	}

	//Collision with the pipe
	if (bird_x > pipe_x - pipe_size && bird_x < pipe_x + pipe_size) {
		if (bird_y <= pipe_y + 50 || bird_y > pipe_y + 320) {
			printf("Collision!");
			//printf("Point: %d", point_count);
			exit(-1);
		}
	}
	//else if (bird_x == pipe_x + pipe_size) point_count++;
	glutPostRedisplay();
}

//======================================================
// RESHAPE CALLBACK ROUTINE
//======================================================
void reshapeCallback(int width, int height)
{
	//Update globals containing window pipe_size
	w_width = width;
	w_height = height;
}

//======================================================
// MOUSE MOTION CALLBACK ROUTINE 
//======================================================
void motionCallBack(int x, int y)
{
	printf("YYYMotion call back: %d, %d)\n", x, y);
	//Set bird's location to current mouse position
	bird_x = x;
	bird_y = w_height - y; //Invert mouse y (as its measured from top)

	glutPostRedisplay();
}



//======================================================
// KEYBOARD CALLBACK ROUTINE 
//======================================================
bool isSimulating = false;
void keyboardCallBack(unsigned char key, int x, int y)
{
	printf("Keyboard call back: key=%c, x=%d, y=%d, theta=%f\n", key, x, y, theta);
	switch (key)
	{
	case 'i':
	case 'I':
		if (!isSimulating) {
			isSimulating = !isSimulating;
			printf("Idle function ON\n");
			break;
		}
		else {
			isSimulating = !isSimulating;
			glutIdleFunc(NULL);
			printf("Idle function OFF\n");
			break;
		}
	case 'r':
		theta = theta + 10.0;
		break;
	case ' ':
		hasStarted = true;
		isSimulating = true;
		bounce();
		break;
	case 27:
	case 'q':
		cout << "Exiting" << endl;
		exit(-1);
	default:
		printf("Press i (Idle Off), I (Idle ON) or r (Rotate)");
	}

	glutPostRedisplay();
}

//======================================================
// MOUSE CALLBACK ROUTINE 
//======================================================
void mouseCallBack(int btn, int state, int x, int y)
{
	printf("Mouse call back: button=%d, state=%d, x=%d, y=%d\n", btn, state, x, y);

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //Mouse click to jump

		hasStarted = true;
		isSimulating = true;
		bounce();
		//exit(-1);
	}


}

void DrawEllipse(float cx, float cy, float rx, float ry, int num_segments) //Wing 1
{
	float theta = 2 * 3.1415926 / float(num_segments);
	float c = cos(wing_theta);//precalculate the sine and cosine
	float s = sin(wing_theta);
	float t;

	float x = 10;//we start at angle = 0 
	float y = 0;

	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++)
	{
		//apply radius and offset
		glVertex2f(x * rx + cx - 30, y * ry + cy);//output vertex 

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
}

void DrawEllipse1(float cx, float cy, float rx, float ry, int num_segments) //Wing 2
{
	float theta = 2 * 3.1415926 / float(num_segments);
	float c = cos(wing_theta);//precalculate the sine and cosine
	float s = sin(wing_theta);
	float t;

	float x = 10;//we start at angle = 0 
	float y = 0;

	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++)
	{
		//apply radius and offset
		glVertex2f(x * rx + cx - 30, y * ry + cy + 10);//output vertex 

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
}

void DrawEllipse2(float cx, float cy, float rx, float ry, int num_segments) //Mouth
{
	float theta = 2 * 3.1415926 / float(num_segments);
	float c = cos(theta);//precalculate the sine and cosine
	float s = sin(theta);
	float t;

	float x = 10;//we start at angle = 0 
	float y = 0;

	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++)
	{
		//apply radius and offset
		glVertex2f(x * rx + cx + 25, y * ry + cy);//output vertex 

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
}

void DrawTail() {
	if (position.size() > 50) {
		float a = 255;
		float b = 255;
		float c = 0;
		glLineWidth(20.0);
		glPushMatrix();
		glBegin(GL_LINE_STRIP);
		glColor3f(a, b, c);
		for (int i = 0; i < position.size(); i++) {
			glVertex2f(position[i].first - bird_r / 2, position[i].second);
		}
		glEnd();
		glPopMatrix();
	}
}

void DrawWing(float r) {
	glBegin(GL_TRIANGLES);
	glVertex2f(0, 0);
	glVertex2f(-2 * r, 0);
	glVertex2f(-2 * r, -r / 2);
	glEnd();
}

void DrawCircle(float r, int num_segments) { //Body
	float theta = 2 * 3.1415926 / float(num_segments);
	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++) {
		float xi = r * cos((theta * ii));
		float yi = r * sin((theta * ii));
		glVertex2f(xi, yi);
	}
	glEnd();
}

void DrawCircle1(float r, int num_segments) { //eye white
	float theta = 2 * 3.1415926 / float(num_segments);
	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++) {
		float xi = r * cos((theta * ii));
		float yi = r * sin((theta * ii));
		glVertex2f(xi + 10, yi + 10);
	}
	glEnd();
}

void DrawCircle2(float r, int num_segments) { //eye black
	float theta = 2 * 3.1415926 / float(num_segments);
	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++) {
		float xi = r * cos((theta * ii));
		float yi = r * sin((theta * ii));
		glVertex2f(xi + 15, yi + 10);
	}
	glEnd();
}



//======================================================
// DISPLAY CALL BACK ROUTINE 
//======================================================
void displayCallBack(void)
{
	//printf("Display call back %d\n", display_count++);
	//cout << "pos of bird [" << bird_x << "," << bird_y << "]"<< endl;

	if (bird_x > w_width / 2) {
		// Set Projection Mode and Volume
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glOrtho(0.0, w_width, 0.0, w_height, -1.0, 1.0);
		glOrtho(bird_x - w_width / 2, bird_x + w_width / 2, 0.0, w_height, -1.0, 1.0);
	}

	glClearColor(0.0, 216.0, 230.0, 1.0); //background color
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*
	//draw plane
	glPushMatrix();
	glColor3ub(0, 0, 255);
	glBegin(GL_QUADS);
	glVertex2f(w_width, 20);
	glVertex2f(-w_width, 20);
	glVertex2f(-w_width, 0);
	glVertex2f(w_width, 0);
	glPopMatrix();
	*/

	//DrawTail();

	//draw bird
	glPushMatrix();
	glTranslatef(bird_x, bird_y, 0.0);
	glRotated(theta, 0, 0, 1);
	glColor3ub(255, 215, 0);
	DrawCircle(bird_r, 20);
	glColor3ub(255, 255, 255);
	DrawCircle1(10, 20);
	glColor3ub(0, 0, 0);
	DrawCircle2(4, 20);
	glColor3ub(255, 69, 0);
	DrawEllipse2(3, -5, 1.5, 1, 20);
	glRotatef(wing_theta, 1, 0, 0.0);

	/*glBegin(GL_POLYGON);
	glColor3ub(255, 255, 0);
	glVertex2f(pipe_size, pipe_size);
	glVertex2f(-pipe_size, pipe_size);
	glVertex2f(-pipe_size, -pipe_size);
	glVertex2f(+pipe_size, -pipe_size);
	glEnd();
	*/

	glColor3ub(211, 211, 211);
	DrawEllipse1(3, 2, 2, 1, 20);

	glColor3ub(255, 255, 255);
	DrawEllipse(3, 2, 2, 1, 20);

	glPopMatrix();



	//draw pipe
	glPushMatrix();
	glTranslatef(pipe_x, pipe_y, 0.0);
	glBegin(GL_QUADS);
	glColor3ub(34, 139, 34);
	glVertex2f(pipe_size, pipe_size);
	glVertex2f(-pipe_size, pipe_size);
	glVertex2f(-pipe_size, -pipe_size - w_height);
	glVertex2f(+pipe_size, -pipe_size - w_height);

	glColor3ub(0, 128, 0);
	glVertex2f(pipe_size + 10, pipe_size);
	glVertex2f(-pipe_size - 10, pipe_size);
	glVertex2f(-pipe_size - 10, -pipe_size + 50);
	glVertex2f(+pipe_size + 10, -pipe_size + 50);

	glColor3ub(34, 139, 34);
	glVertex2f(pipe_size, pipe_size + pipe_gap);
	glVertex2f(-pipe_size, pipe_size + pipe_gap);
	glVertex2f(-pipe_size, pipe_size + w_height + pipe_gap);
	glVertex2f(+pipe_size, pipe_size + w_height + pipe_gap);

	glColor3ub(0, 128, 0);
	glVertex2f(pipe_size + 10, pipe_size + pipe_gap);
	glVertex2f(-pipe_size - 10, pipe_size + pipe_gap);
	glVertex2f(-pipe_size - 10, pipe_size + 50 + pipe_gap);
	glVertex2f(+pipe_size + 10, pipe_size + 50 + pipe_gap);

	glEnd();

	/*
	glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
	glVertex2f(obj_x-50, obj_y+50);     // Define vertices in counter-clockwise (CCW) order
	glVertex2f(obj_x-50, obj_y-50);     //  so that the normal (front-face) is facing you
	glVertex2f(obj_x+50, obj_y-50);
	glVertex2f(obj_x+50, obj_y+50);
	*/


	glPopMatrix();


	//draw obj
	glPushMatrix();
	glTranslatef(obj_x, obj_y, 0.0);
	glColor3f(0, 0, 1);
	DrawCircle(bird_r, 20);
	glPopMatrix();

	//Draw wing
	/*
	glPushMatrix();
	glPopMatrix();
	glColor3f(255, 0, 0);
	glTranslatef(bird_x, bird_y, 0.0);
	glRotated(wing_theta, 0, 0, 1);
	DrawWing(bird_r);
	*/
	//Swap double buffers 
	glutSwapBuffers();
}

//======================================================
// MAIN PROGRAM
//======================================================
int main(int argc, char** argv)
{
	// Allow cmd line arguments to be passed to the glut
	glutInit(&argc, argv);

	// Create and name window
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(w_width, w_height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Flappy Bird");

	// Set Projection Mode and Volume
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, w_width, 0.0, w_height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);

	// Set clear color to black and clear window 
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();

	// Set up callbacks
	glutMouseFunc(mouseCallBack);
	glutKeyboardFunc(keyboardCallBack);
	//glutMotionFunc(motionCallBack);
	glutDisplayFunc(displayCallBack);
	glutReshapeFunc(reshapeCallback);
	glutIdleFunc(update);

	// Enter main event loop
	glutMainLoop();
}

