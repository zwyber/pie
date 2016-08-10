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
void DrawGrid(int stepSize)
{
    //give stepSize in px
    GLFWwindow* CurrentWindow = glfwGetCurrentContext();

    int Width;
    int Height;

    glfwGetWindowSize(CurrentWindow, &Width, &Height);
    glBegin(GL_LINES);
    glColor3d(0.75, 0.75, 0.75);
    for(int i=0;i<=Width/2;i+=stepSize/2)
    {
        glVertex2d((double)i*2.0/Width,-1.0);
        glVertex2d((double)i*2.0/Width,1.0);
        glVertex2d((double)-i*2.0/Width,-1.0);
        glVertex2d((double)-i*2.0/Width,1.0);
    }
    for(int i=0;i<=Height/2;i+=stepSize/2)
    {
        glVertex2d(-1.0,(double)i*2.0/Height);
        glVertex2d(1.0,(double)i*2.0/Height);
        glVertex2d(-1.0,(double)-i*2.0/Height);
        glVertex2d(1.0,(double)-i*2.0/Height);
    }
    glEnd();
}
void drawFilledCircle(vec2d &pos, GLdouble &r, int num_segments, GLdouble &screenWtHRatio, std::array<double,4> Color)
{
    GLdouble phi = 2.0f / double(num_segments) * PI ;
    GLdouble c = cosf(phi);//precalculate the sine and cosine
    GLdouble s = sinf(phi);
    GLdouble t;

    GLdouble x = r;//we start at angle = 0
    GLdouble y = 0;

    //Start a surface (triangle fan)
    glBegin(GL_TRIANGLE_FAN);
    glColor4d(Color[0], Color[1], Color[2], Color[3]);
    glVertex2d(x/screenWtHRatio +pos[0], y +pos[1]); // center of circle
    for(int ii = 0; ii < num_segments; ii++)
    {
        glVertex2d(x/screenWtHRatio + pos[0], y + pos[1]);//output vertex

        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    glEnd();
}

void drawObjectList(std::vector<Object> &objects, double Uwidth, double Uheight){
    GLFWwindow* CurrentWindow = glfwGetCurrentContext();
    int Width;
    int Height;
    glfwGetWindowSize(CurrentWindow, &Width, &Height);
    GLdouble WtHratio = (GLdouble)Width/Height;
    for(int ii = 0; ii < objects.size(); ii++){
        GLdouble radius = 2.0*objects[ii].get_radius()/Uheight;
        vec2d position = objects[ii].get_position();
        position[0] *= 2.0/Uwidth;
        position[1] *= 2.0/Uheight;
        drawFilledCircle(position, radius, 100, WtHratio, objects[ii].get_colour());
        //std::cout << "Position of object "<< ii << ": " << position[0] << "," << position[1] << "  \t";
    }
    //std::cout << std::endl;
}

int main( void )
{
    int width = 720;
    int height = 480;

    Object A;
    Object B;

    // Set them apart, and on a collision course
    A.set_position(5, -4);
    A.set_velocity(3, 0);
    A.set_mass(2);
    //A.bouncyness = 0.9;

    B.set_position(-5, 0);
    B.set_velocity(0, 0);
    B.set_mass(2);
    //B.bouncyness = 0.9;

    // Generate a universe
    Universe universe(width/25.0, height/25.0);

    // Add them to the universe
    id_type A_id = universe.add_object(A);
    id_type B_id = universe.add_object(B);

    Object* Ap = universe.get_object_by_id(A_id);
    Object* Bp = universe.get_object_by_id(B_id);

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
    unsigned IT = 0;
    vec2d posA;
    vec2d posB;
	do{

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawGrid(50);
        drawObjectList(universe.objects,universe.width, universe.height);
        universe.physics_runtime_iteration();
        posA = Ap->get_position();
        posB = Bp->get_position();
        //std::cout << "Position of A: " << posA[0] << "," << posA[1] << " Position of B: " << posB[0] << "," << posB[1] << ";" << std::endl;

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

