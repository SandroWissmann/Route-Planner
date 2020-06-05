.PHONY: format
format:
	find . -regex '.*\.\(cpp\|hpp\|cu\|c\|h\)' -exec clang-format -style=file -i {} \;
.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make -j${nproc}	

.PHONY: debug
debug:
	mkdir -p build
	cp /le
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	make -j${nproc}

.PHONY: clean
clean:
	rm -rf build

.PHONY: memcheck
memcheck:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	make -j${nproc}
	valgrind --leak-check=full --show-reachable=no --show-leak-kinds=all --track-origins=yes --log-file="valgrindReport" ./build/route_planner	
