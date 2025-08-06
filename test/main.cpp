#include <iostream>

/*
	--- 単体テストの使用方法 ---
	1. testディレクトリ内に、main関数を実装したC++ファイルを作成してください。

	2. testディレクトリ内で、以下のコマンドを実行:
	make re MAIN=your_test_file.cpp
	(your_test_file.cpp の部分を、ステップ1で作成したファイル名に置き換えてください。)
	例: make re MAIN=cmd_test.cpp

	3. ./test_irc で実行
	--------------------------
*/
int main()
{
	std::cout << "--- 単体テストの使用方法 ---" << std::endl;
	std::cout << "1. testディレクトリ内に、main関数を実装したC++ファイルを作成してください。" << std::endl;
	std::cout << std::endl;
	std::cout << "2. testディレクトリ内で、以下のコマンドを実行:" << std::endl;
	std::cout << "   make re MAIN=your_test_file.cpp" << std::endl;
	std::cout << "   (your_test_file.cpp の部分を、ステップ1で作成したファイル名に置き換えてください。)" << std::endl;
	std::cout << "   例: make re MAIN=cmd_test.cpp" << std::endl;
	std::cout << std::endl;
	std::cout << "3. ./test_irc で実行" << std::endl;
	std::cout << "--------------------------" << std::endl;
	return 0;
}
