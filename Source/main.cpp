#include <fstream>
#include <string>
#include "graphic.h"
#include "input.h"
#include "logic.h"


void Initialize( int argc, char** argv );
void Update();

Graphic graphic;
Logic logic;
Input input(graphic, logic);
std::ofstream out;
int main( int argc, char** argv )
{
	try {
		Initialize( argc, argv );
		while( true )
			Update();
	} catch( std::string error ) {
		out << error << std::endl;
	} catch( exit_success e ) {
	}
	glfwTerminate();
	out.close();

	exit( EXIT_SUCCESS );
};



void Initialize( int argc, char** argv )
{
	out.open( "log.txt" );
	graphic.Initialize();
	input.Initialize();
	logic.Initialize();
}

void Update()
{
	graphic.Update();
	input.Update();
	logic.Update();
}

