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
#include "graphic.h"
#include "logic.h"

class exit_success : std::exception {};

class Input
{
private:
	Graphic& graphic;
	Logic& logic;
	float mouse_wheel_z, mouse_wheel_scale, scale, line_x, line_y;
	bool create, line_lock;
	enum { select, build_road, build_farm, build_city } state;
public:
	Input( Graphic& _graphic, Logic& _logic );

	void Initialize();
	void Update();
};

