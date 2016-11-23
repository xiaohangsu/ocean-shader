#pragma once
#ifndef TRACKBALL_H
#define TRACKBALL_H
#include "matrix4.h"
#include "glsupport.h"
#include "quat.h"

class Trackball {
public:
	Trackball(int winWidth, int winHeight, int trackBallRadius, Matrix4 &eyeMatrix)
		: cx(winWidth / 2), cy(winHeight / 2),
		radius(trackBallRadius), eyeMatrix(eyeMatrix){	
	};

	Cvec3 calculateVec(int x, int y) {
		x = x - cx;
		y = y - cy;
		return Cvec3(x, y, sqrt(radius * radius - x * x - y * y));
	}

	bool isInside(int x, int y) {
		x = abs(x - cx);
		y = abs(y - cy);
		return radius * radius >= x * x + y * y;
	}

	bool active = false;
	int cx = 0;
	int cy = 0;
	int radius = 35;
	Cvec3 lastPos;
	Matrix4 &eyeMatrix;
};

#endif // !TRACKBALL_H
