BINARIES=final

all:	${BINARIES}

final: main.cpp 
	g++ -std=c++11 main.cpp -o final

clean:
	/bin/rm -rf *.o *~ ${BINARIES}
