#include "Application.h"
#include <iostream>

int main() {
    OpenGLApp app;

    if (!app.Initialize()) {
        std::cerr << "Failed to initialize application" << std::endl;
        return -1;
    }

    app.Run();

    return 0;
}