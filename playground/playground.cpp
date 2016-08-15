#include <stdio.h>
#include <stdlib.h>


#include "../framework.h"

int main()
{
    int width = 600;
    int height = 400;
    Object* A = new Player;
    Object* B = new Object;
    Object* C = new Object;
    Object* D = new Object;

    Window window = Window(width,height,NULL,30,vis::AUTO_SIZE_UNIVERSE);

    // Set them apart, and on a collision course
    A->set_position(2, 0);
    A->set_velocity(0,0);
    A->set_radius(0.5);
    A->set_mass(5);
    A->bouncyness = 1;

    B->set_position(2, 2);
    B->set_velocity(0, -1);
    B->set_mass(2);
    //B->bouncyness = 0.7;

    C->set_position(-2, -2);
    C->set_mass(2);
    //C->bouncyness = 0.7;

    D->set_position(-4, -4);
    D->set_mass(2);
    //D->bouncyness = 0.7;



    // Generate a universe
    Universe universe(width/window.pixRatio, height/window.pixRatio);
    window.bindUniverse(&universe);
    // Add them to the universe
    universe.add_object(A);
    universe.add_object(B);
    universe.add_object(C);
    universe.add_object(D);

    vec2d posA;
    vec2d posB;

    // Set the buffer clear color to:
    glClearColor(0.3, 0.2, 0.2, 1.0);
	do{
        // Clear the buffers to set values (in our case only colour buffer needs to be cleared)
        glClear(GL_COLOR_BUFFER_BIT);
        // window.drawGrid(window.pixRatio);
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

