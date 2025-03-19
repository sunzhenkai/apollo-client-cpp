.PHONY: release build test

release:
	@cmake --preset=release
	@cmake --build release

build:
	@cmake --preset=test
	@cmake --build build

test:
	# for example: make run_test cases='ToString.*'
	# run all test: make run_test cases='*'
	@./build/tests/gtest_main --gtest_filter=$(cases)

test-all:
	@./build/tests/gtest_main --gtest_filter='*'
