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
		Text( std::string _s, float _x, float _y, float _size ) : s(_s), x(_x), y(_y), size(_size), used(true) {}
		std::string s;
		float x, y;
		float size;
		bool used;
	};
	buffer< Text > texts;
	std::map<int,int> char_textures;

	struct Hud : public Rectangle
	{
		Hud( Rectangle& _r, int _texture ) : Rectangle(_r), texture(_texture) {}
		int texture;
	};
	buffer< Hud > hud;
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
	void Zoom( float _z ) { viewMatrix[3][2] = _z - 5.f; }
	float GetZoom() { return viewMatrix[3][2] + 5.f; }
	void MoveX( float _distance ) { viewMatrix[3][0] += _distance; }
	void MoveY( float _distance ) { viewMatrix[3][1] += _distance; }

	std::pair< float, float > GetIngameCoordinates( float _x, float _y );

	int AddText( std::string _s, float _x = 0, float _y = 0, float _size = 1 );
	void RemoveText( int _i );
	void RemoveTopText();
	void MoveText( int _i, float __x, float __y );
	void MoveTopText( float _x, float _y );

	int AddRectangle( int _texture, float _scale, float _x = 0, float _y = 0, float _rotation = 0, float _scale_x = 0 );
	void RemoveRectangle( int _i );
	void MoveRectangle( int _i, float _x, float _y );
	void ResizeRectangle( int _i, float _scale, float _scale_x = 0 );
	Rectangle& GetRectangle( int _i ) { return hud.v[_i]; }

	void Initialize();
	void Update();
};

