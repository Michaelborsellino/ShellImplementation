all:shell myls

shell:shelldriver.o
	g++ -oshell -g -Wall -pedantic shelldriver.o
myls:myls.o
	g++ -omyls -Wall -pedantic myls.o
.PHONY: clean
clean: 
	rm *.o
