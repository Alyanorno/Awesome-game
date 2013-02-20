#pragma once

#include <vector>
#include <string>
#include <map>
#include <fstream>
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
	GLuint glTexture[46];

	struct Text
	{
		Text( std::string _s, float _x, float _y, float _size ) : s(_s), x(_x), y(_y), size(_size) {}
		std::string s;
		float x, y;
		float size;
	};
	std::vector< Text > texts;
	std::map<int,int> char_textures;

	glm::mat4 viewMatrix;
	GLuint Vbo[3];
	void DrawLines( glm::mat4& projectionMatrix );
	void DrawRectangle( Rectangle& r );
	void DrawText( glm::mat4& projectionMatrix );

	enum ShaderType { Vertex, Fragment, Geometry };
	GLuint shaderProgram, shaderLine, shaderText;

	GLuint LoadShader( std::string name, ShaderType type );
	GLuint CreateShader( std::string vertex, std::string fragment );
public:
	void Zoom( float _z ) { viewMatrix[3][2] = _z; }
	void MoveX( float _distance ) { viewMatrix[3][0] += _distance; }
	void MoveY( float _distance ) { viewMatrix[3][1] += _distance; }

	std::pair< float, float > GetIngameCoordinates( float _x, float _y );

	void AddText( std::string _s, float _x = 0, float _y = 0, float _size = 1 );
	void RemoveText( float _x, float _y );
	void RemoveTopText();
	void MoveText( float _x, float _y, float __x, float __y );
	void MoveTopText( float _x, float _y );

	void Initialize();
	void Update();
};

