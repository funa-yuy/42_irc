#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "Database.hpp"
#include "Channel.hpp"
#include "Command/ModeCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args) {
    t_parsed in;
    in.cmd = cmd;
    in.client_fd = fd;
    in.args = args;
    return in;
}

static void test_view_success_minimal() {
    Database db("password");
    ModeCommand mode;

    // オペとして #room を作成
    int op_fd = 10;
    Client* op = db.addClient(op_fd);
    op->setNickname("op10");

    Channel ch("#room", op_fd);
    db.addChannel(ch);

    // 閲覧: 324 と 329 が返る（デフォルトは +t のみ）
    std::vector<std::string> args;
    args.push_back("#room");
    std::vector<t_response> res = mode.execute(makeInput("MODE", op_fd, args), db);

    assert(res.size() == 2);
    // 324
    assert(res[0].is_success == true);
    assert(res[0].should_send == true);
    assert(res[0].reply.find(" 324 op10 #room +t") != std::string::npos);
    // 329
    assert(res[1].is_success == true);
    assert(res[1].should_send == true);
    assert(res[1].reply.find(" 329 op10 #room ") != std::string::npos);
}

static void test_view_success_with_k_l() {
    Database db("password");
    ModeCommand mode;

    int op_fd = 11;
    Client* op = db.addClient(op_fd);
    op->setNickname("op11");

    Channel ch("#room2", op_fd);
    ch.setKey("secret");
    ch.setLimit(50);
    db.addChannel(ch);

    std::vector<std::string> args;
    args.push_back("#room2");
    std::vector<t_response> res = mode.execute(makeInput("MODE", op_fd, args), db);

    assert(res.size() == 2);
    // +tkl とキー/リミット
    assert(res[0].is_success == true);
    assert(res[0].should_send == true);
    assert(res[0].reply.find(" 324 op11 #room2 +tkl secret 50") != std::string::npos);
    // 329
    assert(res[1].reply.find(" 329 op11 #room2 ") != std::string::npos);
}

static void test_view_err_not_member() {
    Database db("password");
    ModeCommand mode;

    // チャンネルだけ作成（op20 はメンバー）
    int op_fd = 20;
    Client* op = db.addClient(op_fd);
    op->setNickname("op20");
    Channel ch("#view", op_fd);
    db.addChannel(ch);

    // 非メンバー user21 が閲覧 → 442
    int user_fd = 21;
    Client* user = db.addClient(user_fd);
    user->setNickname("user21");

    std::vector<std::string> args;
    args.push_back("#view");
    std::vector<t_response> res = mode.execute(makeInput("MODE", user_fd, args), db);

    assert(res.size() == 1);
    assert(res[0].should_send == true);
    assert(res[0].reply.find(" 442 user21 #view ") != std::string::npos);
}

static void test_view_err_not_operator() {
    Database db("password");
    ModeCommand mode;

    int op_fd = 30;
    Client* op = db.addClient(op_fd);
    op->setNickname("op30");
    Channel ch("#view2", op_fd);
    db.addChannel(ch);

    // メンバーだが非オペ
    int mem_fd = 31;
    Client* mem = db.addClient(mem_fd);
    mem->setNickname("mem31");
    Channel* ch_ptr = db.getChannel(std::string("#view2"));
    ch_ptr->addClientFd(mem_fd);

    std::vector<std::string> args;
    args.push_back("#view2");
    std::vector<t_response> res = mode.execute(makeInput("MODE", mem_fd, args), db);

    // 閲覧はメンバーなら許可され、324 と 329 が返る
    assert(res.size() == 2);
    assert(res[0].is_success == true);
    assert(res[0].should_send == true);
    assert(res[0].reply.find(" 324 mem31 #view2 ") != std::string::npos);
    assert(res[1].reply.find(" 329 mem31 #view2 ") != std::string::npos);
}

static void test_err_no_args_461() {
    Database db("password");
    ModeCommand mode;

    int fd = 40;
    Client* c = db.addClient(fd);
    c->setNickname("nick40");

    std::vector<std::string> args; // 空
    std::vector<t_response> res = mode.execute(makeInput("MODE", fd, args), db);

    assert(res.size() == 1);
    assert(res[0].should_send == true);
    assert(res[0].reply.find(" 461 nick40 MODE ") != std::string::npos);
}

static void test_err_no_such_channel_403() {
    Database db("password");
    ModeCommand mode;

    int fd = 41;
    Client* c = db.addClient(fd);
    c->setNickname("nick41");

    std::vector<std::string> args;
    args.push_back("#nochan");
    std::vector<t_response> res = mode.execute(makeInput("MODE", fd, args), db);

    assert(res.size() == 1);
    assert(res[0].reply.find(" 403 nick41 #nochan ") != std::string::npos);
}

static void test_modify_err_permission_442_482() {
    Database db("password");
    ModeCommand mode;

    int op_fd = 50;
    Client* op = db.addClient(op_fd);
    op->setNickname("op50");
    Channel ch("#perm", op_fd);
    db.addChannel(ch);

    // 非メンバーが +i → 442
    int user_fd = 51;
    Client* user = db.addClient(user_fd);
    user->setNickname("user51");
    {
        std::vector<std::string> args;
        args.push_back("#perm");
        args.push_back("+i");
        std::vector<t_response> res = mode.execute(makeInput("MODE", user_fd, args), db);
        assert(res.size() == 1);
        assert(res[0].reply.find(" 442 user51 #perm ") != std::string::npos);
    }

    // メンバーだが非オペが +i → 482
    int mem_fd = 52;
    Client* mem = db.addClient(mem_fd);
    mem->setNickname("mem52");
    Channel* ch_ptr = db.getChannel(std::string("#perm"));
    ch_ptr->addClientFd(mem_fd);

    {
        std::vector<std::string> args;
        args.push_back("#perm");
        args.push_back("+i");
        std::vector<t_response> res = mode.execute(makeInput("MODE", mem_fd, args), db);
        assert(res.size() == 1);
        assert(res[0].reply.find(" 482 mem52 #perm ") != std::string::npos);
    }
}

static void test_modify_err_unknown_mode_472() {
    Database db("password");
    ModeCommand mode;

    int op_fd = 60;
    Client* op = db.addClient(op_fd);
    op->setNickname("op60");
    Channel ch("#roomx", op_fd);
    db.addChannel(ch);

    std::vector<std::string> args;
    args.push_back("#roomx");
    args.push_back("+x"); // 未知
    std::vector<t_response> res = mode.execute(makeInput("MODE", op_fd, args), db);

    assert(res.size() == 1);
    assert(res[0].reply.find(" 472 op60 x ") != std::string::npos);
}

static void test_modify_err_params_461_and_441() {
    Database db("password");
    ModeCommand mode;

    int op_fd = 70;
    Client* op = db.addClient(op_fd);
    op->setNickname("op70");
    Channel ch("#roomy", op_fd);
    db.addChannel(ch);

    // +l の値不足 → 461
    {
        std::vector<std::string> args;
        args.push_back("#roomy");
        args.push_back("+l");
        std::vector<t_response> res = mode.execute(makeInput("MODE", op_fd, args), db);
        assert(res.size() == 1);
        assert(res[0].reply.find(" 461 op70 MODE ") != std::string::npos);
    }

    // +l の値が数値でない → 461
    {
        std::vector<std::string> args;
        args.push_back("#roomy");
        args.push_back("+l");
        args.push_back("abc");
        std::vector<t_response> res = mode.execute(makeInput("MODE", op_fd, args), db);
        assert(res.size() == 1);
        assert(res[0].reply.find(" 461 op70 MODE ") != std::string::npos);
    }

    // +k のキー不足 → 461
    {
        std::vector<std::string> args;
        args.push_back("#roomy");
        args.push_back("+k");
        std::vector<t_response> res = mode.execute(makeInput("MODE", op_fd, args), db);
        assert(res.size() == 1);
        assert(res[0].reply.find(" 461 op70 MODE ") != std::string::npos);
    }

    // -k はパラメータ不要に変更
    // 1) キーが設定されている場合は解除され、MODE -k が通知される（パラメータ無し）
    {
        Channel* ch_ptr = db.getChannel(std::string("#roomy"));
        ch_ptr->setKey("secret");

        std::vector<std::string> args;
        args.push_back("#roomy");
        args.push_back("-k");
        std::vector<t_response> res = mode.execute(makeInput("MODE", op_fd, args), db);

        assert(res.size() == 1);
        assert(res[0].should_send == true);
        // 送信行に「 MODE #roomy -k」が含まれる（キーは付かない）
        assert(res[0].reply.find(" MODE #roomy -k") != std::string::npos);
    }

    // 2) すでにキー未設定なら no-op（通知なし）
    {
        std::vector<std::string> args;
        args.push_back("#roomy");
        args.push_back("-k");
        std::vector<t_response> res = mode.execute(makeInput("MODE", op_fd, args), db);
        assert(res.size() == 0);
    }

    // +o 対象がチャンネル外 → 441
    {
        std::vector<std::string> args;
        args.push_back("#roomy");
        args.push_back("+o");
        args.push_back("bob");
        std::vector<t_response> res = mode.execute(makeInput("MODE", op_fd, args), db);
        assert(res.size() == 1);
        assert(res[0].reply.find(" 441 op70 bob #roomy ") != std::string::npos);
    }
}

static void test_modify_err_invalid_key_696() {
    Database db("password");
    ModeCommand mode;

    int op_fd = 75;
    Client* op = db.addClient(op_fd);
    op->setNickname("op75");
    Channel ch("#kbad", op_fd);
    db.addChannel(ch);

    std::vector<std::string> args;
    args.push_back("#kbad");
    args.push_back("+k");
    args.push_back("bad,key"); // カンマは不正
    std::vector<t_response> res = mode.execute(makeInput("MODE", op_fd, args), db);

    assert(res.size() == 1);
    assert(res[0].reply.find(" 696 op75 #kbad k bad,key ") != std::string::npos);
}

int main() {
    test_view_success_minimal();
    test_view_success_with_k_l();
    test_view_err_not_member();
    test_view_err_not_operator();
    test_err_no_args_461();
    test_err_no_such_channel_403();
    test_modify_err_permission_442_482();
    test_modify_err_unknown_mode_472();
    test_modify_err_params_461_and_441();
    test_modify_err_invalid_key_696();
    std::cout << "MODE command tests: OK" << std::endl;
    return 0;
}
