.PHONY: release

release:
	@cmake --preset=release
	@cmake --build release

test:
	@cmake --preset=test
	@cmake --build build
	#@./build/tests/gtest_main

run_test:
	# for example: make run_test cases='ToString.*'
	# run all test: make run_test cases='*'
	@./build/tests/gtest_main --gtest_filter=$(cases)

