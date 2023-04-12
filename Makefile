.PHONY: run

run:
	@mkdir -p bin
	@gcc -o bin/main src/main.c
	@./bin/main