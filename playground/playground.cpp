#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <common/shader.hpp>
#include <external/glfw-3.1.2/deps/linmath.h>

#include "../framework.h"

//GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

void drawFilledCircle(vec2d &pos, GLfloat &r, int num_segments, GLfloat &screenWtHRatio, std::array<double,4> Color)
{
    GLfloat theta = 2.0f / double(num_segments) * PI ;
    GLfloat c = cosf(theta);//precalculate the sine and cosine
    GLfloat s = sinf(theta);
    GLfloat t;

    GLfloat x = r;//we start at angle = 0
    GLfloat y = 0;

    glBegin(GL_TRIANGLE_FAN);
    glColor4d(Color[0], Color[1], Color[2], Color[3]);
    glVertex2f(x/screenWtHRatio +pos[0], y +pos[1]); // center of circle
    for(int ii = 0; ii < num_segments; ii++)
    {
        glVertex2f(x/screenWtHRatio + pos[0], y + pos[1]);//output vertex

        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    glEnd();
}

void drawObjectList(std::vector<Object> &objects){
    GLFWwindow* CurrentWindow = glfwGetCurrentContext();
    int Width;
    int Height;
    glfwGetWindowSize(CurrentWindow, &Width, &Height);
    GLfloat WtHratio = (GLfloat)Width/Height;
    for(int ii = 0; ii < objects.size(); ii++){
        GLfloat radius = objects[ii].get_radius()/Height;
        vec2d position = objects[ii].get_position();
        position[0] /= Width;
        position[1] /= Height;
        drawFilledCircle(position, radius, 100, WtHratio, objects[ii].get_colour());
    }
}

int main( void )
{
    int width = 1024;
    int height = 768;

    Object A;
    Object B;
    // Set positions and velocities
    A.set_position(50, 20);
    B.set_position(-1000, -700);

    A.set_velocity(-2,-1);
    B.set_velocity(1,1);

    A.set_radius(50);
    B.set_radius(20);
    // Generate a universe
    Universe universe(width, height);

    universe.add_object(A);
    universe.add_object(B);
    // Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow( width, height, "Playground", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	do{

        drawObjectList(universe.objects);

        // Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

