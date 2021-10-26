CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror
pwcheck: pwcheck.c
	$(CC) -o pwcheck pwcheck.c

test: ./tests/test.c
	gcc -std=c99 -o ./tests/test ./tests/test.c
	
run: pwcheck.c
	./pwcheck 2 2 --stats

test1:
	@./pwcheck 1 1 <example.txt >output.txt 
	@./tests/test ./tests/test1.txt output.txt 1

test2: 
	@./pwcheck 2 3 <example.txt >output.txt
	@./tests/test ./tests/test2.txt output.txt 2 

test3:
	@./pwcheck 3 2 <example.txt >output.txt 
	@./tests/test ./tests/test3.txt output.txt 3

test4:
	@./pwcheck 4 2 <example.txt >output.txt 
	@./tests/test ./tests/test4.txt output.txt 4

test5:
	@./pwcheck 2 4 --stats <example.txt >output.txt
	@./tests//test ./tests/test5.txt output.txt 5

testall:
	echo Testing all inputs
	@make test1 
	@make test2 
	@make test3 
	@make test4 
	@make test5

test11:
	@./pwcheck -l 1 -p 1 <example.txt >output.txt 
	@./tests/test ./tests/test1.txt output.txt 1

test22: 
	@./pwcheck -l 2 -p 3 <example.txt >output.txt
	@./tests/test ./tests/test2.txt output.txt 2 

test33:
	@./pwcheck -l 3 -p 2 <example.txt >output.txt 
	@./tests/test ./tests/test3.txt output.txt 3

test44:
	@./pwcheck -l 4 -p 2 <example.txt >output.txt 
	@./tests/test ./tests/test4.txt output.txt 4

test55:
	@./pwcheck -l 2 -p 4 --stats <example.txt >output.txt
	@./tests//test ./tests/test5.txt output.txt 5

testallpos:
	echo Testing all inputs
	@make test1 
	@make test2 
	@make test3 
	@make test4 
	@make test5

testallopt:
	echo Testing all inputs
	@make test11 
	@make test22 
	@make test33 
	@make test44
	@make test55

clean:
	@rm -f output.txt

indent:
	indent -linux pwcheck.c 


testinput:
	py test.py --bonus pwcheck
