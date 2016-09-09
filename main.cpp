//
// Created by paul on 7/15/16.
//

#include "framework.h"
#include "lib/testing.h"
#include "lib/game.h"

int main() {

    Window thisGame = Window();

    std::cout << "exit code " << show_menu(&thisGame) << endl;
    return 0;

}