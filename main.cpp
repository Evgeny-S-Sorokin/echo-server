#include <iostream>

#include "co_server/co_server.hpp"

int main()
{
	echo_servers::CoServer server;

	server.Run();

	return 0;
}
