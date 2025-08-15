#!/bin/bash

CXX="c++"
CXX_FLAG="-Wall -Wextra -Werror -std=c++98 -pedantic"

SRCS=$(ls ../src/*.cpp | grep -v "main.cpp")
INC_DIR="../includes"
INCLUDES="-I$INC_DIR"

# make testで実行する場合
BASE_DIR="test/"

# すべてのテストを実行する
UNIT_TESTS=$BASE_DIR"test_*.cpp"
OUT_DIR=$BASE_DIR"bin/"

echo $UNIT_TESTS

echo "Running unit tests..."
mkdir -p $OUT_DIR
	for test in $UNIT_TESTS
	do
		test_name=$(basename "$test" .cpp)
		out_path="$OUT_DIR$test_name.out"
		if $CXX $CXX_FLAG $test $SRCS $INCLUDES -o $out_path && ./$out_path
			then
			echo "Passed: $test"
		else
			echo "Failed: $test"
			rm -rf $OUT_DIR
			exit 1
		fi
	done
echo "⭐️Passed all unit tests!⭐️"
rm -rf $OUT_DIR
