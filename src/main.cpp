#include <iostream>
#include "lo_server.h"
using namespace std;

int main()
{
    lo lo_server;
    lo_server.tinyweb_start(uv_default_loop(), "127.0.0.1", 8080);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
