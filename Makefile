run: link
	./output

link: assemble
	ld -o output output.o \
		-lSystem \
		-syslibroot `xcrun -sdk macosx --show-sdk-path` \
		-e _start \
		-arch arm64

assemble: compile
	as -o output.o output.s

compile: build
	./ntc test.nt output.s

build: src/main.cpp
	g++-13 -std=c++20 -Wall -Wextra src/main.cpp -o ntc

clean:
	rm ntc output.s output.o output
