//
// Created by paul on 7/15/16.
//

#include "framework.h"
#include "lib/testing.h"
#include "lib/game.h"

int main() {

    Window thisGame = Window();

    GLuint menuTex = loadDDS("menuDummy.DDS");
    TextureShader menuMultiTex(menuTex);

    loadMenuResources(&menuMultiTex);

    std::cout << "exit code " << show_menu(&thisGame, &menuMultiTex) << endl;
    return 0;

}