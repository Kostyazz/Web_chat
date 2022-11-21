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
}
void Client::chatMenu()
{
	//showChat();
	cout << "Enter a message to send it to all. Enter '/dm <username> <message>' to send direct message to another user. Enter '/logout' to logout" << endl;
	cin.ignore();
	string msg;
	int result;
	while (true) {
		getline(cin, msg);
		result = stoi(talk_to_server(msg));
		if (result == 1) {
			cout << "target user not found" << endl;
		}
		if (result = -1) {
			logout();
			break;
		}
	}
}
/*
void Client::showChat()
{
	for (auto msg : messages) {
		if (msg->getFrom() == currentLogin || msg->getTo() == currentLogin || msg->getTo() == channel) {
			cout << msg->getFrom() << " to " << msg->getTo() << ": " << msg->getText() << endl;
		}
	}
}*/

void Client::logout()
{
	currentLogin.clear();
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
	} else {	
		cout << "Pair login-password not found" << endl;
	}
	return result;
}

int Client::signUp() {
	
	string login;
	string password;
	cout << "Please enter your login. Only digits and latin symbols allowed: ";
	cin >> login;
	for (char c : login) {
		if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9')) {
			cout << "Only digits and latin symbols allowed" << endl;
			return 2;
		}
	}
	cout << "Please enter your password. Space symbol not allowed: ";
	cin >> password;
	for (char c : login) {
		if (c == ' ') {
			cout << "Space symbol not allowed" << endl;
			return 2;
		}
	}
	string msg = "/signup " + login + " " + password;
	int result = stoi(talk_to_server(msg));
	if (result == 0) {
		currentLogin = login;
	} else {
		cout << "This login is taken" << endl;		
	}
	return result;
}

void Client::finish() {
	// закрываем сокет, завершаем соединение
    close(socket_file_descriptor);
}