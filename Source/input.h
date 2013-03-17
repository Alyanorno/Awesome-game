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
		int army_selected, army_select_rectangle;
		int select_rectangle;
		Type select;
		Resource selected_resource;
		bool lock, lock_cart_production, lock_soldier_production, lock_select;
		Select( Graphic& _graphic, Logic& _logic, float& _mouse_wheel );
		~Select();
		void Input( float _x, float _y );
	};
	template < class T > struct Build : public State
	{
		float scale;
		int rectangle;
		bool expand;
		Build( Graphic& _graphic, Logic& _logic, float& _mouse_wheel );
		~Build();
		void Input( float _x, float _y );
	};
	template <> struct Build<Road> : public State
	{
		int from, to;
		int rectangle;
		bool expand;
		Build( Graphic& _graphic, Logic& _logic, float& _mouse_wheel );
		~Build();
		void Input( float _x, float _y );
	};
	template <> struct Build<City> : public State
	{
		float scale;
		int rectangle;
		int farm;
		bool expand;
		int expand_size;
		Build( Graphic& _graphic, Logic& _logic, float& _mouse_wheel );
		~Build();
		void Input( float _x, float _y );
	};
	std::unique_ptr< State > state;

	Graphic& graphic;
	Logic& logic;
	float mouse_wheel, mouse_wheel_z;
public:
	Input( Graphic& _graphic, Logic& _logic );

	void Initialize();
	void Update();
};

