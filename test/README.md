## 単体テストの使い方

### 1. main関数の実装
 `test`ディレクトリ内に、main関数を実装したC++ファイルを作成



### 2. コンパイル
`test`ディレクトリ内で、以下のコマンド実行

```bash
make re MAIN=your_test_file.cpp
```

例) `cmd_test.cpp`をテストしたい場合

```bash
make re MAIN=cmd_test.cpp
```


### 3. 実行

コンパイルが完了したら、生成された実行ファイルを実行。

```bash
./test_irc
```
