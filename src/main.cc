#include <iostream>
#include <memory>
#include <ncurses.h>
#include <stdexcept>

#include "controller.h"
#include "model.h"
#include "ncurses_display.h"


int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Usage ./vm [filepath]" << std::endl;
        return 1;
    }


    // Instantiate editor
    Editor editor(argv[1]);

    // Construct display and controller
    auto display = std::make_shared<NCurses_Display>(editor);
    auto controller = std::make_shared<Controller>(editor);

    // Pass display and controller to editor
    editor.init(display, controller);

    // Run editor
    try {
        editor.run();
    } catch (const std::exception& e) {
        display->close_display();
        std::cout << e.what() << std::endl;
    }

    return 0;
}
