#include "graphic.h"


void Graphic::Texture::LoadBmp( std::string name )
{
	std::fstream in;
	in.open( name.c_str(), std::ios::in | std::ios::binary );
	assert( in.is_open() );

	unsigned int offset;
	int width, height, size;
	unsigned short bits_per_pixel;

	in.seekg( sizeof( short ) + sizeof( int ) + sizeof( short ) * 2 );
	in.read( (char*)&offset, sizeof( offset) );
	in.seekg( in.tellp() + (std::fstream::pos_type)sizeof( int ) );
	in.read( (char*)&width, sizeof( width ) );
	in.read( (char*)&height, sizeof( height ) );
	in.seekg( in.tellp() + (std::fstream::pos_type)sizeof( short ) );
	in.read( (char*)&bits_per_pixel, sizeof( bits_per_pixel ) );

	assert( bits_per_pixel == 24 ); // only supports 24 bits

	size = bits_per_pixel / 8 * width * height;
	Set( size, width, height );

	in.seekg( offset );
	in.read( (char*)&(t[0]), size );

	in.close();
}
void Graphic::DrawLines( glm::mat4& projectionMatrix )
{
	glUseProgram( shaderLine );

	glUniformMatrix4fv( glGetUniformLocation(shaderLine, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0] );

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glUniformMatrix4fv( glGetUniformLocation( shaderLine, "viewMatrix" ), 1, GL_FALSE, &viewMatrix[0][0] );

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, lines.size() * 2 * 3 * sizeof(float), &lines[0], GL_DYNAMIC_DRAW);

	glDrawArrays( GL_LINES, 4, lines.size() * 2 );	

	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glUseProgram( 0 );
}

float vertexs[] = {
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f
};
float normals[] = {
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f
};
float textureCoordinates[] = {
	1.0f, 0.0f, 
	1.0f, 1.0f, 
	0.0f, 0.0f,
	0.0f, 1.0f
};
void Graphic::DrawRectangle( Rectangle& r )
{
	if( r.used == false )
		return;

	glBindTexture( GL_TEXTURE_2D, glTexture[r.texture] );

	glm::mat4 modelMatrix( glm::mat4( 1.0f ) );
	modelMatrix[3][0] = r.x;
	modelMatrix[3][1] = r.y;
	if( r.texture == 1 )
		modelMatrix[3][2] = 0.001; // TODO: Remove temporary code.

	modelMatrix[0][0] *= r.scale;
	modelMatrix[1][1] *= r.scale;
	modelMatrix[2][2] *= r.scale;

	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "modelViewMatrix" ), 1, GL_FALSE, &modelViewMatrix[0][0] );

	glm::mat3 tempMatrix = glm::inverseTranspose( (glm::mat3)modelViewMatrix );
	glUniformMatrix3fv( glGetUniformLocation( shaderProgram, "normalInverseTranspose"), 1, GL_FALSE, &tempMatrix[0][0] );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}
void Graphic::DrawText( glm::mat4& projectionMatrix )
{
	glUseProgram( shaderText );

	glUniformMatrix4fv( glGetUniformLocation(shaderText, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0] );

	glEnableVertexAttribArray(3);
	glDisableVertexAttribArray(1);

	for each( Text t in texts )
	{
		const char* c = t.s.c_str();
		glm::mat4 modelMatrix( glm::mat4( 1.0f ) );
		modelMatrix[3][2] = 0.002;
		for( int i(0), next_char(0), new_line(0); i < t.s.size(); i++ )
		{
			if( c[i] == ' ' )
			{
				next_char++;
				continue;
			}
			else if( c[i] == '\n' )
			{
				next_char = 0;
				new_line++;
				continue;
			}
			glBindTexture( GL_TEXTURE_2D, glTexture[ char_textures[ c[i] ] ] );

			modelMatrix[3][0] = t.x + next_char * t.size + t.size / 2;
			modelMatrix[3][1] = t.y - new_line * t.size - t.size / 2;
			next_char++;

			modelMatrix[0][0] = modelMatrix[1][1] = modelMatrix[2][2] = t.size;

			glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
			glUniformMatrix4fv( glGetUniformLocation( shaderText, "modelViewMatrix" ), 1, GL_FALSE, &modelViewMatrix[0][0] );

			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		}
	}

	glEnableVertexAttribArray(1);
	glDisableVertexAttribArray(3);

	glUseProgram( 0 );
}

GLuint Graphic::LoadShader( std::string name, ShaderType type )
{
	std::fstream in( name.c_str() );
	std::string program( ( std::istreambuf_iterator<char>( in ) ), std::istreambuf_iterator<char>() );
	in.close();

	GLuint shader;
	switch( type )
	{
		case Vertex:
			shader = glCreateShader( GL_VERTEX_SHADER );
			break;
		case Fragment:
			shader = glCreateShader( GL_FRAGMENT_SHADER );
			break;
		case Geometry:
			shader = glCreateShader( GL_GEOMETRY_SHADER );
			break;
	}
	assert( shader );

	const char* ptr = program.c_str();
	glShaderSource( shader, 1, &ptr, 0 );
	glCompileShader( shader );

	int e;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &e );
	if( !e ) {
		int l, t;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &l );
		char* error = new char[l];
		glGetShaderInfoLog( shader, l, &t, error );
		std::cout << std::endl << error;
		std::string err( error );
		delete error;
		throw err;
	}

	return shader;
}

GLuint Graphic::CreateShader( std::string vertex, std::string fragment )
{
	GLuint vertexShader = LoadShader( vertex, Vertex );
	GLuint fragmentShader = LoadShader( fragment, Fragment );

	GLuint result = glCreateProgram();

	glAttachShader( result, vertexShader );
	glAttachShader( result, fragmentShader );

	glLinkProgram( result );

	GLint e;
	glGetProgramiv( result, GL_LINK_STATUS, &e );
	if( e == GL_FALSE ) {
		int l, t;
		glGetProgramiv( result, GL_INFO_LOG_LENGTH, &l );
		char* error = new char[l];
		glGetProgramInfoLog( result, l, &t, error );
		std::string err( error );
		delete error;
		throw err;
	}

	return result;
}

std::pair< float, float > Graphic::GetIngameCoordinates( float _x, float _y )
{
	int width, height;
	glfwGetWindowSize( &width, &height );

	float t_x = ((float)(_x - width / 2) / width) * -viewMatrix[3][2] / 5 * width * ((float)height / width) / 100 - viewMatrix[3][0];
	float t_y = (1 - (float)(_y + height / 2) / height) * -viewMatrix[3][2] / 5 * height * ((float)height / width) / 100 - viewMatrix[3][1];
	return std::make_pair( t_x, t_y );
}

void Graphic::AddText( std::string _s, float _x, float _y, float _size )
{
	texts.push_back( Text( _s, _x, _y, _size ) );
}
void Graphic::RemoveText( float _x, float _y )
{
	for( int i(0); i < texts.size(); i++ )
		if( texts[i].x == _x && texts[i].y == _y )
		{
			texts.erase( texts.begin() + i );
			return;
		}
}
void Graphic::RemoveTopText()
{
	texts.pop_back();
}
void Graphic::MoveText( float _x, float _y, float __x, float __y )
{
	for( int i(0); i < texts.size(); i++ )
		if( texts[i].x == _x && texts[i].y == _y )
		{
			texts[i].x = __x;
			texts[i].y = __y;
			return;
		}
}
void Graphic::MoveTopText( float _x, float _y )
{
	texts.back().x = _x;
	texts.back().y = _y;
}


void Graphic::Initialize()
{
	GLuint Vao;

	glfwInit();

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwOpenWindow( 800, 600, 0, 0, 0, 0, 0, 0, GLFW_WINDOW );

	glewInit();

	glfwSetWindowTitle( "Hej!" );

	glEnable( GL_DEPTH_TEST ); 
	glEnable( GL_CULL_FACE );
	glEnable( GL_BLEND );

	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glClearColor( 0.5f, 0.5f, 0.5f, 0.5f );
	glDepthFunc( GL_LESS );

	glm::vec3 eye( 0.f, 0.f, 5.f ), centre( 0.f, 0.f, 0.f ), up( 0.f, 1.f, 0.f );
	viewMatrix = glm::lookAt(eye, centre, up);


	shaderProgram = CreateShader( "Source/shader.vertex", "Source/shader.fragment" );
	shaderLine = CreateShader( "Source/line.vertex", "Source/line.fragment" );
	shaderText = CreateShader( "Source/text.vertex", "Source/text.fragment" );

	Texture t;
	glGenTextures( 40, glTexture );

	int i = 0;
	
	t.LoadBmp( "test.bmp" );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, t.width, t.height, 0, GL_BGR, GL_UNSIGNED_BYTE, &t[0] );

	t.LoadBmp( "farm.bmp" );
	assert( i == (int)Textures::Farm );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, t.width, t.height, 0, GL_BGR, GL_UNSIGNED_BYTE, &t[0] );

	t.LoadBmp( "city.bmp" );
	assert( i == (int)Textures::City );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, t.width, t.height, 0, GL_BGR, GL_UNSIGNED_BYTE, &t[0] );

	t.LoadBmp( "structure.bmp" );
	assert( i == (int)Textures::Structure );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, t.width, t.height, 0, GL_BGR, GL_UNSIGNED_BYTE, &t[0] );

	
	// Load char numbers
	for( char j(0); j < 10; j++ )
	{
		char_textures[ std::to_string(j)[0] ] = i;
		t.LoadBmp( "Font/" + std::to_string(j) + ".bmp" );
		glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, t.width, t.height, 0, GL_BGR, GL_UNSIGNED_BYTE, &t[0] );
	}
	// Load char letters
	for( char j(0); j < 26; j++ )
	{
		char_textures[ 'A'+j ] = i;
		std::stringstream s;
		s << "Font/" << (char)('A'+j) << ".bmp";
		t.LoadBmp( s.str() );
		glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, t.width, t.height, 0, GL_BGR, GL_UNSIGNED_BYTE, &t[0] );
	}
	char_textures[ '/' ] = i;
	t.LoadBmp( "Font/forward_slash.bmp" );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, t.width, t.height, 0, GL_BGR, GL_UNSIGNED_BYTE, &t[0] );


	lines.push_back( Line( vertexs[0], vertexs[1], vertexs[3], vertexs[4] ) );
	lines.push_back( Line( vertexs[6], vertexs[7], vertexs[9], vertexs[10] ) );



	// Vertex, normal, texture, text
	glGenBuffers( 3, Vbo );
	int size = 4 * sizeof(float);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, lines.size() * 2 * 3 * sizeof(float), &lines[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, size * 3, &normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, size * 2, &textureCoordinates[0], GL_STATIC_DRAW);


	glGenVertexArrays(1, &Vao);
	glBindVertexArray(Vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	GLubyte* null = 0;

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, null);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, null);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, null);


	glBindVertexArray(Vao);
}


void Graphic::Update()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	int width, height;
	glfwGetWindowSize( &width, &height );

	float nearClip = 1.0f, farClip = 1000.0f, fovDeg = 45.0f, aspect = (float)width / (float)height;
	glm::mat4 projectionMatrix = glm::perspective(fovDeg, aspect, nearClip, farClip);

	DrawLines( projectionMatrix );

	glUseProgram( shaderProgram );
	glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0] );

	for( int i(rectangles.v.size()-1); i >= 0; i-- )
		DrawRectangle( rectangles.v[i] );

	DrawText( projectionMatrix );

	glUseProgram(0);

	glfwSwapBuffers();
}

