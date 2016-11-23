#pragma once
#ifndef SHADER_H
#define SHADER_H
#include "glsupport.h"
#include <GLUT/GLUT.h>
#include <string>
#include <cassert>
#include <unordered_map>
#include <iostream>
using namespace std;
class Shader {
public:
	Shader() {};
	Shader(const GLchar* vertexShaderFileName, const GLchar* fragmentShaderFileName) {
		program = glCreateProgram();
		readAndCompileShader(program, vertexShaderFileName, fragmentShaderFileName);
	};

	GLuint& setAttribute(const GLchar* name) {
		GLuint attribute = glGetAttribLocation(program, name);
		set(name, attribute);
		return get(name);
	}

	GLuint& setUniform(const GLchar* name) {
		GLuint uniform = glGetUniformLocation(program, name);
		set(name, uniform);
		return get(name);
	}

	GLuint& setTexture(const GLchar* name, const char* filename) {
		GLuint texture = loadGLTexture(filename);
		set(name, texture);
		return get(name);
	};

	GLuint& get(const GLchar* name) {
		assert(shaderValue.find(name) != shaderValue.end());
		return shaderValue.find(name)->second;
	}

	GLuint program;

private:
	void set(const GLchar* name, GLuint location) {
		assert(shaderValue.find(name) == shaderValue.end());
		shaderValue.emplace(name, location);
	}
	std::unordered_map<const GLchar*, GLuint> shaderValue;



};
#endif // !SHADER_H
