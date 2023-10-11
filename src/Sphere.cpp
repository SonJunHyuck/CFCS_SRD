#include "Sphere.h"

#include <glm//gtc/constants.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

Sphere::Sphere()
{
}

Sphere::~Sphere()
{
}

Sphere::Sphere(float rad, GLuint sl, GLuint st) :
radius(rad), slices(sl), stacks(st)
{
	nVerts = (slices + 1) * (stacks + 1);
	elements = (slices * 2 * (stacks - 1)) * 3;

	// Verts
	float * v = new float[3 * nVerts];
	// Normals
	float * n = new float[3 * nVerts];  
	// Tex coords
	float * tex = new float[2 * nVerts];    //we don't use it now
	// Index
	unsigned int * el = new unsigned int[elements];  // index

	// Generate the vertex data
	generateVerts(v, n, tex, el);
	
	//create vao, vbo and ibo here... (We didn't use std::vector here...)

	setup(v, n, el);

	delete[] v;
	delete[] n;
	delete[] el;
	delete[] tex;

}

void Sphere::setup(float* verts, float* norms, unsigned int* el)
{
	// Setup a VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);  // (create count, hander variable)
	glBindVertexArray(VAO);  // activate VAO

	int attribute_index = 0;
	int vertex_count = 3;
	glGenBuffers(1, &VBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (nVerts) * 3, verts, GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_index, vertex_count, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // no. 0 attribute enable

	attribute_index = 1;
	vertex_count = 3;
	glGenBuffers(1, &VBO_normal);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (nVerts) * 3, norms, GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_index, vertex_count, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // no. 0 attribute enable

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elements, el, GL_STATIC_DRAW);

	// Close VAO
	glBindVertexArray(0);
}

void Sphere::draw() 
{
	glBindVertexArray(VAO);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void Sphere::generateVerts(float * verts, float * norms, float * tex, GLuint * el)
{
	// Generate positions and normals
	GLfloat theta, phi;
	GLfloat thetaFac = glm::two_pi<float>() / slices;
	GLfloat phiFac = glm::pi<float>() / stacks;
	GLfloat nx, ny, nz, s, t;
	GLuint idx = 0, tIdx = 0;

	for (GLuint i = 0; i <= slices; i++) {
		theta = i * thetaFac;
		s = (GLfloat)i / slices;
		for (GLuint j = 0; j <= stacks; j++) {
			phi = j * phiFac;
			t = (GLfloat)j / stacks;
			nx = sinf(phi) * cosf(theta);
			ny = sinf(phi) * sinf(theta);
			nz = cosf(phi);
			verts[idx] = radius * nx; verts[idx + 1] = radius * ny; verts[idx + 2] = radius * nz;
			norms[idx] = nx; norms[idx + 1] = ny; norms[idx + 2] = nz;
			idx += 3;

			tex[tIdx] = s;
			tex[tIdx + 1] = t;
			tIdx += 2;
		}
	}

	// Generate the element list
	idx = 0;
	for (GLuint i = 0; i < slices; i++) {
		GLuint stackStart = i * (stacks + 1);
		GLuint nextStackStart = (i + 1) * (stacks + 1);
		for (GLuint j = 0; j < stacks; j++) {
			if (j == 0) {
				el[idx] = stackStart;
				el[idx + 1] = stackStart + 1;
				el[idx + 2] = nextStackStart + 1;
				idx += 3;
			}
			else if (j == stacks - 1) {
				el[idx] = stackStart + j;
				el[idx + 1] = stackStart + j + 1;
				el[idx + 2] = nextStackStart + j;
				idx += 3;
			}
			else {
				el[idx] = stackStart + j;
				el[idx + 1] = stackStart + j + 1;
				el[idx + 2] = nextStackStart + j + 1;
				el[idx + 3] = nextStackStart + j;
				el[idx + 4] = stackStart + j;
				el[idx + 5] = nextStackStart + j + 1;
				idx += 6;
			}
		}
	}
}

int Sphere::getVertexArrayHandle() 
{
	return this->VAO;
}

