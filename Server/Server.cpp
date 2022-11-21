#include "Server.h"
#include <fstream>
#include "exception"

using namespace std;

void Server::initialize() {
	//fill users array with login credentials from "Login.txt"
	//separator is ' '
	try {
		ifstream loginFile;
		loginFile.open("Login.txt");
		if (! loginFile.is_open()) {
			throw runtime_error("ERROR: Login file not found");
		}
		string login;
		string password;
		while (loginFile) {
			loginFile >> login >> password;
			loginFile.ignore();
			for (char c : login) {
				if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9')) {
					throw runtime_error("ERROR: bad credentials in the file");
				}
			}
			if (password.empty()) {
				throw runtime_error("ERROR: bad credentials in the file");
			}
			users[login] = password;
		}
	}
	catch (runtime_error& ex) {
		cerr << ex.what() << endl;
		exit(4);
	}
	
	// Создадим сокет
	try {
		socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
		if(socket_file_descriptor == -1){
			throw runtime_error("ERROR: Socket creation failed!");
		}
	}
	catch (runtime_error& ex) {
		cerr << ex.what() << endl;
		exit(1);
	}
	serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
	// Зададим номер порта для связи
	serveraddress.sin_port = htons(PORT);
	// Используем IPv4
	serveraddress.sin_family = AF_INET;
	// Привяжем сокет
	try {
		bind_status = bind(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
		if(bind_status == -1)  {
			throw runtime_error("ERROR: Socket binding failed!");
		}
	}
	catch (runtime_error& ex) {
		cerr << ex.what() << endl;
		exit(2);
	}
	// Поставим сервер на прием данных
	try {
		connection_status = listen(socket_file_descriptor, 5);
		if(connection_status == -1){
			throw runtime_error("ERROR: Socket is unable to listen for new connections!");
		} else {
			cout << "Server is listening for new connection: " << endl;
		}
		length = sizeof(client);
	}
	catch (runtime_error& ex) {
		cerr << ex.what() << endl;
		exit(3);
	}
}

void Server::work() {
	connection = accept(socket_file_descriptor,(struct sockaddr*)&client, &length);
	if(connection == -1)  {
		throw runtime_error("ERROR: Server is unable to accept the data from client!");
	}	
	
	// Communication Establishment
	while(1){
		bzero(message, MESSAGE_LENGTH);
		read(connection, message, sizeof(message));
		cout << "Data received from client: " <<  message << endl;
		if (strncmp("/logout", message, 7) == 0) {
			cout << "Client Exited." << endl;
			cout << "Server is Exiting..!" << endl;
			break;
		}
		if (strncmp("/login", message, 6) == 0) {
			string msg = message;
			size_t space1 = msg.find(' ');
			size_t space2 = msg.find(' ', space1 + 1);
			string command = msg.substr(0, space1);
			string login = msg.substr(space1 + 1, space2 - space1 - 1);
			string password = msg.substr(space2 + 1, msg.size());
			
			
			bzero(message, MESSAGE_LENGTH);
			if (users[login] == password) {
				strcpy(message, "0");
			} else {
				strcpy(message, "1");
			}
		}
		ssize_t bytes = write(connection, message, sizeof(message));
		// Если передали >= 0  байт, значит пересылка прошла успешно
		if(bytes >= 0)  {
		   cout << "Data successfully sent to the client!" << message << endl;
		}
	}
}

void Server::finish() {
	// закрываем сокет, завершаем соединение
	close(socket_file_descriptor);
}