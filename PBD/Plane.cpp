#include "Plane.h"

Plane::Plane()
{
}
Plane::Plane(float size, int squares)
{
	set_up(size, squares);
}

void Plane::set_up(float size, int squares)
{
	int verticesCount = 0;

	glm::vec3 floorColor1 = glm::vec3(0, 0, 0);
	glm::vec3 floorColor2 = glm::vec3(1, 1, 1);

	// parameters:
	float maxX = size / 2, maxY = size / 2;
	float minX = -size / 2, minY = -size / 2;

	int x, y, i;
	float xp, yp, xd, yd;
	xd = (maxX - minX) / ((float)squares);
	yd = (maxY - minY) / ((float)squares);

	int pointCount = 0;
	for (x = 0, xp = minX; x < squares; x++, xp += xd) {
		for (y = 0, yp = minY, i = x; y < squares; y++, i++, yp += yd) {
			glm::vec3 floorColor;

			if (i % 2 == 1) {
				floorColor = floorColor1;
			}
			else {
				floorColor = floorColor2;
			}

			// color List
			colors.push_back(glm::vec3(floorColor[0], floorColor[1], floorColor[2]));
			colors.push_back(glm::vec3(floorColor[0], floorColor[1], floorColor[2]));
			colors.push_back(glm::vec3(floorColor[0], floorColor[1], floorColor[2]));
			colors.push_back(glm::vec3(floorColor[0], floorColor[1], floorColor[2]));

			// normal List
			normals.push_back(glm::vec3(0, 1, 0));
			normals.push_back(glm::vec3(0, 1, 0));
			normals.push_back(glm::vec3(0, 1, 0));
			normals.push_back(glm::vec3(0, 1, 0));

			// triangle of cell's position
			positions.push_back(glm::vec4(xp, 0, yp, 1));
			positions.push_back(glm::vec4(xp, 0, yp + yd, 1));
			positions.push_back(glm::vec4(xp + xd, 0, yp + yd, 1));
			positions.push_back(glm::vec4(xp + xd, 0, yp, 1));

			indices.push_back(0 + 4 * y + x * squares * 4);
			indices.push_back(1 + 4 * y + x * squares * 4);
			indices.push_back(2 + 4 * y + x * squares * 4);

			indices.push_back(0 + 4 * y + x * squares * 4);
			indices.push_back(2 + 4 * y + x * squares * 4);
			indices.push_back(3 + 4 * y + x * squares * 4);

			// each cell's verticesCount is 6
			verticesCount += 6;
		}
	}
	
	// Setup a VAO (Vertex Array Object)
	glGenVertexArrays(1, &vaoHandle);  // (create count, hander variable)
	glBindVertexArray(vaoHandle);  // activate VAO (VAO에 작업 시작을 알림)

	glGenBuffers(1, &vbo_vertex_cell_positions);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size() * 4, positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0, // attribute number (atti 위치 지정)
		4, // data count of each vertex
		GL_FLOAT, // data type
		GL_FALSE, // is normalized data?
		0, // 하나의 vertex와 다음 vertex 데이터 간의 사이 offset
		0); // offset of Starting reference
	glEnableVertexAttribArray(0);  // no. 0 attribute enable

	glGenBuffers(1, &vbo_vertex_cell_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size() * 3, normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		1, // attribute number (atti 위치 지정)
		3, // data count of each vertex
		GL_FLOAT, // data type
		GL_FALSE, // is normalized data?
		0, // 하나의 vertex와 다음 vertex 데이터 간의 사이 offset
		0); // offset of Starting reference
	glEnableVertexAttribArray(1);  // no. 1 attribute enable

	glGenBuffers(1, &vbo_vertex_cell_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size() * 3, colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		2, // attribute number (atti 위치 지정)
		3, // data count of each vertex
		GL_FLOAT, // data type
		GL_FALSE, // is normalized data?
		0, // 하나의 vertex와 다음 vertex 데이터 간의 사이 offset
		0); // offset of Starting reference
	glEnableVertexAttribArray(2);  // no. 1 attribute enable

	glGenBuffers(1, &ibo_cell_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cell_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Close VAO (VAO에 작업 끝)
	glBindVertexArray(0);

	
}

void Plane::draw()
{
	glBindVertexArray(vaoHandle);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}