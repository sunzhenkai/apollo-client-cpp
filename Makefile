.PHONY: release

release:
	@cmake --preset=release
	@cmake --build release

test:
	@cmake --preset=test
	@cmake --build build
	@./build/tests/gtest_main

