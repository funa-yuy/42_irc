#include <cassert>
#include <iostream>
#include <algorithm>

#include "Database.hpp"
#include "Command/QuitCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args) {
    t_parsed in;
    in.cmd = cmd;
    in.client_fd = fd;
    in.args = args;
    return in;
}

static void test_quit_broadcast_and_error_with_reason() {
    Database db("password");
    QuitCommand quit;

    int sender_fd = 10;
    Client* sender = db.addClient(sender_fd);
    sender->setNickname("alice");
    // sender->setUsername("alice"); // 必要なら

    int other_fd = 11;
    Client* other = db.addClient(other_fd);
    other->setNickname("bob");

    // チャンネル作成し両者を参加させる
    Channel ch("#room", sender_fd);
    db.addChannel(ch);
    Channel* chp = db.getChannel(std::string("#room"));
    chp->addClientFd(other_fd);

    std::vector<std::string> args;
    args.push_back("bye");
    t_parsed in = makeInput("QUIT", sender_fd, args);

    std::vector<t_response> res = quit.execute(in, db);
    assert(res.size() == 2);

    // 0) ブロードキャスト（他メンバー向け）
    assert(res[0].is_success == true);
    assert(res[0].should_send == true);
    assert(res[0].should_disconnect == false);
    assert(res[0].reply.find(":alice!@ft.irc QUIT :bye") != std::string::npos); // username未設定想定
    // 送信先は他メンバーのみ（senderは含まれない）
    const std::vector<int>& br_tg = res[0].target_fds;
    assert(std::find(br_tg.begin(), br_tg.end(), other_fd) != br_tg.end());
    assert(std::find(br_tg.begin(), br_tg.end(), sender_fd) == br_tg.end());

    // 1) ERROR（送信元向け、切断）
    assert(res[1].is_success == true);
    assert(res[1].should_send == true);
    assert(res[1].should_disconnect == true);
    assert(res[1].reply.find("ERROR :Closing Link: alice [Quit: bye]") != std::string::npos);
    assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == sender_fd);
}

static void test_quit_no_reason_uses_default() {
    Database db("password");
    QuitCommand quit;

    int sender_fd = 20;
    Client* sender = db.addClient(sender_fd);
    sender->setNickname("u20");

    int other_fd = 21;
    Client* other = db.addClient(other_fd);
    other->setNickname("u21");

    Channel ch("#v", sender_fd);
    db.addChannel(ch);
    db.getChannel(std::string("#v"))->addClientFd(other_fd);

    std::vector<std::string> args; // 理由なし
    t_parsed in = makeInput("QUIT", sender_fd, args);

    std::vector<t_response> res = quit.execute(in, db);
    assert(res.size() == 2);

    assert(res[0].reply.find(":u20!@ft.irc QUIT :Client quit") != std::string::npos);
    assert(std::find(res[0].target_fds.begin(), res[0].target_fds.end(), other_fd) != res[0].target_fds.end());

    assert(res[1].reply.find("ERROR :Closing Link: u20 [Quit: Client quit]") != std::string::npos);
    assert(res[1].should_disconnect == true);
    assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == sender_fd);
}

static void test_quit_no_channels_broadcast_empty() {
    Database db("password");
    QuitCommand quit;

    int sender_fd = 30;
    Client* sender = db.addClient(sender_fd);
    sender->setNickname("solo");

    std::vector<std::string> args;
    args.push_back("see ya");
    t_parsed in = makeInput("QUIT", sender_fd, args);

    std::vector<t_response> res = quit.execute(in, db);
    assert(res.size() == 2);

    // ブロードキャストは空ターゲットで返る想定
    assert(res[0].reply.find(":solo!@ft.irc QUIT :see ya") != std::string::npos);
    assert(res[0].target_fds.empty());
    assert(res[0].should_disconnect == false);

    assert(res[1].reply.find("ERROR :Closing Link: solo [Quit: see ya]") != std::string::npos);
    assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == sender_fd);
    assert(res[1].should_disconnect == true);
}

static void test_quit_client_not_found_returns_empty() {
    Database db("password");
    QuitCommand quit;

    int fd = 999; // 未登録
    std::vector<std::string> args;
    args.push_back("bye");
    t_parsed in = makeInput("QUIT", fd, args);

    std::vector<t_response> res = quit.execute(in, db);
    assert(res.size() == 0);
}

static void test_quit_factory() {
    Command* cmd = QuitCommand::createQuitCommand();
    assert(cmd != NULL);
    QuitCommand* q = dynamic_cast<QuitCommand*>(cmd);
    assert(q != NULL);
    delete cmd;
}

int main() {
    test_quit_broadcast_and_error_with_reason();
    test_quit_no_reason_uses_default();
    test_quit_no_channels_broadcast_empty();
    test_quit_client_not_found_returns_empty();
    test_quit_factory();
    std::cout << "QUIT command tests: OK" << std::endl;
    return 0;
}
