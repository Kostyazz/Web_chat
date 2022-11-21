#pragma once
#include <string>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;
#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
#define PORT 50100 // Будем использовать этот номер порта
#define IP_ADDRESS "127.0.0.1"
 

class Client {
private:
	int socket_file_descriptor, connection;
	struct sockaddr_in serveraddress, client;
	char message[MESSAGE_LENGTH];
	
	string currentLogin = "";
	const string channel = "#all";
	
	
public:
	void initialize();
	void work();
	void loginMenu();
	void chatMenu();
	string talk_to_server(string& msg);
	int login();
	int signUp();
	void finish();
	void logout();
};