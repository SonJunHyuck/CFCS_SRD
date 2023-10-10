#pragma once

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <assimp/mesh.h>
#include <assimp/scene.h>

#include "Loader.h"

class Mesh
{
public:
	struct MeshEntry 
	{
		enum BUFFERS 
		{
			VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER, INDEX_BUFFER
		};

		GLuint vao;
		GLuint vbo[4];

		std::vector<glm::vec3> vertices;

		unsigned int elementCount;
		aiColor3D dcolor;
		aiColor3D acolor;
		aiColor3D scolor;
		float shininessStrength;
		MeshEntry(aiMesh *mesh, const aiScene* scene, Mesh * m);
		~MeshEntry();

		Mesh * parent;

		void render();	
	};
		
public:
	Mesh(const char *filename, ShaderProgram * sh);
	~Mesh(void);

	std::vector<Mesh::MeshEntry*> meshEntries;

	ShaderProgram * shader;
	void draw();	
};