// filepath: /Users/ken/Documents/42/42cursus/work/rank05/ft_irc/test/test_server.cpp
#include <cassert>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../includes/Server.hpp"

// ユーティリティ
static int connect_client(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(fd >= 0);
    struct sockaddr_in addr;
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
    int loops = 10;
    while (loops--)
    {
        ssize_t n = recv(fd, buf, sizeof(buf)-1, MSG_DONTWAIT);
        if (n <= 0) break;
        buf[n] = 0;
        out += buf;
    }
    return out;
}

static bool contains(const std::string& s, const std::string& pat)
{
    return s.find(pat) != std::string::npos;
}

static void drive(Server& srv, int ms_total, int slice_ms=10)
{
    // 合計 ms_total になるまで step()
    int elapsed = 0;
    while (elapsed < ms_total)
    {
        srv.step(slice_ms);
        elapsed += slice_ms;
        usleep(slice_ms * 1000);
    }
}

// テスト 1: 遅延 PASS (NICK → USER → PASS) で 001
static void test_delayed_pass(Server& srv, int port)
{
    int fd = connect_client(port);
    drive(srv, 100);
    send_line(fd, "NICK alice");
    send_line(fd, "USER alice 0 * :Alice Real");
    drive(srv, 100);
    std::string r1 = recv_nonblock(fd);
    assert(!contains(r1, " 001 ")); // まだ登録前

    send_line(fd, "PASS pw");
    drive(srv, 200);
    std::string r2 = recv_nonblock(fd);
    assert(contains(r2, " 001 "));
    close(fd);
}

// テスト 2: PASS 不一致 464
static void test_wrong_pass(Server& srv, int port)
{
    int fd = connect_client(port);
    send_line(fd, "PASS wrong");
    drive(srv, 150);
    std::string r = recv_nonblock(fd);
	assert(contains(r, " 464 "));

	// 切断はしない: もう一度 PASS を送り直せる
	send_line(fd, "PASS pw");
	send_line(fd, "NICK user1");
	send_line(fd, "USER u 0 * :real");
	drive(srv, 200);
	std::string r2 = recv_nonblock(fd);
	// 登録完了(001)が返ることを確認
	assert(contains(r2, " 001 "));
	close(fd);
}

// テスト 3: 未登録 PRIVMSG で 451
static void test_prereg_privmsg(Server& srv, int port)
{
    int fd = connect_client(port);
    send_line(fd, "PRIVMSG bob :hi");
    drive(srv, 150);
    std::string r = recv_nonblock(fd);
    assert(contains(r, " 451 "));
    close(fd);
}

// テスト 4: 重複 NICK で 433
static void test_duplicate_nick(Server& srv, int port)
{
    int fd1 = connect_client(port);
    send_line(fd1, "PASS pw");
    send_line(fd1, "NICK aaa");
    send_line(fd1, "USER aaa 0 * :A A");
    drive(srv, 200);
    std::string r1 = recv_nonblock(fd1);
    assert(contains(r1, " 001 "));

    int fd2 = connect_client(port);
    send_line(fd2, "NICK aaa");
    drive(srv, 150);
    std::string r2 = recv_nonblock(fd2);
    assert(contains(r2, " 433 "));
    close(fd2);
    close(fd1);
}

int main()
{
    const int PORT = 7201;
    Server srv(PORT, "pw");

    test_delayed_pass(srv, PORT);
    test_wrong_pass(srv, PORT);
    test_prereg_privmsg(srv, PORT);
    test_duplicate_nick(srv, PORT);

    std::cout << "test_server (step-based): ALL TESTS PASSED\n";
    return 0;
}
