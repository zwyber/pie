#include <stdio.h>
#include <stdlib.h>


#include "../framework.h"

int main()
{
    int width = 1600;
    int height = 900;
    Object* A = new Object;
    Object* B = new Object;

    Window window = Window(width, height);
    window.pixRatio = 12;

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
    Universe universe(width/window.pixRatio, height/window.pixRatio);

    // Add them to the universe
    universe.add_object(A);
    universe.add_object(B);

    vec2d posA;
    vec2d posB;

    // Set the buffer clear color to:
    glClearColor(0.3, 0.2, 0.2, 1.0);
	do{
        // Clear the buffers to set values (in our case only colour buffer needs to be cleared)
        glClear(GL_COLOR_BUFFER_BIT);
        window.drawGrid(window.pixRatio);
        window.drawObjectList(universe.objects);
        universe.physics_runtime_iteration();
        posA = A->get_position();
        posB = B->get_position();
        // std::cout << "Position of A: " << posA[0] << "," << posA[1] << " Position of B: " << posB[0] << "," << posB[1] << ";" << std::endl;

        // Swap buffers
		glfwSwapBuffers(window.GLFWpointer);
		glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window.GLFWpointer, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window.GLFWpointer) == 0 );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

