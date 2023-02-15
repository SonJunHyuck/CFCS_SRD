#include "CheckerdBoard.h"

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

	// center로부터 시작하니까 max와 min은 size의 절반
	float maxX = size / 2, maxY = size / 2;
	float minX = -size / 2, minY = -size / 2;

	int x, y, i;
	float xp, yp, xd, yd;
	xd = (maxX - minX) / ((float)nSquares);  // x를 size를 n 개의 slice로
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
	glBindVertexArray(vaoHandle);  // activate VAO (VAO에 작업 시작을 알림)

	glGenBuffers(1, &vbo_vertex_cell_positions);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vlists.size() * 4, vlists.data(), GL_STATIC_DRAW);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nlists.size() * 3, nlists.data(), GL_STATIC_DRAW);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * clists.size() * 3, clists.data(), GL_STATIC_DRAW);
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * iLists.size(), iLists.data(), GL_STATIC_DRAW);

	// Close VAO (VAO에 작업 끝)
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
	// 드로잉과 area 객체 인덱스가 맞지 않는것을 해결해야함
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

			// y축이 내려갈 수록 양수가 되기 때문에 ccw로 그려야해서 인덱스 순서 변경
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
	glBindVertexArray(vaoHandle);  // activate VAO (VAO에 작업 시작을 알림)

	glGenBuffers(1, &vbo_vertex_cell_positions);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cell_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vlists.size() * 4, vlists.data(), GL_STATIC_DRAW);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nlists.size() * 3, nlists.data(), GL_STATIC_DRAW);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * clists.size() * 3, clists.data(), GL_STATIC_DRAW);
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * iLists.size(), iLists.data(), GL_STATIC_DRAW);

	// Close VAO (VAO에 작업 끝)
	glBindVertexArray(0);
}

void CheckerdBoard::draw()
{
	//glBindVertexArray(vaoHandle);  // vao bind (activate의 의미)

	//// 그리기
	//glDrawArrays(
	//	GL_TRIANGLES,  // primitive,
	//	0,  // 시작 vertex
	//	verticesCount);  // count of vertices (중요)

	glBindVertexArray(vaoHandle);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}