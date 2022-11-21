#include "Client.h"

using namespace std;

void Client::initialize() {
    // Создадим сокет
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_file_descriptor == -1){
        cout << "Creation of Socket failed!" << endl;
        exit(1);
    }
 
    // Установим адрес сервера
    serveraddress.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    // Зададим номер порта
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;
    // Установим соединение с сервером
    connection = connect(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
    if(connection == -1){
        cout << "Connection with the server failed.!" << endl;
        exit(1);
    }
}

string Client::talk_to_server(string& msg) {
	strcpy(message, msg.c_str());
    if ((strncmp(message, "/logout", 7)) == 0) {
        write(socket_file_descriptor, message, sizeof(message));
        cout << "Client Exit." << endl;
    }
    ssize_t bytes = write(socket_file_descriptor, message, sizeof(message));
    // Если передали >= 0  байт, значит пересылка прошла успешно
	bzero(message, sizeof(message));
	// Ждем ответа от сервера
	read(socket_file_descriptor, message, sizeof(message));
	msg = message;
    bzero(message, sizeof(message));
	return msg;
 }

void Client::loginMenu()
{
	char command = 0;
	// Successful loginResult is 0;
	int loginResult = -1; 
	do {
		cout << "Please enter l to login, s to sign up, q to quit" << endl;
		cin >> command;
		switch (command) {
			case 'l':
				loginResult = login();
				break;
			case 's':
				loginResult = signUp();
				break;
			case 'q':
				exit(0);
				break;
			default:
				break;
		}
	} while (loginResult);
	cout << "Welcome, " << currentLogin << endl;
	//chatMenu();
}

int Client::login()
{
	string login;
	string password;
	cout << "Login: ";
	cin >> login;
	cout << "Password: ";
	cin >> password;
	string msg = "/login " + login + " " + password;
	int result = stoi(talk_to_server(msg));
	if (result == 0) {
		currentLogin = login;
	}
	return result;
}

int Client::signUp() {
	
}

void Client::finish() {
	// закрываем сокет, завершаем соединение
    close(socket_file_descriptor);
}