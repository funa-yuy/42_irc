#include <cassert>
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../includes/Server.hpp"

static int connect_client(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(fd >= 0);
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    int r = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    assert(r == 0);
    return fd;
}

static void send_line(int fd, const std::string& l)
{
    std::string msg = l;
    if (msg.find("\r\n") == std::string::npos)
        msg += "\r\n";
    ssize_t n = send(fd, msg.c_str(), msg.size(), 0);
    assert(n == (ssize_t)msg.size());
}

static std::string recv_nonblock(int fd)
{
    char buf[1024];
    std::string out;
    for (int i = 0; i < 10; ++i) {
        ssize_t n = recv(fd, buf, sizeof(buf)-1, MSG_DONTWAIT);
        if (n <= 0) break;
        buf[n] = 0;
        out += buf;
    }
    return out;
}

static void drive(Server& srv, int ms_total, int slice_ms=10)
{
    int elapsed = 0;
    while (elapsed < ms_total) {
        srv.step(slice_ms);
        usleep(slice_ms * 1000);
        elapsed += slice_ms;
    }
}

static void test_ping_pong(Server& srv, int port)
{
    int fd = connect_client(port);

    // 登録 (PASS/NICK/USER)
    send_line(fd, "PASS pw");
    send_line(fd, "NICK pinguser");
    send_line(fd, "USER uu 0 * :Real User");
    drive(srv, 200);
    std::string r = recv_nonblock(fd);
    assert(r.find(" 001 ") != std::string::npos); // 登録完了

    // クライアント発 PING
    std::string token = "abc123";
    send_line(fd, "PING :" + token);
    drive(srv, 100);
    std::string r2 = recv_nonblock(fd);
    // 期待: PONG :abc123
    assert(r2.find("PONG :" + token) != std::string::npos);

    // 引数なし PING → 409
    send_line(fd, "PING");
    drive(srv, 100);
    std::string r3 = recv_nonblock(fd);
    assert(r3.find(" 409 ") != std::string::npos);

    close(fd);
}

int main()
{
    const int PORT = 7301; // 他テストと衝突しないポート
    Server srv(PORT, "pw");
    test_ping_pong(srv, PORT);
    std::cout << "test_cmd_ping: OK\n";
    return 0;
}
