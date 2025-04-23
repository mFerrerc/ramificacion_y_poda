# Miguel Ferrer Castellá 
OPTS = -O3 -std=c++0x

#poner make solo ejecuta solo el primero
all: mcp_bb

mcp_bb: mcp_bb.cc
	g++ ${OPTS} -o mcp_bb mcp_bb.cc	

tar: mcp_bb.cc makefile
	tar cvfz 18071705F.tar.gz mcp_bb.cc makefile memoria.pdf

clean: 
	rm -f mcp_bb *~ 18071705F.tar.gz 
