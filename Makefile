.PHONY: all clean

all: clienteTCP servidorTCP

clienteTCP: clienteTCP.c
	gcc -o cliente clienteTCP.c

servidorTCP: servidorTCP.c
	gcc -o servidor servidorTCP.c

all: clienteTCP servidorTCP
	@echo "Se ha compilado sin problemas"

clean:
	rm -f clienteTCP servidorTCP

