all:shell

shell:shelldriver.o
	g++ -oshell -Wall -pedantic shelldriver.o
