#include "CheckerdBoard.h"
#include "common.h"

CheckerdBoard::CheckerdBoard()
{
	setup(10.0f, 10);
}

CheckerdBoard::CheckerdBoard(float size, int nSquares)
{
	setup(size, nSquares);
}

CheckerdBoard::CheckerdBoard(glm::vec2 floorSize)
{
	setup(floorSize.x, floorSize.y);
}

CheckerdBoard::CheckerdBoard(glm::vec2 size, std::vector<glm::vec3> colors, glm::vec2 slices)
{
	setup(size, colors, slices);
}

CheckerdBoard::~CheckerdBoard()
{
}

void CheckerdBoard::setup(float size, int nSquares)
{
	verticesCount = 0;

	std::vector <glm::vec4> vlists;
	std::vector <glm::vec3> nlists;
	std::vector <glm::vec3> clists;
	std::vector <GLushort> iLists;

	glm::vec3 floorColor1 = VEC_ONE * 0.0f;
	glm::vec3 floorColor2 = VEC_ONE * 0.3f;

	float maxX = size / 2, maxY = size / 2;
	float minX = -size / 2, minY = -size / 2;

	int x, y, i;
	float xp, yp, xd, yd;
	xd = (maxX - minX) / ((float)nSquares);
	yd = (maxY - minY) / ((float)nSquares);

	int pointCount = 0;
	for (x = 0, xp = minX; x < nSquares; x++, xp += xd) {
		for (y = 0, yp = minY, i = x; y < nSquares; y++, i++, yp += yd) {

			glm::vec3 floorColor;
			if (i % 2 == 1) {
				floorColor = floorColor1;
			}
			else {
				floorColor = floorColor2;
			}

			// color List
			clists.push_back(floorColor);
			clists.push_back(floorColor);
			clists.push_back(floorColor);
			clists.push_back(floorColor);

			// normal List
			nlists.push_back(glm::vec3(0, 1, 0));
			nlists.push_back(glm::vec3(0, 1, 0));
			nlists.push_back(glm::vec3(0, 1, 0));
			nlists.push_back(glm::vec3(0, 1, 0));

			// triangle of cell's position
			vlists.push_back(glm::vec4(xp, -1.0f, yp, 1));
			vlists.push_back(glm::vec4(xp, -1.0f, yp + yd, 1));
			vlists.push_back(glm::vec4(xp + xd, -1.0f, yp + yd, 1));
			vlists.push_back(glm::vec4(xp + xd, -1.0f, yp, 1));

			iLists.push_back(0 + 4 * y + x * nSquares * 4);
			iLists.push_back(1 + 4 * y + x * nSquares * 4);
			iLists.push_back(2 + 4 * y + x * nSquares * 4);

			iLists.push_back(0 + 4 * y + x * nSquares * 4);
			iLists.push_back(2 + 4 * y + x * nSquares * 4);
			iLists.push_back(3 + 4 * y + x * nSquares * 4);

			// each cell's verticesCount is 6
			verticesCount += 6;
		}
	}

	// Setup a VAO (Vertex Array Object)
	glGenVertexArrays(1, &vaoHandle);  // (create count, hander variable)
	glBindVertexArray(vaoHandle);  // activate VAO

	int attribute_index = 0;
	int vertex_count = 4;
	glGenBuffers(1, &vbo_vertex_cell_positions);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vlists.size() * 4, vlists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_index, vertex_count, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // no. 0 attribute enable

	attribute_index = 1;
	vertex_count = 3;
	glGenBuffers(1, &vbo_vertex_cell_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nlists.size() * 3, nlists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_index, vertex_count, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // no. 1 attribute enable

	attribute_index = 2;
	vertex_count = 3;
	glGenBuffers(1, &vbo_vertex_cell_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * clists.size() * 3, clists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_index, vertex_count, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);  // no. 1 attribute enable

	glGenBuffers(1, &ibo_cell_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cell_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * iLists.size(), iLists.data(), GL_STATIC_DRAW);

	// Close VAO
	glBindVertexArray(0);
}

void CheckerdBoard::setup(glm::vec2 size, std::vector<glm::vec3> colors, glm::vec2 slices)
{
	verticesCount = 0;

	std::vector <glm::vec4> vlists;
	std::vector <glm::vec3> nlists;
	std::vector <glm::vec3> clists;
	std::vector <GLushort> iLists;

	// parameters:
	float maxX = size.x / 2, maxY = -size.y / 2;
	float minX = -size.x / 2, minY = size.y / 2;

	int x, y, i;
	float xp, yp, xd, yd;
	xd = (maxX - minX) / ((float)slices.x);
	yd = (maxY - minY) / ((float)slices.y);

	int pointCount = 0;

	for (x = 0, xp = minX; x < slices.x; x++, xp += xd) {
		for (y = 0, yp = minY, i = x * slices.y; y < slices.y; y++, i++, yp += yd) {
			glm::vec3 floorColor = colors[i];

			// color List
			clists.push_back(floorColor);
			clists.push_back(floorColor);
			clists.push_back(floorColor);
			clists.push_back(floorColor);

			// normal List
			nlists.push_back(glm::vec3(0, 1, 0));
			nlists.push_back(glm::vec3(0, 1, 0));
			nlists.push_back(glm::vec3(0, 1, 0));
			nlists.push_back(glm::vec3(0, 1, 0));

			// triangle of cell's position
			vlists.push_back(glm::vec4(xp, -1.0f, yp, 1));
			vlists.push_back(glm::vec4(xp, -1.0f, yp + yd, 1));
			vlists.push_back(glm::vec4(xp + xd, -1.0f, yp + yd, 1));
			vlists.push_back(glm::vec4(xp + xd, -1.0f, yp, 1));

			// counter clock wise
			iLists.push_back(0 + 4 * y + x * slices.y * 4);
			iLists.push_back(2 + 4 * y + x * slices.y * 4);
			iLists.push_back(1 + 4 * y + x * slices.y * 4);

			iLists.push_back(0 + 4 * y + x * slices.y * 4);
			iLists.push_back(3 + 4 * y + x * slices.y * 4);
			iLists.push_back(2 + 4 * y + x * slices.y * 4);

			// each cell's verticesCount is 6
			verticesCount += 6;
		}
	}

	// Setup a VAO (Vertex Array Object)
	glGenVertexArrays(1, &vaoHandle);  // (create count, hander variable)
	glBindVertexArray(vaoHandle);  // activate VAO (VAO�� �۾� ������ �˸�)
	
	int attribute_index = 0;
	int vertex_count = 4;
	glGenBuffers(1, &vbo_vertex_cell_positions);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vlists.size() * 4, vlists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_index, vertex_count, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // no. 0 attribute enable

	attribute_index = 1;
	vertex_count = 3;
	glGenBuffers(1, &vbo_vertex_cell_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nlists.size() * 3, nlists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_index, vertex_count, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // no. 1 attribute enable

	attribute_index = 2;
	vertex_count = 3;
	glGenBuffers(1, &vbo_vertex_cell_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * clists.size() * 3, clists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_index, vertex_count, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);  // no. 1 attribute enable

	glGenBuffers(1, &ibo_cell_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cell_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * iLists.size(), iLists.data(), GL_STATIC_DRAW);

	// Close VAO
	glBindVertexArray(0);
}

void CheckerdBoard::draw()
{
	glBindVertexArray(vaoHandle);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}