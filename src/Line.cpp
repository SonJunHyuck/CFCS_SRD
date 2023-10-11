#include "Line.h"

Line::Line(glm::vec3 pos1, glm::vec3 pos2)
{
	setup(pos1, pos2);
}

void Line::setup(glm::vec3 pos1, glm::vec3 pos2)
{
	std::vector<glm::vec4> vertexList;
	std::vector<glm::vec3> normalLists;

	vertexList.push_back(glm::vec4(pos1, 1.0)); vertexList.push_back(glm::vec4(pos2, 1.0));

	// Setup a VAO (Vertex Array Object)
	glGenVertexArrays(1, &vaoHandle);  // (create count, hander variable)
	glBindVertexArray(vaoHandle);  // activate VAO

	glGenBuffers(1, &vbo_vertex_positions);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexList.size() * 4, vertexList.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0, // attribute number
		4, // data count of each vertex
		GL_FLOAT, // data type
		GL_FALSE, // is normalized data?
		0, // vertex point offset
		0); // offset of Starting reference
	glEnableVertexAttribArray(0);  // no. 0 attribute enable

	// Close VAO
	glBindVertexArray(0);
}

void Line::draw()
{
	glBindVertexArray(vaoHandle);
	glLineWidth(5.0f);
	glDrawArrays(
		GL_LINES,  // primitive,
		0,  // starting vertex
		2);  // count of vertices
}