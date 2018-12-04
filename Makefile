
# Note: each line of makefile is run in its own shell, which is why multiple commands are on one line.
default:
	mkdir -p build/release
	mkdir -p build/release-static
	cd build/release; cmake ../..; make
	cd build/release-static; cmake ../.. -DBUILD_STATIC_LIBRARIES=ON; make

clean:
	rm -rf build
	
install: build
	cd build/release; make install
	cd build/release-static; make install
