#include <iostream>
#include <cassert>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sstream>
#include <stdint.h>

// Include the headers
#include "../includes/Server.hpp"
#include "../includes/Database.hpp"
#include "../includes/Client.hpp"
#include "../includes/irc.hpp"

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

// Helper function to convert numbers to string (C++98 compatible)
template<typename T>
std::string toString(T value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// Helper function to convert pointer to hex string
std::string toHexString(unsigned long value) {
    std::ostringstream oss;
    oss << "0x" << std::hex << value;
    return oss.str();
}

// Test helper functions
void printTestResult(const std::string& testName, bool passed, const std::string& details = "") {
    if (passed) {
        std::cout << "[" << GREEN << BOLD << "成功" << RESET << "] " << CYAN << testName << RESET << std::endl;
    } else {
        std::cout << "[" << RED << BOLD << "失敗" << RESET << "] " << CYAN << testName << RESET << std::endl;
    }
    if (!details.empty()) {
        std::cout << "    " << YELLOW << "詳細: " << RESET << details << std::endl;
    }
    std::cout << std::endl;
}

// Test Database class
void testDatabase() {
    std::cout << "\n" << BOLD << MAGENTA << "=== Databaseクラステスト ===" << RESET << std::endl;

    Database db("password");

    // Test 1: Add client
    Client* client1 = db.addClient(10);
    std::string details1 = "fd=10, ポインタ=" + toHexString(reinterpret_cast<unsigned long>(client1));
    printTestResult("Database::addClient - 有効なポインタを返す", client1 != NULL, details1);

    // Test 2: Get client that exists
    Client* retrieved = db.getClient(10);
    std::string details2 = "fd=10, 元のポインタ=" + toHexString(reinterpret_cast<unsigned long>(client1)) +
                          ", 取得したポインタ=" + toHexString(reinterpret_cast<unsigned long>(retrieved)) +
                          ", 同一=" + (retrieved == client1 ? "はい" : "いいえ");
    printTestResult("Database::getClient - 既存クライアントの取得", retrieved != NULL && retrieved == client1, details2);

    // Test 3: Get client that doesn't exist
    Client* nonExistent = db.getClient(999);
    std::string details3 = "fd=999, ポインタ=" + toHexString(reinterpret_cast<unsigned long>(nonExistent));
    printTestResult("Database::getClient - 存在しないクライアントはNULLを返す", nonExistent == NULL, details3);

    // Test 4: Add multiple clients
    Client* client2 = db.addClient(20);
    Client* client3 = db.addClient(30);
    std::string details4 = "クライアント1(fd=10)=" + toHexString(reinterpret_cast<unsigned long>(client1)) +
                          ", クライアント2(fd=20)=" + toHexString(reinterpret_cast<unsigned long>(client2)) +
                          ", クライアント3(fd=30)=" + toHexString(reinterpret_cast<unsigned long>(client3));
    printTestResult("Database::addClient - 複数クライアントの追加",
                   client2 != NULL && client3 != NULL && client1 != client2 && client2 != client3, details4);

    // Test 5: Remove client
    db.removeClient(20);
    Client* removedClient = db.getClient(20);
    std::string details5 = "削除したfd=20, getClient(20)の戻り値=" + toHexString(reinterpret_cast<unsigned long>(removedClient));
    printTestResult("Database::removeClient - クライアントの削除成功", removedClient == NULL, details5);

    // Test 6: Verify other clients still exist after removal
    Client* stillExists1 = db.getClient(10);
    Client* stillExists3 = db.getClient(30);
    std::string exists1 = (stillExists1 != NULL ? "存在" : "なし");
    std::string exists3 = (stillExists3 != NULL ? "存在" : "なし");
    std::string details6 = "fd=10 " + exists1 + ", fd=30 " + exists3;
    printTestResult("Database::removeClient - 他のクライアントに影響なし",
                   stillExists1 != NULL && stillExists3 != NULL, details6);

    // Test 7: Remove non-existent client (should not crash)
    db.removeClient(999);
    printTestResult("Database::removeClient - 存在しないクライアントの削除でクラッシュしない", true, "fd=999");

    // Test 8: Const version of getClient
    const Database& constDb = db;
    const Client* constClient = constDb.getClient(10);
    std::string details8 = "const getClient(fd=10)=" + toHexString(reinterpret_cast<unsigned long>(constClient));
    printTestResult("Database::getClient const - 有効なconstポインタを返す", constClient != NULL, details8);

    const Client* constNonExistent = constDb.getClient(999);
    std::string details8b = "const getClient(fd=999)=" + toHexString(reinterpret_cast<unsigned long>(constNonExistent));
    printTestResult("Database::getClient const - 存在しない場合NULLを返す", constNonExistent == NULL, details8b);
}

// Test Client class
void testClient() {
    std::cout << "\n" << BOLD << MAGENTA << "=== Clientクラステスト ===" << RESET << std::endl;

    // Test 1: Default constructor
    Client client;
    printTestResult("Client::Client - デフォルトコンストラクタ", true, "オブジェクトが正常に作成されました");

    // Test 2: Initialize client
    client.initializeClient(42);
    int actualFd = client.getFd();
    std::string details2 = "期待値 fd=42, 実際の値 fd=" + toString(actualFd);
    printTestResult("Client::initializeClient - 初期化", client.getFd() == 42, details2);

    // Test 3: Get file descriptor
    int fd = client.getFd();
    std::string details3 = "getFd()の戻り値 " + toString(fd);
    printTestResult("Client::getFd - 正しいfdを返す", fd == 42, details3);

    // Test 4: Get pollfd structure
    struct pollfd pfd = client.getPfd();
    std::string details4 = "pfd.fd=" + toString(pfd.fd) + ", pfd.events=" + toString(pfd.events) +
                          " (POLLIN=" + toString(POLLIN) + ")";
    printTestResult("Client::getPfd - 正しいpollfd構造体を返す", pfd.fd == 42 && (pfd.events & POLLIN), details4);

    // Test 5: Buffer operations
    std::string& buffer = client.getBuffer();
    buffer = "test message";
    std::string details5 = "バッファ内容: '" + buffer + "'";
    printTestResult("Client::getBuffer - バッファの変更", buffer == "test message", details5);

    // Test 6: Const buffer access
    const Client& constClient = client;
    const std::string& constBuffer = constClient.getBuffer();
    std::string details6 = "constバッファ内容: '" + constBuffer + "'";
    printTestResult("Client::getBuffer const - constアクセス", constBuffer == "test message", details6);

    // Test 7: Buffer append simulation
    buffer += " appended";
    std::string details7 = "追記後のバッファ: '" + buffer + "'";
    printTestResult("Client::getBuffer - バッファの追記", buffer == "test message appended", details7);

    // Test 8: Multiple clients with different fds
    Client client2;
    client2.initializeClient(100);
    std::string details8 = "クライアント1 fd=" + toString(client.getFd()) +
                          ", クライアント2 fd=" + toString(client2.getFd());
    printTestResult("Client - 異なるfdを持つ複数クライアント",
                   client.getFd() == 42 && client2.getFd() == 100, details8);
}

// Integration tests
void testDatabaseClientIntegration() {
    std::cout << "\n" << BOLD << MAGENTA << "=== Database-Client統合テスト ===" << RESET << std::endl;

    Database db("password");

    // Test 1: Add client and verify initialization
    Client* client = db.addClient(50);
    std::string details1 = "fd=50, クライアントポインタ=" + toHexString(reinterpret_cast<unsigned long>(client)) +
                          ", client->getFd()=" + (client ? toString(client->getFd()) : "NULL");
    printTestResult("統合テスト - データベース内でクライアントが適切に初期化される",
                   client != NULL && client->getFd() == 50, details1);

    // Test 2: Buffer operations through database
    client->getBuffer() = "integration test";
    Client* retrieved = db.getClient(50);
    std::string details2 = "元のバッファ: '" + client->getBuffer() +
                          "', 取得したバッファ: '" + (retrieved ? retrieved->getBuffer() : "NULL") + "'";
    printTestResult("統合テスト - データベース経由でバッファが保持される",
                   retrieved != NULL && retrieved->getBuffer() == "integration test", details2);

    // Test 3: Multiple clients with different buffers
    Client* client2 = db.addClient(60);
    client2->getBuffer() = "second client buffer";

    std::string buffer1 = db.getClient(50)->getBuffer();
    std::string buffer2 = db.getClient(60)->getBuffer();
    std::string details3 = "クライアント1(fd=50) バッファ: '" + buffer1 + "', クライアント2(fd=60) バッファ: '" + buffer2 + "'";
    printTestResult("統合テスト - 複数クライアントが独立したバッファを維持",
                   buffer1 == "integration test" && buffer2 == "second client buffer", details3);

    // Test 4: Remove client and verify cleanup
    db.removeClient(50);
    Client* removedClient = db.getClient(50);
    Client* remainingClient = db.getClient(60);
    std::string details4 = "削除されたクライアント(fd=50)=" + toHexString(reinterpret_cast<unsigned long>(removedClient)) +
                          ", 残存クライアント(fd=60)=" + toHexString(reinterpret_cast<unsigned long>(remainingClient)) +
                          ", 残存バッファ: '" + (remainingClient ? remainingClient->getBuffer() : "NULL") + "'";
    printTestResult("統合テスト - クライアント削除が他のクライアントに影響しない",
                   removedClient == NULL && remainingClient != NULL &&
                   remainingClient->getBuffer() == "second client buffer", details4);
}

// Stress test
void testStress() {
    std::cout << "\n" << BOLD << MAGENTA << "=== ストレステスト ===" << RESET << std::endl;

    Database db("password");
    const int NUM_CLIENTS = 100;

    // Test 1: Add many clients
    bool allAdded = true;
    int failedAt = -1;
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        Client* client = db.addClient(i + 1000);
        if (client == NULL) {
            allAdded = false;
            failedAt = i;
            break;
        }
        client->getBuffer() = "Client " + toString(i);
    }
    std::string details1 = "fd範囲: 1000-" + toString(1000 + NUM_CLIENTS - 1) +
                          (failedAt >= 0 ? ", インデックス " + toString(failedAt) + " で失敗" : ", すべて成功");
    printTestResult("ストレステスト - 100個のクライアント追加", allAdded, details1);

    // Test 2: Retrieve all clients
    bool allRetrieved = true;
    int retrieveFailedAt = -1;
    std::string failedBuffer = "";
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        Client* client = db.getClient(i + 1000);
        if (client == NULL || client->getBuffer() != "Client " + toString(i)) {
            allRetrieved = false;
            retrieveFailedAt = i;
            failedBuffer = client ? client->getBuffer() : "NULL";
            break;
        }
    }
    std::string details2 = "チェックしたfd範囲: 1000-" + toString(1000 + NUM_CLIENTS - 1);
    if (retrieveFailedAt >= 0) {
        details2 += ", fd=" + toString(retrieveFailedAt + 1000) + " で失敗" +
                   ", 期待値='Client " + toString(retrieveFailedAt) + "', 実際='" + failedBuffer + "'";
    }
    printTestResult("ストレステスト - 100個のクライアントを正しいデータで取得", allRetrieved, details2);

    // Test 3: Remove half the clients
    bool halfRemoved = true;
    int removeFailedAt = -1;
    for (int i = 0; i < NUM_CLIENTS / 2; ++i) {
        db.removeClient(i + 1000);
    }

    // Verify removed clients are gone and remaining clients are still there
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        Client* client = db.getClient(i + 1000);
        if (i < NUM_CLIENTS / 2) {
            if (client != NULL) {
                halfRemoved = false;
                removeFailedAt = i;
                break;
            }
        } else {
            if (client == NULL || client->getBuffer() != "Client " + toString(i)) {
                halfRemoved = false;
                removeFailedAt = i;
                break;
            }
        }
    }
    std::string details3 = "削除したfd範囲: 1000-" + toString(1000 + NUM_CLIENTS/2 - 1) +
                          ", 残存fd範囲: " + toString(1000 + NUM_CLIENTS/2) + "-" + toString(1000 + NUM_CLIENTS - 1);
    if (removeFailedAt >= 0) {
        details3 += ", fd=" + toString(removeFailedAt + 1000) + " で検証失敗";
    }
    printTestResult("ストレステスト - 半分のクライアント削除、残りを検証", halfRemoved, details3);
}

// Edge case tests
void testEdgeCases() {
    std::cout << "\n" << BOLD << MAGENTA << "=== エッジケーステスト ===" << RESET << std::endl;

    Database db("password");

    // Test 1: Add client with fd 0
    Client* client0 = db.addClient(0);
    std::string details1 = "fd=0, ポインタ=" + toHexString(reinterpret_cast<unsigned long>(client0)) +
                          ", getFd()=" + (client0 ? toString(client0->getFd()) : "NULL");
    printTestResult("エッジケース - fd=0のクライアント追加", client0 != NULL && client0->getFd() == 0, details1);

    // Test 2: Add client with negative fd
    Client* clientNeg = db.addClient(-1);
    std::string details2 = "fd=-1, ポインタ=" + toHexString(reinterpret_cast<unsigned long>(clientNeg)) +
                          ", getFd()=" + (clientNeg ? toString(clientNeg->getFd()) : "NULL");
    printTestResult("エッジケース - 負のfdのクライアント追加", clientNeg != NULL && clientNeg->getFd() == -1, details2);

    // Test 3: Add client with same fd twice (should overwrite)
    Client* client1 = db.addClient(123);
    client1->getBuffer() = "first";
    unsigned long addr1 = reinterpret_cast<unsigned long>(client1);

    Client* client2 = db.addClient(123);  // Same fd
    client2->getBuffer() = "second";
    unsigned long addr2 = reinterpret_cast<unsigned long>(client2);

    Client* retrieved = db.getClient(123);
    std::string details3 = "fd=123, 最初のアドレス=" + toHexString(addr1) +
                          ", 2番目のアドレス=" + toHexString(addr2) +
                          ", 取得したバッファ='" + (retrieved ? retrieved->getBuffer() : "NULL") + "'";
    printTestResult("エッジケース - 同じfdでクライアント追加（上書き）",
                   retrieved != NULL && retrieved->getBuffer() == "second", details3);

    // Test 4: Very long buffer
    Client client;
    client.initializeClient(999);
    std::string longBuffer(10000, 'A');  // 10KB of 'A's
    client.getBuffer() = longBuffer;
    std::string details4 = "fd=999, バッファ長=" + toString(client.getBuffer().length()) +
                          ", 期待値=10000, 最初の10文字='" + client.getBuffer().substr(0, 10) + "'";
    printTestResult("エッジケース - 非常に長いバッファ", client.getBuffer().length() == 10000, details4);

    // Test 5: Empty buffer operations
    Client emptyClient;
    emptyClient.initializeClient(888);
    std::string isEmpty = (emptyClient.getBuffer().empty() ? "はい" : "いいえ");
    std::string details5 = "fd=888, バッファが空=" + isEmpty +
                          ", バッファ長=" + toString(emptyClient.getBuffer().length());
    printTestResult("エッジケース - 空のバッファ操作", emptyClient.getBuffer().empty(), details5);
}

int main() {
    std::cout << BOLD << BLUE << "========================================" << RESET << std::endl;
    std::cout << BOLD << BLUE << "    Database と Client テストスイート" << RESET << std::endl;
    std::cout << BOLD << BLUE << "========================================" << RESET << std::endl;

    try {
        std::cout << "\n" << BOLD << WHITE << "Server、Database、Clientクラスの包括的テストを実行中..." << RESET << std::endl;

        testDatabase();
        testClient();
        testDatabaseClientIntegration();
        testStress();
        testEdgeCases();

        std::cout << "\n" << BOLD << BLUE << "========================================" << RESET << std::endl;
        std::cout << BOLD << BLUE << "              テスト結果まとめ" << RESET << std::endl;
        std::cout << BOLD << BLUE << "========================================" << RESET << std::endl;
        std::cout << BOLD << GREEN << "すべてのテストスイートが完了しました。" << RESET << std::endl;
        std::cout << "詳細情報については上記の個別テスト結果を確認してください。" << std::endl;
        std::cout << BOLD << "各テストには以下の情報が含まれています：" << RESET << std::endl;
        std::cout << "  - " << CYAN << "テスト対象のファイルディスクリプタ" << RESET << std::endl;
        std::cout << "  - " << CYAN << "オブジェクトのメモリアドレス（16進数表記）" << RESET << std::endl;
        std::cout << "  - " << CYAN << "バッファの内容と比較結果" << RESET << std::endl;
        std::cout << "  - " << CYAN << "期待値と実際の値" << RESET << std::endl;
        std::cout << "\n" << RED << BOLD << "[失敗]" << RESET << "と表示されたテストがある場合は、詳細を確認してデバッグしてください。" << std::endl;
        std::cout << BOLD << BLUE << "========================================" << RESET << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\n" << RED << BOLD << "[エラー]" << RESET << " テスト実行中に例外が発生しました: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "\n" << RED << BOLD << "[エラー]" << RESET << " テスト実行中に不明な例外が発生しました" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
