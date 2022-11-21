#include "Client.h"

int main() {
	Client client;
	client.initialize();
	client.loginMenu();
	client.chatMenu();
	
	client.finish();
}