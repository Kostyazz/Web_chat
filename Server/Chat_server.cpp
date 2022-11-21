#include "Server.h"

using namespace std;

int main()
{
	Server server;
	server.initialize();
	server.work();

	server.finish();
}
