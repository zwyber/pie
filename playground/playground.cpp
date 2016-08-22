#include <stdio.h>
#include <stdlib.h>


#include "../framework.h"

int main()
{
    int width = 600;
    int height = 400;
    Object* A = new Player;

    Window window = Window(width,height,NULL,30,vis::AUTO_SIZE_UNIVERSE);

    // Set them apart, and on a collision course
    A->set_position(2, 0);
    A->set_velocity(0,0);
    A->set_radius(1);
    A->set_mass(6);
    A->bouncyness = 1;



    // Generate a universe
    Universe universe(width/window.pixRatio, height/window.pixRatio);
    window.bindUniverse(&universe);
    // Add them to the universe
    universe.add_object(A);

    Player* player = dynamic_cast<Player*>(A);

    window.fps = 60;

    // Set the buffer clear color to:
    glClearColor(0.3, 0.2, 0.2, 1.0);
	do{
        // Clear the buffers to set values (in our case only colour buffer needs to be cleared)
        glClear(GL_COLOR_BUFFER_BIT);
        // window.drawGrid(window.pixRatio);
        window.drawObjectList(universe.objects);
        universe.simulate_one_time_unit(window.fps);

        // Check if we want to add an object
        if ( glfwGetMouseButton(window.GLFWpointer, GLFW_MOUSE_BUTTON_LEFT) ) {
            double xpos, ypos;
            glfwGetCursorPos(window.GLFWpointer, &xpos, &ypos);
            vec2d cursor_pos = {xpos, ypos};

            // Translate to universe position
            vec2d pos = window.px_to_length(cursor_pos);

            // Spawn an object there
            Object* obj = universe.add_object();
            obj->set_position(pos);
            obj->set_radius(0.4);
            obj->bouncyness = 0.9;
            obj->set_mass(0.4);

            // Halt the program for a bit
            usleep(100000);

        }

        if ( glfwGetKey(window.GLFWpointer, GLFW_KEY_M) ) {
            player->thruster_force += 20;

            std::cout << player->thruster_force << std::endl;
            usleep(100000);
        }

        if ( glfwGetKey(window.GLFWpointer, GLFW_KEY_N) ) {
            player->thruster_force -= 20;
            usleep(100000);
        }




        // Swap buffers
		glfwSwapBuffers(window.GLFWpointer);
		glfwPollEvents();

        window.pace_frame();

    } // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window.GLFWpointer, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window.GLFWpointer) == 0 );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    std::cout << "There were " << universe.objects.size() << " objects!";

    return 0;
}

