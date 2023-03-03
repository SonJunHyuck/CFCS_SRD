#include "ColorCube.h"

ColorCube::ColorCube()
{
	setup();
}

ColorCube::ColorCube(float width, float height, float length)
{
	setup(width, height, length);
}

ColorCube::~ColorCube()
{

}

void ColorCube::setup()
{
#pragma region IBO
	std::vector <glm::vec4> vertexList;
	std::vector <glm::vec3> normalLists;

	vertexList.push_back(glm::vec4(-1.0, 2.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 2.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 4.0, 1.0, 1.0));
	vertexList.push_back(glm::vec4(-1.0, 2.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 4.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(-1.0, 4.0, 1.0, 1.0));
	vertexList.push_back(glm::vec4(1.0, 2.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 2.0, -1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 4.0, -1.0, 1.0));
	vertexList.push_back(glm::vec4(1.0, 2.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 4.0, -1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 4.0, 1.0, 1.0));
	vertexList.push_back(glm::vec4(-1.0, 2.0, -1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 2.0, -1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 4.0, -1.0, 1.0));
	vertexList.push_back(glm::vec4(-1.0, 2.0, -1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 4.0, -1.0, 1.0)); vertexList.push_back(glm::vec4(-1.0, 4.0, -1.0, 1.0));
	vertexList.push_back(glm::vec4(-1.0, 2.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(-1.0, 2.0, -1.0, 1.0)); vertexList.push_back(glm::vec4(-1.0, 4.0, -1.0, 1.0));
	vertexList.push_back(glm::vec4(-1.0, 2.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(-1.0, 4.0, -1.0, 1.0)); vertexList.push_back(glm::vec4(-1.0, 4.0, 1.0, 1.0));
	vertexList.push_back(glm::vec4(-1.0, 2.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 2.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 2.0, -1.0, 1.0));
	vertexList.push_back(glm::vec4(-1.0, 2.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 2.0, -1.0, 1.0)); vertexList.push_back(glm::vec4(-1.0, 2.0, -1.0, 1.0));
	vertexList.push_back(glm::vec4(-1.0, 4.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 4.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 4.0, -1.0, 1.0));
	vertexList.push_back(glm::vec4(-1.0, 4.0, 1.0, 1.0)); vertexList.push_back(glm::vec4(1.0, 4.0, -1.0, 1.0)); vertexList.push_back(glm::vec4(-1.0, 4.0, -1.0, 1.0));

	//// front
	//vertexList.push_back(glm::vec4(-1.0, 5.0, 1.0, 1.0));
	//vertexList.push_back(glm::vec4(1.0, 5.0, 1.0, 1.0));
	//vertexList.push_back(glm::vec4(1.0, 7.0, 1.0, 1.0));
	//vertexList.push_back(glm::vec4(-1.0, 7.0, 1.0, 1.0));
	//							 
	//// back						 
	//vertexList.push_back(glm::vec4(-1.0, 5.0, -1.0, 1.0));
	//vertexList.push_back(glm::vec4(1.0, 5.0, -1.0, 1.0));
	//vertexList.push_back(glm::vec4(1.0, 7.0, -1.0, 1.0));
	//vertexList.push_back(glm::vec4(-1.0, 7.0, -1.0, 1.0));


	GLushort cube_elements[] = {
0, 1, 2,		0, 2, 3,  // front
1, 5, 6,		1, 6, 2,  // right
4, 5, 6,		4, 6, 7,  // back
0, 4, 7,		0, 7, 3,  // left
0, 1, 5,		0, 5, 4,  // bottom
3, 2, 6,		3, 6, 7,  // top
	};
	
	for (int i = 0; i < 12; i++)
	{
		normalLists.push_back(ComputeNormal(vertexList[0 + i * 3], vertexList[1 + i * 3], vertexList[2 + i * 3]));
		normalLists.push_back(ComputeNormal(vertexList[0 + i * 3], vertexList[1 + i * 3], vertexList[2 + i * 3]));
		normalLists.push_back(ComputeNormal(vertexList[0 + i * 3], vertexList[1 + i * 3], vertexList[2 + i * 3]));
	}

	GLuint ibo_cube_elements;

	// Setup a VAO (Vertex Array Object)
	glGenVertexArrays(2, &vaoHandle);  // (create count, hander variable)
	glBindVertexArray(vaoHandle);  // activate VAO (VAO에 작업 시작을 알림)

	glGenBuffers(1, &vbo_vertex_cube_positions);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cube_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexList.size() * 4, vertexList.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0, // attribute number (atti 위치 지정)
		4, // data count of each vertex
		GL_FLOAT, // data type
		GL_FALSE, // is normalized data?
		0, // 하나의 vertex와 다음 vertex 데이터 간의 사이 offset
		0); // offset of Starting reference
	glEnableVertexAttribArray(0);  // no. 0 attribute enable

	glGenBuffers(1, &vbo_vertex_cube_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cube_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normalLists.size() * 3, normalLists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		1, // attribute number (atti 위치 지정)
		3, // data count of each vertex
		GL_FLOAT, // data type
		GL_FALSE, // is normalized data?
		0, // 하나의 vertex와 다음 vertex 데이터 간의 사이 offset
		0); // offset of Starting reference
	glEnableVertexAttribArray(1);  // no. 0 attribute enable

	/*glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);*/

	// Close VAO (VAO에 작업 끝)
	glBindVertexArray(0);

#pragma endregion
}

void ColorCube::setup(float width, float height, float length)
{
	std::vector <glm::vec4> vertexList;
	std::vector <glm::vec3> normalLists;

	float p_X, p_Y, p_Z;

	p_X = width / 2;
	p_Y = height / 2;
	p_Z = length / 2;

	vertexList.push_back(glm::vec4(-p_X, -p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, -p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, p_Y, p_Z, 1.0));
	vertexList.push_back(glm::vec4(-p_X, -p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(-p_X, p_Y, p_Z, 1.0));
	vertexList.push_back(glm::vec4(p_X, -p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, -p_Y, -p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, p_Y, -p_Z, 1.0));
	vertexList.push_back(glm::vec4(p_X, -p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, p_Y, -p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, p_Y, p_Z, 1.0));
	vertexList.push_back(glm::vec4(-p_X, -p_Y, -p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, -p_Y, -p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, p_Y, -p_Z, 1.0));
	vertexList.push_back(glm::vec4(-p_X, -p_Y, -p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, p_Y, -p_Z, 1.0)); vertexList.push_back(glm::vec4(-p_X, p_Y, -p_Z, 1.0));
	vertexList.push_back(glm::vec4(-p_X, -p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(-p_X, -p_Y, -p_Z, 1.0)); vertexList.push_back(glm::vec4(-p_X, p_Y, -p_Z, 1.0));
	vertexList.push_back(glm::vec4(-p_X, -p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(-p_X, p_Y, -p_Z, 1.0)); vertexList.push_back(glm::vec4(-p_X, p_Y, p_Z, 1.0));
	vertexList.push_back(glm::vec4(-p_X, -p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, -p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, -p_Y, -p_Z, 1.0));
	vertexList.push_back(glm::vec4(-p_X, -p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, -p_Y, -p_Z, 1.0)); vertexList.push_back(glm::vec4(-p_X, -p_Y, -p_Z, 1.0));
	vertexList.push_back(glm::vec4(-p_X, p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, p_Y, -p_Z, 1.0));
	vertexList.push_back(glm::vec4(-p_X, p_Y, p_Z, 1.0)); vertexList.push_back(glm::vec4(p_X, p_Y, -p_Z, 1.0)); vertexList.push_back(glm::vec4(-p_X, p_Y, -p_Z, 1.0));

	GLushort cube_elements[] = {
0, 1, 2,		0, 2, 3,  // front
1, 5, 6,		1, 6, 2,  // right
4, 5, 6,		4, 6, 7,  // back
0, 4, 7,		0, 7, 3,  // left
0, 1, 5,		0, 5, 4,  // bottom
3, 2, 6,		3, 6, 7,  // top
	};

	for (int i = 0; i < 12; i++)
	{
		normalLists.push_back(ComputeNormal(vertexList[0 + i * 3], vertexList[1 + i * 3], vertexList[2 + i * 3]));
		normalLists.push_back(ComputeNormal(vertexList[0 + i * 3], vertexList[1 + i * 3], vertexList[2 + i * 3]));
		normalLists.push_back(ComputeNormal(vertexList[0 + i * 3], vertexList[1 + i * 3], vertexList[2 + i * 3]));
	}

	GLuint ibo_cube_elements;

	// Setup a VAO (Vertex Array Object)
	glGenVertexArrays(2, &vaoHandle);  // (create count, hander variable)
	glBindVertexArray(vaoHandle);  // activate VAO (VAO에 작업 시작을 알림)

	glGenBuffers(1, &vbo_vertex_cube_positions);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cube_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexList.size() * 4, vertexList.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0, // attribute number (atti 위치 지정)
		4, // data count of each vertex
		GL_FLOAT, // data type
		GL_FALSE, // is normalized data?
		0, // 하나의 vertex와 다음 vertex 데이터 간의 사이 offset
		0); // offset of Starting reference
	glEnableVertexAttribArray(0);  // no. 0 attribute enable

	glGenBuffers(1, &vbo_vertex_cube_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_cube_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normalLists.size() * 3, normalLists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		1, // attribute number (atti 위치 지정)
		3, // data count of each vertex
		GL_FLOAT, // data type
		GL_FALSE, // is normalized data?
		0, // 하나의 vertex와 다음 vertex 데이터 간의 사이 offset
		0); // offset of Starting reference
	glEnableVertexAttribArray(1);  // no. 0 attribute enable

	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	// Close VAO (VAO에 작업 끝)
	glBindVertexArray(0);
}

void ColorCube::draw()
{
	glBindVertexArray(vaoHandle);  // vao bind (activate의 의미)

	// 그리기
	glDrawArrays(
		GL_TRIANGLES,  // primitive,
		0,  // 시작 vertex
		36);  // count of vertices (중요)

	/*glBindVertexArray(vaoHandle);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);*/
}

glm::vec3 ColorCube::ComputeNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	glm::vec3 normal;
	glm::vec3 element1;
	glm::vec3 element2;

	glm::normalize(p1);
	glm::normalize(p2);
	glm::normalize(p3);

	element1 = p2 - p1;
	element2 = p3 - p1;

	normal = glm::cross(element1, element2);

	return normal;
}
