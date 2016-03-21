#include "Application.h"

int main(int, char**)
{
    std::unique_ptr<Application> application = std::make_unique<Application>("Hairstylist", 1000, 500);
    application->run();

    return 0;
}
