all:
	clear
	gcc -Wall vcd.c -o vcd

run:
	./vcd

clean:
	rm vcd
	rm *~
