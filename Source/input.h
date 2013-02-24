#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
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
	struct State
	{
		Graphic& graphic;
		Logic& logic;
		float& mouse_wheel;
		bool create;
		State( Graphic& _graphic, Logic& _logic, float& _mouse_wheel )
			: graphic(_graphic), logic(_logic), mouse_wheel(_mouse_wheel), create(false)
		{}
		virtual void Input( float _x, float _y ) {}
		virtual ~State() {}
	};
	struct Select : public State
	{
		int army_selected;
		bool lock;
		Select( Graphic& _graphic, Logic& _logic, float& _mouse_wheel );
		~Select();
		void Input( float _x, float _y );
	};
	struct BuildRoad : public State
	{
		int from, to;
		BuildRoad( Graphic& _graphic, Logic& _logic, float& _mouse_wheel );
		~BuildRoad();
		void Input( float _x, float _y );
	};
	struct BuildFarm : public State
	{
		float scale;
		BuildFarm( Graphic& _graphic, Logic& _logic, float& _mouse_wheel );
		~BuildFarm();
		void Input( float _x, float _y );
	};
	struct BuildCity : public State
	{
		float scale;
		BuildCity( Graphic& _graphic, Logic& _logic, float& _mouse_wheel );
		~BuildCity();
		void Input( float _x, float _y );
	};
	std::unique_ptr< State > state;

	Graphic& graphic;
	Logic& logic;
	float mouse_wheel, mouse_wheel_z;
	static const int inputRectangle = 0, selectRectangle = 1;
public:
	Input( Graphic& _graphic, Logic& _logic );

	void Initialize();
	void Update();
};

