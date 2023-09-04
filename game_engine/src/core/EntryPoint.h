#pragma once
#include "Application.h"


int main() {
    {
        Application* app = Application::CreateApplication();
        app->Run();
        delete app;
    }
    return EXIT_SUCCESS;
}