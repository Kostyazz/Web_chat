CREATE DATABASE IF NOT EXISTS  chatdb;
USE chatdb;
DROP TABLE IF EXISTS users, messages;
CREATE TABLE users (
	login VARCHAR(30) PRIMARY KEY,
	password VARCHAR(30) NOT NULL);
CREATE TABLE messages (
	id SERIAL PRIMARY KEY,
	sender VARCHAR(30),
	receiver VARCHAR(30),
	text VARCHAR(9000),
	send_date DATE,
	FOREIGN KEY (sender) REFERENCES users(login));
INSERT INTO users VALUES('a', 'a');
INSERT INTO users VALUES('b', 'b');
INSERT INTO users VALUES('Vasya', '1111');
INSERT INTO users VALUES('Petya', '1234');
