.PHONY: release

release:
	@cmake --preset=release
	@cmake --build release

