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
float deapth = 0.f;
void Graphic::DrawRectangle( Rectangle& r )
{
	if( !r.used )
		return;

	glm::mat4 modelMatrix( glm::mat4( 1.0f ) );

	// translate
//	modelMatrix[3] = glm::vec4( r.x, r.y, deapth, 1.f );
	modelMatrix[3][0] = r.x;
	modelMatrix[3][1] = r.y;
	modelMatrix[3][2] = deapth;

	// rotate
	if( r.rotation != 0 )
		modelMatrix = glm::rotate( modelMatrix, r.rotation, glm::vec3( 0.f, 0.f, -1.f ) );

	// scale
	modelMatrix = glm::scale( modelMatrix, glm::vec3( r.scale, r.scale_x ? r.scale_x: r.scale, 0.f ) );

	glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "viewMatrix" ), 1, GL_FALSE, &viewMatrix[0][0] );
	glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "modelMatrix" ), 1, GL_FALSE, &modelMatrix[0][0] );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}
void Graphic::DrawText( glm::mat4& projectionMatrix )
{
	glUseProgram( shaderText );

	glUniformMatrix4fv( glGetUniformLocation(shaderText, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0] );

	glEnableVertexAttribArray(3);
	glDisableVertexAttribArray(1);

	for( int i(0); i < texts.size(); i++ )
	{
		Text& t( texts[i] );
		if( t.used == false )
			continue;
		std::string& s(t.s);
		glm::mat4 modelMatrix( glm::mat4( 1.0f ) );
		modelMatrix[3][2] = 0.05;
		for( int i(0), next_char(0), new_line(0); i < t.s.size(); i++ )
		{
			if( s[i] == ' ' )
			{
				next_char++;
				continue;
			}
			else if( s[i] == '\n' )
			{
				next_char = 0;
				new_line++;
				continue;
			}
			glBindTexture( GL_TEXTURE_2D, glTexture[ char_textures[ s[i] ] ] );

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


int Graphic::AddText( std::string _s, float _x, float _y, float _size )
{
	return texts.insert( Text( _s, _x, _y, _size ) );
}
void Graphic::RemoveText( int _i )
{
	texts.erase( _i );
}
void Graphic::RemoveTopText()
{
	texts.pop_back();
}
void Graphic::MoveText( int _i, float __x, float __y )
{
	texts[_i].x = __x;
	texts[_i].y = __y;
}
void Graphic::MoveTopText( float _x, float _y )
{
	texts.back().x = _x;
	texts.back().y = _y;
}


int Graphic::AddRectangle( int _texture, float _scale, float _x, float _y, float _rotation, float _scale_x )
{
	return hud.insert( Hud( Rectangle( _x, _y, _scale, _rotation, _scale_x ), _texture ) );
}
void Graphic::RemoveRectangle( int _i )
{
	hud.erase( _i );
}
void Graphic::MoveRectangle( int _i, float _x, float _y )
{
	hud[_i].x = _x;
	hud[_i].y = _y;
}
void Graphic::ResizeRectangle( int _i, float _scale, float _scale_x )
{
	hud[_i].scale = _scale;
	hud[_i].scale_x = _scale_x;
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
	glGenTextures( 46, glTexture );

	int i = 0;

	#define DEFAULT_TEXTURE_OPTIONS \
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); \
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); \
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, t.width, t.height, 0, GL_BGR, GL_UNSIGNED_BYTE, &t[0] );

	t.LoadBmp( "road.bmp" );
	assert( i == (int)Type::Road );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	DEFAULT_TEXTURE_OPTIONS

	t.LoadBmp( "farm.bmp" );
	assert( i == (int)Type::Farm );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	DEFAULT_TEXTURE_OPTIONS

	t.LoadBmp( "city.bmp" );
	assert( i == (int)Type::City );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	DEFAULT_TEXTURE_OPTIONS

	t.LoadBmp( "army.bmp" );
	assert( i == (int)Type::Army );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	DEFAULT_TEXTURE_OPTIONS

	t.LoadBmp( "structure.bmp" );
	assert( i == (int)Type::Structure );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	DEFAULT_TEXTURE_OPTIONS
	
	// Load char numbers
	for( char j(0); j < 10; j++ )
	{
		char_textures[ std::to_string(j)[0] ] = i;
		t.LoadBmp( "Font/" + std::to_string(j) + ".bmp" );
		glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
		DEFAULT_TEXTURE_OPTIONS
	}
	// Load char letters
	for( char j(0); j < 26; j++ )
	{
		char_textures[ 'A'+j ] = i;
		std::stringstream s;
		s << "Font/" << (char)('A'+j) << ".bmp";
		t.LoadBmp( s.str() );
		glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
		DEFAULT_TEXTURE_OPTIONS
	}
	char_textures[ '/' ] = i;
	t.LoadBmp( "Font/forward_slash.bmp" );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	DEFAULT_TEXTURE_OPTIONS

	char_textures[ '+' ] = i;
	t.LoadBmp( "Font/plus.bmp" );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	DEFAULT_TEXTURE_OPTIONS

	char_textures[ '-' ] = i;
	t.LoadBmp( "Font/minus.bmp" );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	DEFAULT_TEXTURE_OPTIONS

	char_textures[ '#' ] = i;
	t.LoadBmp( "food.bmp" );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	DEFAULT_TEXTURE_OPTIONS

	char_textures[ '$' ] = i;
	t.LoadBmp( "money.bmp" );
	glBindTexture( GL_TEXTURE_2D, glTexture[i++] );
	DEFAULT_TEXTURE_OPTIONS

	#undef DEFAULT_TEXTURE_OPTIONS


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

	//DrawLines( projectionMatrix );

	glUseProgram( shaderProgram );
	glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0] );

	for( int i(0); i < rectangles.size(); i++ )
	{
		deapth = (float)i / 100;
		glBindTexture( GL_TEXTURE_2D, glTexture[ i ] );
		for( int j(0); j < rectangles[i].size(); j++ )
		{
			DrawRectangle( rectangles[i][j] );
			deapth += 0.001f;
		}
	}


	// 2D
	glDepthMask( GL_FALSE );
	glDisable( GL_DEPTH_TEST );

	for( int i(0); i < hud.size(); i++ )
	{
		glBindTexture( GL_TEXTURE_2D, glTexture[ hud[i].texture ] );
		DrawRectangle( hud[i] );
	}

	DrawText( projectionMatrix );

	glDepthMask( GL_TRUE );
	glEnable( GL_DEPTH_TEST );

	glUseProgram(0);

	glfwSwapBuffers();
}

