#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include "Message.h"
#include <arpa/inet.h>
#include<netinet/in.h>
#include <mysql/mysql.h>
 
using namespace std;
 
#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
#define PORT 50101 // Будем использовать этот номер порта

class Server {
private:
	MYSQL mysql; // Дескриптор соединения c MySql
	MYSQL_RES* res;
	MYSQL_ROW row;
	struct sockaddr_in serveraddress, client;
	socklen_t length;
	int socket_file_descriptor;
	int connection;
	int bind_status;
	int connection_status;
	char message[MESSAGE_LENGTH];
	
	const string channel = "#all";
public:
	void initialize();
	void work();
	void finish();
};