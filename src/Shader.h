#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void Use();
	void Set_Unirofm_b(const std::string& name, bool value) const;
	void Set_Uniform_i(const std::string& name, int value) const;
	void Set_Uniform_f(const std::string& name, float vlaue) const;
};

#endif