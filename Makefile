all:
	clear
	gcc -Wall vcd.c -o vcd

run:
	./vcd

runTest:
	gcc -Wall test.c -o test
	./test
