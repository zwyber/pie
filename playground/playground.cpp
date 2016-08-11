#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <glfw3.h>
GLFWwindow* window;

#include "../framework.h"

int main()
{
    int width = 1600;
    int height = 900;

    Object* A = new Object;
    Object* B = new Object;

    // Set them apart, and on a collision course
    A->set_position(2, 0);
    A->set_velocity(2,10);
    A->set_mass(3);
    A->bouncyness = 1;

    B->set_position(2, 2);
    B->set_velocity(0, -1);
    B->set_mass(3);
    B->set_radius(1);

    // Generate a universe
    Universe universe(width/25.0, height/25.0);

    // Add them to the universe
    universe.add_object(A);
    universe.add_object(B);

    vec2d posA;
    vec2d posB;
    window = initNewWindow(width, height);
    glClearColor(0.3, 0.2, 0.2, 1.0);
	do{

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawGrid(50);
        drawObjectList(universe.objects,universe.width, universe.height);
        universe.physics_runtime_iteration();
        posA = A->get_position();
        posB = B->get_position();
        // std::cout << "Position of A: " << posA[0] << "," << posA[1] << " Position of B: " << posB[0] << "," << posB[1] << ";" << std::endl;

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

