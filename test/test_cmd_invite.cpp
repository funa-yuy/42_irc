#include <cassert>
#include <iostream>
#include <algorithm>

#include "Database.hpp"
#include "Command/InviteCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args) {
    t_parsed in;
    in.cmd = cmd;
    in.client_fd = fd;
    in.args = args;
    return in;
}

static void test_success_existing_channel() {
    Database db("password");
    InviteCommand invite;

    // inviter と invitee を準備
    int inviter_fd = 1;
    Client* inviter = db.addClient(inviter_fd);
    inviter->setNickname("alice");

    int invitee_fd = 2;
    Client* invitee = db.addClient(invitee_fd);
    invitee->setNickname("bob");

    // 既存チャンネル作成（alice が OP）
    Channel ch("#room", inviter_fd);
    db.addChannel(ch);

    std::vector<std::string> args;
    args.push_back("bob");
    args.push_back("#room");
    t_parsed in = makeInput("INVITE", inviter_fd, args);
    std::vector<t_response> res = invite.execute(in, db);

    assert(res.size() == 2);

    // 341 RPL_INVITING to inviter
    assert(res[0].is_success == true);
    assert(res[0].should_send == true);
    assert(res[0].should_disconnect == false);
    assert(res[0].reply.find(" 341 ") != std::string::npos);
    assert(res[0].reply.find(" alice bob #room") != std::string::npos);
    assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == inviter_fd);

    // INVITE message to invitee
    assert(res[1].is_success == true);
    assert(res[1].should_send == true);
    assert(res[1].should_disconnect == false);
    assert(res[1].reply.find(" INVITE bob #room") != std::string::npos);
    assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == invitee_fd);

    // チャンネルの招待リストに invitee が追加されている
    Channel* room = db.getChannel("#room");
    assert(room != NULL);
    assert(room->isInvited(invitee_fd) == true);
}

static void test_success_nonexistent_channel() {
    Database db("password");
    InviteCommand invite;

    int inviter_fd = 10;
    Client* inviter = db.addClient(inviter_fd);
    inviter->setNickname("alice10");

    int invitee_fd = 11;
    Client* invitee = db.addClient(invitee_fd);
    invitee->setNickname("bob11");

    // チャンネルは存在しない
    std::vector<std::string> args;
    args.push_back("bob11");
    args.push_back("#no_such");
    t_parsed in = makeInput("INVITE", inviter_fd, args);
    std::vector<t_response> res = invite.execute(in, db);

    assert(res.size() == 2);
    // 341 は返る
    assert(res[0].reply.find(" 341 ") != std::string::npos);
    assert(res[0].reply.find(" alice10 bob11 #no_such") != std::string::npos);
    // INVITE も返る
    assert(res[1].reply.find(" INVITE bob11 #no_such") != std::string::npos);

    // チャンネルは作成されていない
    assert(db.getChannel("#no_such") == NULL);
}

static void test_err_461_needmoreparams() {
    Database db("password");
    InviteCommand invite;

    int fd = 20;
    Client* c = db.addClient(fd);
    c->setNickname("u20");

    std::vector<std::string> args; // 引数なし
    t_parsed in = makeInput("INVITE", fd, args);
    std::vector<t_response> res = invite.execute(in, db);

    assert(res.size() == 1);
    assert(res[0].is_success == false);
    assert(res[0].should_send == true);
    assert(res[0].reply.find(" 461 ") != std::string::npos);
    assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
}

static void test_err_401_nosuchnick() {
    Database db("password");
    InviteCommand invite;

    int fd = 21;
    Client* c = db.addClient(fd);
    c->setNickname("u21");

    std::vector<std::string> args;
    args.push_back("nobody"); // 存在しないニック
    args.push_back("#room");
    t_parsed in = makeInput("INVITE", fd, args);
    std::vector<t_response> res = invite.execute(in, db);

    assert(res.size() == 1);
    assert(res[0].is_success == false);
    assert(res[0].should_send == true);
    assert(res[0].reply.find(" 401 ") != std::string::npos);
    assert(res[0].reply.find(" nobody ") != std::string::npos);
    assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
}

static void test_err_442_notonchannel() {
    Database db("password");
    InviteCommand invite;

    // inviter
    int inviter_fd = 30;
    Client* inviterCl = db.addClient(inviter_fd);
    inviterCl->setNickname("inv30");

    // invitee
    int invitee_fd = 31;
    Client* inviteeCl = db.addClient(invitee_fd);
    inviteeCl->setNickname("bob31");

    // inviter が参加していないチャンネルを作成（別ユーザが作成）
    int owner_fd = 32;
    Client* ownerCl = db.addClient(owner_fd);
    ownerCl->setNickname("owner32");

    Channel ch("#ch442", owner_fd);
    db.addChannel(ch);

    std::vector<std::string> args;
    args.push_back("bob31");
    args.push_back("#ch442");
    t_parsed in = makeInput("INVITE", inviter_fd, args);
    std::vector<t_response> res = invite.execute(in, db);

    assert(res.size() == 1);
    assert(res[0].is_success == false);
    assert(res[0].should_send == true);
    assert(res[0].reply.find(" 442 ") != std::string::npos);
    assert(res[0].reply.find(" #ch442 ") != std::string::npos);
    assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == inviter_fd);
}

static void test_err_482_notchanop() {
    Database db("password");
    InviteCommand invite;

    // inviter（メンバーだがOPではない）
    int inviter_fd = 40;
    Client* inviterCl = db.addClient(inviter_fd);
    inviterCl->setNickname("inv40");

    // invitee
    int invitee_fd = 41;
    Client* inviteeCl = db.addClient(invitee_fd);
    inviteeCl->setNickname("bob41");

    // チャンネル作成（別ユーザがOP）
    int owner_fd = 42;
    Client* ownerCl = db.addClient(owner_fd);
    ownerCl->setNickname("owner42");

    Channel ch("#ch482", owner_fd);
    // inviter をメンバーとして追加（OPにはしない）
    ch.addClientFd(inviter_fd);
    db.addChannel(ch);

    std::vector<std::string> args;
    args.push_back("bob41");
    args.push_back("#ch482");
    t_parsed in = makeInput("INVITE", inviter_fd, args);
    std::vector<t_response> res = invite.execute(in, db);

    assert(res.size() == 1);
    assert(res[0].is_success == false);
    assert(res[0].should_send == true);
    assert(res[0].reply.find(" 482 ") != std::string::npos);
    assert(res[0].reply.find(" #ch482 ") != std::string::npos);
    assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == inviter_fd);
}

static void test_err_443_useronchannel() {
    Database db("password");
    InviteCommand invite;

    // inviter（OP）
    int inviter_fd = 50;
    Client* inviterCl = db.addClient(inviter_fd);
    inviterCl->setNickname("op50");

    // invitee（既にメンバー）
    int invitee_fd = 51;
    Client* inviteeCl = db.addClient(invitee_fd);
    inviteeCl->setNickname("bob51");

    Channel ch("#ch443", inviter_fd); // inviter が OP
    ch.addClientFd(invitee_fd);       // invitee はすでにメンバー
    db.addChannel(ch);

    std::vector<std::string> args;
    args.push_back("bob51");
    args.push_back("#ch443");
    t_parsed in = makeInput("INVITE", inviter_fd, args);
    std::vector<t_response> res = invite.execute(in, db);

    assert(res.size() == 1);
    assert(res[0].is_success == false);
    assert(res[0].should_send == true);
    assert(res[0].reply.find(" 443 ") != std::string::npos);
    assert(res[0].reply.find(" bob51 #ch443 ") != std::string::npos);
    assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == inviter_fd);
}

int main() {
    test_success_existing_channel();
    test_success_nonexistent_channel();
    test_err_461_needmoreparams();
    test_err_401_nosuchnick();
    test_err_442_notonchannel();
    test_err_482_notchanop();
    test_err_443_useronchannel();
    std::cout << "INVITE command tests: OK" << std::endl;
    return 0;
}
