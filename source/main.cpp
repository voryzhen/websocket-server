#include "RVWSServer.h"

int main(int argc, char** argv)
{
    std::make_unique<RVWSServer>("127.0.0.1", 3030)->start();
    return 0;
}