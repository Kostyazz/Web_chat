#include "Server.h"
#include "exception"
#include <mysql/mysql.h>

using namespace std;

void Server::initialize() {
	mysql_init(&mysql);
 
	if (&mysql == NULL) 	{
		// Если дескриптор не получен — выводим сообщение об ошибке
		cout << "Error: can't create MySQL-descriptor" << endl;
	}
 
	// Подключаемся к серверу
	if (!mysql_real_connect(&mysql, "localhost", "root", "root", "chatdb", 0, NULL, 0)) {
		// Если нет возможности установить соединение с БД выводим сообщение об ошибке
		cout << "Error: can't connect to database " << mysql_error(&mysql) << endl;
	}
	else {
		// Если соединение успешно установлено выводим фразу — "Success!"
		cout << "Success!" << endl;
	}
 
	mysql_set_character_set(&mysql, "utf8");
	
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
	string msg;
	string command;
	string arg;
	string to = "";
	string text = "";
	string currentLogin;
	bool logout = false;
	// Communication Establishment
	while(! logout){
		to = "";
		text = "";
		bzero(message, MESSAGE_LENGTH);
		read(connection, message, sizeof(message));
		cout << "Data received from client: " <<  message << endl;
		if (strncmp("/logout", message, 7) == 0) {
			cout << "Client Exited." << endl;
			cout << "Server is Exiting..!" << endl;
			strcpy(message, "-1");
			logout = true;
		} else if (strncmp("/login", message, 6) == 0) {
			msg = message;
			size_t space1 = msg.find(' ');
			size_t space2 = msg.find(' ', space1 + 1);
			command = msg.substr(0, space1);
			string login = msg.substr(space1 + 1, space2 - space1 - 1);
			string password = msg.substr(space2 + 1, msg.size());
			
			mysql_query(&mysql, string("SELECT 1 FROM users WHERE login = '" + login + "' AND password = '" + password + "'").c_str());
			
			bool login_pass_in_db = false;
			if (res = mysql_store_result(&mysql)) {
				login_pass_in_db = mysql_num_rows(res);
			}
			else
				cout << "Ошибка MySql номер " << mysql_error(&mysql);
			
			bzero(message, MESSAGE_LENGTH);
			if (login_pass_in_db) {
				strcpy(message, "0");
				currentLogin = login;
			} else {
				strcpy(message, "1");
			}
		} else if (strncmp("/signup", message, 7) == 0) {
			msg = message;
			size_t space1 = msg.find(' ');
			size_t space2 = msg.find(' ', space1 + 1);
			string command = msg.substr(0, space1);
			string login = msg.substr(space1 + 1, space2 - space1 - 1);
			string password = msg.substr(space2 + 1, msg.size());
			
			bzero(message, MESSAGE_LENGTH);
			mysql_query(&mysql, ("SELECT 1 FROM users WHERE login = '" + login + "'").c_str());
			
			bool login_in_db = false;
			if (res = mysql_store_result(&mysql)) {
				login_in_db = mysql_num_rows(res);
			}
			else
				cout << "Ошибка MySql номер " << mysql_error(&mysql);
			
			if (login_in_db) {
				strcpy(message, "2");
			} else {
				strcpy(message, "0");
				mysql_query(&mysql, ("INSERT INTO users VALUES('" + login + "', '" + password + "')").c_str());
			}
			
		} else if (strncmp("/dm", message, 3) == 0) {
			msg = message;
			size_t space1 = msg.find(' ');
			size_t space2 = msg.find(' ', space1 + 1);
			command = msg.substr(0, space1);
			arg = msg.substr(space1 + 1, space2 - space1 - 1);
			text = msg.substr(space2 + 1, msg.size());
			to.clear();
			
			mysql_query(&mysql, ("SELECT 1 FROM users WHERE login = '" + arg + "'").c_str());
			bool login_in_db = false;
			if (res = mysql_store_result(&mysql)) {
				login_in_db = mysql_num_rows(res);
			}
			else
				cout << "Ошибка MySql номер " << mysql_error(&mysql);
			
			if (! login_in_db) {
				strcpy(message, "1");
			} else {
				to = arg;
				mysql_query(&mysql, ("INSERT INTO messages (sender, receiver, text) VALUES('" + currentLogin + "', '" + to + "', '" + text + "')").c_str());
				strcpy(message, "0");
			}
		} else {
			text = message;
			mysql_query(&mysql, ("INSERT INTO messages (sender, receiver, text) VALUES('" + currentLogin + "', '" + channel + "', '" + text + "')").c_str());
			strcpy(message, "0");
		}
		ssize_t bytes = write(connection, message, sizeof(message));
	}
}

void Server::finish() {
	// закрываем сокет, завершаем соединение
	close(socket_file_descriptor);
	
}