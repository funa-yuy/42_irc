#include <sys/socket.h> // socklen_t
#include <netinet/in.h> // sockaddr_in

#include <stdio.h> //perror
#include <stdlib.h> //exit
#include<string.h> //memset
#include<unistd.h> //close

#include <iostream> //cout


/* server */
// ```bash
// make
// ./ircserv
// ```

/* 確認用 */
// ```bash
// ss -tlnp | grep 6667
// ss -ant | grep TIME-WAIT #TIME-WAITに残っているかどうかの確認
// ```

/* irssi */
// ```bash
// irssi
// /connect 127.0.0.1 6667
// ```

int	main() {
	int	server_fd;
	int	client_fd;
	struct sockaddr_in	server_addr;
	struct sockaddr_in	client_addr;
	socklen_t	client_addr_len = sizeof(client_addr);
	char	read_buffer[1024];
	std::string	send_buffer;
	int	port = 6667;

	//ソケット生成
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket error");
		return (EXIT_FAILURE);
	}

	//サーバーアドレス構造体を設定
	memset(&server_addr, 0, sizeof(server_addr));//ヌル埋め
	server_addr.sin_family = AF_INET;//IPv4?
	// server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//htonlをなくすと仮想環境で接続ができなくなる。なぜ？
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);//TCP/IP ネットワークのバイト順で値を返す

	// ソケット登録
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind error");
		close(server_fd);
		return(EXIT_FAILURE);
	}

	//ソケット接続準備
	if (listen(server_fd, 5) < 0) { //ここの5はマクロに変えてもいい?
		perror("listen error");
		close(server_fd);
		return(EXIT_FAILURE);
	}
	std::cout << "Server listening on port " << port << ". /connect 127.0.0.1 6667" << std::endl;

	//ソケット接続待機
	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_fd < 0) {
		perror("accept error");
		return (EXIT_FAILURE);
	}
	std::cout << "Client connected." << std::endl;

	//読み取り todo: 課題ではreadは使用してはいけないので修正が必要
	while (true)
	{
		memset(&read_buffer, 0, sizeof(read_buffer));//ヌル埋め
		//clientからのメッセージを読む
		ssize_t	bytes_read = read(client_fd, read_buffer, sizeof(read_buffer) - 1);
		if (bytes_read < 0) {
			perror("read error");
			close(client_fd);
			close(server_fd);
			return (EXIT_FAILURE);
		}
		read_buffer[bytes_read] = '\0';
		// std::cout << "受信メッセージ: " << read_buffer << std::endl;

		if (strstr(read_buffer, "CAP LS") != NULL) {
			std::cout << "CAP * LS :\r\n" << read_buffer << std::endl;
			send_buffer = ":irc.example.com Hi! CAP!\r\n";
		}
		else if (strcmp(read_buffer, "NICK") == 0) {
			std::cout << "受信メッセージ NICK: " << read_buffer << std::endl;
			send_buffer = ":irc.example.com Hi! NICK!\r\n";
		}
		else if (strcmp(read_buffer, "USER") == 0) {
			std::cout << "受信メッセージ USER: " << read_buffer << std::endl;
			send_buffer = ":irc.example.com Hi! USER!\r\n";
		}
		else if (strcmp(read_buffer, "QUIT") == 0) {
			std::cout << "受信メッセージ QUIT: " << read_buffer << std::endl;
			break ;
		}
		else {
			std::cout << "受信メッセージ: " << read_buffer << std::endl;
			send_buffer = ":irc.example.com What???\r\n";
		}
		if (send(client_fd, send_buffer.c_str(), send_buffer.length(), 0) < 0) {
			perror("send error");
			close(client_fd);
			close(server_fd);
			return (EXIT_FAILURE);
		}
		// if (getline(std::cin, send_buffer)) {
		// 	send_buffer += "\n";
		// 	if (send(client_fd, send_buffer.c_str(), send_buffer.length(), 0) < 0) {
		// 		perror("send error");
		// 		close(client_fd);
		// 		close(server_fd);
		// 		return (EXIT_FAILURE);
		// 	}
		// }
	}

	// ソケット切断
	close(client_fd);
	close(server_fd);
	return (EXIT_SUCCESS);
}
