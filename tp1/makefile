local:
	gcc -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -o tp1 *.c -lm
	./tp1
	valgrind -s --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes ./tp1
	rm tp1

docker:
	docker build --tag tp1_udesa .
	docker run -v $(CURDIR):/tp tp1_udesa
	docker run tp1_udesa
