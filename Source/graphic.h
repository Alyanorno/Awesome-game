#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#define GLFW_DLL
#include "..\GL\include\glew.h"
#include "..\GL\include\glfw.h"
#include "..\glm\glm.hpp"
#include "..\glm\gtc\matrix_transform.hpp"
#include "..\glm\gtc\matrix_inverse.hpp"
#include "globals.h"


class Graphic
{
private:
	struct Texture
	{
		void Set( int _size, int _width, int _height )
		{
			width = _width;
			height = _height;
			t.resize( _size );
		}
		char& operator[]( int n ) { return t[n]; }
		std::vector<char> t;
		int width, height;

		void LoadBmp( std::string name );
	};
	GLuint glTexture[2];

	glm::mat4 viewMatrix;
	GLuint Vbo[3];
	void DrawLines( glm::mat4& projectionMatrix );
	void DrawRectangle( Rectangle& r );

	enum ShaderType { Vertex, Fragment, Geometry };
	GLuint shaderProgram, shaderLine;

	GLuint LoadShader( std::string name, ShaderType type );
	GLuint CreateShader( std::string vertex, std::string fragment );

public:
	void Zoom( float _z ) { viewMatrix[3][2] = _z; }
	void MoveX( float _distance ) { viewMatrix[3][0] += _distance; }
	void MoveY( float _distance ) { viewMatrix[3][1] += _distance; }

	std::pair< float, float > GetIngameCoordinates( float _x, float _y );

	void Initialize();
	void Update();
};

