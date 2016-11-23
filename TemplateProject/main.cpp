#include <iostream>
#include "glsupport.h"
#include <GLUT/GLUT.h>
#include "Shader.h"
#include "utilities.h" // eyeMatrix In here
#include "trackball.h"
using namespace std;

int WIDTH = 500;
int HEIGHT = 500;
Shader oceanShader;


Entity ocean;

int RANDOM_COLOR_SHAPE = 2;

static Trackball trackball(WIDTH, HEIGHT, 250, eyeMatrix);

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Matrix4 invEyeMatrix = inv(track* eyeMatrix);
	glUseProgram(oceanShader.program);

	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	Transform planeOriginTransform = ocean.transform;



	// draw floor

	glUniform3fv(oceanShader.get("uColor"), 1, color.back());
	glUniform1f(oceanShader.get("time"), (timeSinceStart / 1000.0));
	glUniform1f(oceanShader.get("time2"), (timeSinceStart / 700.0));

    
	ocean.Draw(invEyeMatrix, oceanShader.get("position"), oceanShader.get("normal"),
		oceanShader.get("modelViewMatrix"), oceanShader.get("normalMatrix"), oceanShader.get("texCoord"));

	glutSwapBuffers();

	ocean.transform = planeOriginTransform;
}

void init() {
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glClearDepth(0.0);
	glReadBuffer(GL_BACK);

	Matrix4 projectionMatrix;
	eyeMatrix = lookAt(LOOK_AT_EYE, LOOK_AT_POINT, LOOK_AT_UP);
	projectionMatrix = projectionMatrix.makeProjection(45.0, 1.0, -0.1, -300.0);
	GLfloat glmatrixProjection[16];
	projectionMatrix.writeToColumnMajorMatrix(glmatrixProjection);

	oceanShader = Shader("oceanVertex.glsl", "oceanFragment.glsl");

	glUseProgram(oceanShader.program);


	oceanShader.setAttribute("position");
	oceanShader.setAttribute("normal");
	oceanShader.setAttribute("texCoord");

    // Lights init
    setShaderLight(oceanShader);
    
	// Textures Init

	glUniform1i(oceanShader.setUniform("diffuseTexture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, oceanShader.setTexture("oceanTexture", "ocean.png"));


	// Matrix projectionMatrix eyeMatrix
	glUniformMatrix4fv(oceanShader.setUniform("projectionMatrix"), 1, false, glmatrixProjection);

	oceanShader.setUniform("modelViewMatrix");
	oceanShader.setUniform("normalMatrix");
	oceanShader.setUniform("uColor");
	oceanShader.setUniform("time");
	oceanShader.setUniform("time2");



	// Objects init
	vector<VertexPNTBTG> planeVtx;
	vector<unsigned short>planeIdx;

	vector<VertexPNTBTG> meshVertices;
	vector<unsigned short> meshIndices;

	getMeshFloor(planeVtx, planeIdx, 1, 100, 100);
	generateBuffers(ocean, planeVtx, planeIdx);
	ocean.transform.rotation = Quat::makeXRotation(90.0f);
	ocean.transform.position = Cvec3(0.0, 0, -1.0);

    // OCEAN COLOR
    rgb(109, 165, 255);
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void idle(void) {
	glutPostRedisplay();
}
/*
 BELOW IS TRACKBALL CODE
*/
bool activeTrackBall = false;
int curX = 0;
int curY = 0;
void keyboard(unsigned char key, int x, int y) {
	Cvec3 dif = normalize(LOOK_AT_EYE);
	switch (key)
	{
	case 'w':
		if (LOOK_AT_EYE - dif != LOOK_AT_POINT) {
			LOOK_AT_EYE -= dif;
			eyeMatrix = lookAt(LOOK_AT_EYE, LOOK_AT_POINT, LOOK_AT_UP);
		}
		break;
	case 's':
		if (LOOK_AT_EYE + dif != LOOK_AT_POINT) {
			LOOK_AT_EYE += dif;
			eyeMatrix = lookAt(LOOK_AT_EYE, LOOK_AT_POINT, LOOK_AT_UP);
		}
		break;
	default:
		break;
	}
};

void mouseMoveCallback(int x, int y) {
    x = WIDTH - x;
	if (!trackball.isInside(x, y)) {
		trackball.active = false;
		return;
	}

	if (trackball.active) {
		Cvec3 now = trackball.calculateVec(x, y);
		track *= quatToMatrix(Quat(0, now) * Quat(0, -trackball.lastPos));
		trackball.lastPos = now;
	}
}


void mouseCallback(int button, int state, int x, int y) {
    x = WIDTH - x;

	if (!trackball.isInside(x, y)) return;
	if (state == GLUT_DOWN) {
		trackball.active = true;
		trackball.lastPos = trackball.calculateVec(x, y);
	}
	else if (state == GLUT_UP) {
		trackball.active = false;
	}
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OCEAN-SIMULATOR");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMoveCallback);
	init();
	glutMainLoop();
	return 0;
}
