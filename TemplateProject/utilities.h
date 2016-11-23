#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H
#include "quat.h"
#include "matrix4.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "geometrymaker.h"
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "Shader.h"

Cvec3 LOOK_AT_EYE = Cvec3(0.0, -30.0, 15.0);
Cvec3 LOOK_AT_POINT = Cvec3(0.0, 0.0, 5.0);
Cvec3 LOOK_AT_UP = Cvec3(0, 0, 1);

Matrix4 objectMatrix;
Matrix4 eyeMatrix;

GLfloat glmatrix[16];
GLfloat glmatrixNormal[16];

Matrix4 track;

struct Transform {
	Quat rotation;
	Cvec3 scale;
	Cvec3 position;
	Matrix4 parent;
	Transform() : scale(1.0f, 1.0f, 1.0f), position(0.0f, 0.0f, 0.0f) {
	}
	Matrix4 createMatrix() {
		return parent * Matrix4::makeTranslation(position) * quatToMatrix(rotation) * Matrix4::makeScale(scale);
	};
};

struct VertexPNTBTG {
	Cvec3f p; // point vertices coordinates
	Cvec3f n; // normal coordinates
    Cvec3f b; // binormal coordinate
    Cvec3f tg; // tangent coordinate
	Cvec2f t; // texture coordinates
	VertexPNTBTG() {}
	VertexPNTBTG(float x, float y, float z, float nx, float ny, float nz) : p(x, y, z), n(nx, ny, nz) {};
	VertexPNTBTG& operator = (const GenericVertex& v) {
		p = v.pos;
		n = v.normal;
		t = v.tex;
        b = v.binormal;
        tg = v.tangent;
		return *this;
	}
};

struct Geometry {
	GLuint vertexBO;
	GLuint indexBO;
	int numIndices;
	void Draw(GLuint positionAttribute, GLuint normalAttribute, GLuint texCoordAttribute = -1, GLuint binormalAttribute = -1, GLuint tangentAttribute = -1) {
		// bind buffer objects and draw
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG),
			(void*)offsetof(VertexPNTBTG, p));
		glEnableVertexAttribArray(positionAttribute);

		glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG),
			(void*)offsetof(VertexPNTBTG, n));
		glEnableVertexAttribArray(normalAttribute);

		if (texCoordAttribute != -1) {
			glEnableVertexAttribArray(texCoordAttribute);
			glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG),
				(void*)offsetof(VertexPNTBTG, t));
		}
        if (binormalAttribute != -1) {
            glEnableVertexAttribArray(binormalAttribute);
            glVertexAttribPointer(binormalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, b));
        }
        
        if (tangentAttribute != -1) {
            glEnableVertexAttribArray(tangentAttribute);
            glVertexAttribPointer(tangentAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, tg));
        }
        
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(positionAttribute);
		glDisableVertexAttribArray(normalAttribute);
		if (texCoordAttribute != -1) glDisableVertexAttribArray(texCoordAttribute);
        
        if (binormalAttribute != -1) glDisableVertexAttribArray(binormalAttribute);
        
        if (tangentAttribute != -1) glDisableVertexAttribArray(tangentAttribute);
	}
};

struct Entity {
	Transform transform;
	Geometry geometry;
	Entity() {
	};
	void Draw(Matrix4 &eyeInverse, GLuint positionAttribute, GLuint normalAttribute,
		GLuint modelviewMatrixUniformLocation, GLuint normalMatrixUniformLocation, GLuint texCoordAttribute = -1) {
		// create modelview matrix
		Matrix4 modelViewMatrix;

		modelViewMatrix = eyeInverse * modelViewMatrix * transform.createMatrix();
		modelViewMatrix.writeToColumnMajorMatrix(glmatrix);
		// create normal matrix
		Matrix4 invModelMatrix = inv(modelViewMatrix);
		invModelMatrix(0, 3) = 0; invModelMatrix(1, 3) = 0; invModelMatrix(2, 3) = 0;
		Matrix4 normalMatrix = transpose(invModelMatrix);

		normalMatrix.writeToColumnMajorMatrix(glmatrixNormal);

		// set the model view and normal matrices to the uniforms locations
		glUniformMatrix4fv(modelviewMatrixUniformLocation, 1, false, glmatrix);
		glUniformMatrix4fv(normalMatrixUniformLocation, 1, false, glmatrixNormal);
		geometry.Draw(positionAttribute, normalAttribute, texCoordAttribute);
	}
};




void getCube(std::vector<VertexPNTBTG> &vtx, std::vector<unsigned short> &idx, int size) {
	int vbLen, ibLen;
	getCubeVbIbLen(vbLen, ibLen);
	vtx = vector<VertexPNTBTG>(vbLen);
	idx = vector<unsigned short>(ibLen);
	makeCube(size, vtx.begin(), idx.begin());
}

void getPlane(std::vector<VertexPNTBTG> &vtx, std::vector<unsigned short> &idx, int size) {
	int vbLen, ibLen;
	getPlaneVbIbLen(vbLen, ibLen);
	vtx = vector<VertexPNTBTG>(vbLen);
	idx = vector<unsigned short>(ibLen);
	makePlane(size, vtx.begin(), idx.begin());
}

void getSphere(std::vector<VertexPNTBTG> &vtx, std::vector<unsigned short> &idx, double size, int slices = 8, int stacks = 8) {
	int vbLen, ibLen;
	getSphereVbIbLen(slices, stacks, vbLen, ibLen);
	vtx = vector<VertexPNTBTG>(vbLen);
	idx = vector<unsigned short>(ibLen);
	makeSphere(size, slices, stacks, vtx.begin(), idx.begin());
}

void getMeshFloor(vector<VertexPNTBTG> &vtx, vector<unsigned short> &idx, double size, int row, int col) {
	int vbLen = row * col * 4;
	int ibLen = row * col * 6;
	vtx = vector<VertexPNTBTG>(vbLen);
	idx = vector<unsigned short>(ibLen);

	vector<VertexPNTBTG> planeVdx = vector<VertexPNTBTG>(4);
	vector<unsigned short> planeIdx = vector<unsigned short>(6);
	makePlane(size, planeVdx.begin(), planeIdx.begin());
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {

			for (int k = 0; k < 4; k++) {
				VertexPNTBTG temp = planeVdx[k];
				temp.p += Cvec3f((col / 2 - j) * size, 0, (row / 2 - i) *size);
				vtx[i * row * 4 + j * 4 + k] = temp;
			}
			for (int k = 0; k < 6; k++) {
				idx[i * row * 6 + j * 6 + k] = planeIdx[k] + (i * row + j) * 4;
			}
		}

	}
}

void generateBuffers(Entity &entity, vector<VertexPNTBTG> &vtx, vector<unsigned short> &idx) {
	entity.geometry.numIndices = idx.size();
	glGenBuffers(1, &entity.geometry.vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, entity.geometry.vertexBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPNTBTG) * vtx.size(), vtx.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &entity.geometry.indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity.geometry.indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * idx.size(), idx.data(), GL_STATIC_DRAW);
}

vector<GLfloat*> color;
GLfloat* rgb(int r, int g, int b) {
	GLfloat *c = new GLfloat[3];
	c[0] = r / 255.0f;
	c[1] = g / 255.0f;
	c[2] = b / 255.0f;
	color.push_back(c);
	return c;
}


void randomRGB() {
	GLfloat *c = new GLfloat[3];

	for (int i = 0; i < 3; i++) {
		c[i] = ((double)rand() / RAND_MAX);
	}
	color.push_back(c);
}

Cvec3 calculatePositionVec3(Cvec3 translation, Quat rotation) {
	Cvec4 position = quatToMatrix(rotation) * Cvec4(translation[0], translation[1], translation[2], 0.0);
	return Cvec3(position[0], position[1], position[2]);
}


void loadObjFile(const std::string &fileName, std::vector<VertexPNTBTG> &outVertices, std::vector<unsigned
	short> &outIndices) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str(), NULL, true);

	if (ret) {
		for (int i = 0; i < attrib.vertices.size(); i += 3) {
			VertexPNTBTG v;
			v.p[0] = attrib.vertices[i];
			v.p[1] = attrib.vertices[i + 1];
			v.p[2] = attrib.vertices[i + 2];
			v.n[0] = attrib.normals[i];
			v.n[1] = attrib.normals[i + 1];
			v.n[2] = attrib.normals[i + 2];
			outVertices.push_back(v);
		}
		for (int i = 0; i < shapes.size(); i++) {
			for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
				outIndices.push_back(shapes[i].mesh.indices[j].vertex_index);
			}
		}
	}
	else {
		std::cout << err << std::endl;
		assert(false);
	}
}

void calculateFaceTangent(const Cvec3f &v1, const Cvec3f &v2, const Cvec3f &v3, const Cvec2f &texCoord1, const Cvec2f &texCoord2,
                          const Cvec2f &texCoord3, Cvec3f &tangent, Cvec3f &binormal) {
    Cvec3f side0 = v1 - v2;
    Cvec3f side1 = v3 - v1;
    Cvec3f normal = cross(side1, side0);
    normalize(normal);
    float deltaV0 = texCoord1[1] - texCoord2[1];
    float deltaV1 = texCoord3[1] - texCoord1[1];
    tangent = side0 * deltaV1 - side1 * deltaV0;
    normalize(tangent);
    float deltaU0 = texCoord1[0] - texCoord2[0];
    float deltaU1 = texCoord3[0] - texCoord1[0];
    binormal = side0 * deltaU1 - side1 * deltaU0;
    normalize(binormal);
    Cvec3f tangentCross = cross(tangent, binormal);
    if (dot(tangentCross, normal) < 0.0f) {
        tangent = tangent * -1;
    }
}

void loadObjFileWithTex(const string &fileName, vector<VertexPNTBTG> &outVertices,
	vector<unsigned short> &outIndices) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str(), NULL, true);
	if (ret) {
		for (int i = 0; i < shapes.size(); i++) {
			for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
				unsigned int vertexOffset = shapes[i].mesh.indices[j].vertex_index * 3;
				unsigned int normalOffset = shapes[i].mesh.indices[j].normal_index * 3;
				unsigned int texOffset = shapes[i].mesh.indices[j].texcoord_index * 2;
				VertexPNTBTG v;
				v.p[0] = attrib.vertices[vertexOffset];
				v.p[1] = attrib.vertices[vertexOffset + 1];
				v.p[2] = attrib.vertices[vertexOffset + 2];
				v.n[0] = attrib.normals[normalOffset];
				v.n[1] = attrib.normals[normalOffset + 1];
				v.n[2] = attrib.normals[normalOffset + 2];
                v.t[0] = attrib.texcoords[texOffset];
                v.t[1] = 1.0 - attrib.texcoords[texOffset + 1];
                outVertices.push_back(v);
                outIndices.push_back(outVertices.size() - 1);
			}
		}
    } else {
		std::cout << err << std::endl;
		assert(false);
	}
    
    // calculate Binormal and tangent
    for(int i=0; i < outVertices.size(); i += 3) {
        Cvec3f tangent;
        Cvec3f binormal;
        calculateFaceTangent(outVertices[i].p, outVertices[i+1].p, outVertices[i+2].p,
                             outVertices[i].t, outVertices[i+1].t, outVertices[i+2].t, tangent, binormal);
        outVertices[i].tg = tangent;
        outVertices[i+1].tg = tangent;
        outVertices[i+2].tg = tangent;
        outVertices[i].b = binormal;
        outVertices[i+1].b = binormal;
        outVertices[i+2].b = binormal;
    }
}

void setShaderLight(Shader x) {
    // Lights Init
    
    glUniform3f(x.setUniform("lights[0].lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(x.setUniform("lights[0].lightPosition"), -50.0f, -50.0f, 10.0f);
    glUniform3f(x.setUniform("lights[0].specularLightColor"), 1.0f, 1.0f, 1.0f);
    
    glUniform3f(x.setUniform("lights[1].lightColor"), 1.0f, 0.8f, 1.0f);
    glUniform3f(x.setUniform("lights[1].lightPosition"), 50.0f, 50.0f, 10.0f);
    glUniform3f(x.setUniform("lights[1].specularLightColor"), 0.2f, 0.1f, 0.2f);
    
    
}
#endif // !UTILITIES_H
