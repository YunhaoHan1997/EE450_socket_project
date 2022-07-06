all:
	g++ -std=c++11 serverM.cpp -o serverM
	g++ -std=c++11 client.cpp -o client
	g++ -std=c++11 monitor.cpp -o monitor
	g++ -std=c++11 serverA.cpp -o serverA
	g++ -std=c++11 serverB.cpp -o serverB
	g++ -std=c++11 serverC.cpp -o serverC

.PHONY: serverM
serverM:
	./serverM

.PHONY:serverA
serverA:
	./serverA

.PHONY:serverB
serverB:
	./serverB

.PHONY:serverC
serverC:
	./serverC



#CC = g++
#CFLAGS = -std=c++11
#
#all: serverAoutput serverBoutput serverCoutput serverMoutput client monitor
#
#serverAoutput: serverA.cpp
#	$(CC) $(CFLAGS) -o serverA.o -c serverA.cpp
#	$(CC) $(CFLAGS) -o serverAoutput serverA.o
#
#serverBoutput: serverB.cpp
#	$(CC) $(CFLAGS) -o serverB.o -c serverB.cpp
#	$(CC) $(CFLAGS) -o serverBoutput serverB.o
#
#serverCoutput: serverC.cpp
#	$(CC) $(CFLAGS) -o serverC.o -c serverC.cpp
#	$(CC) $(CFLAGS) -o serverCoutput serverC.o
#
#serverMoutput: serverM.cpp
#	$(CC) $(CFLAGS) -o serverM.o -c serverM.cpp
#	$(CC) $(CFLAGS) -o serverM serverM.o
#
#client: client.cpp
#	$(CC) $(CFLAGS) -o client.o -c client.cpp
#	$(CC) $(CFLAGS) -o client client.o
#
#monitor: monitor.cpp
#	$(CC) $(CFLAGS) -o monitor.o -c monitor.cpp
#	$(CC) $(CFLAGS) -o monitor monitor.o
#
#serverA:
#	@./serverAoutput
#serverB:
#	@./serverBoutput
#serverC:
#	@./serverCoutput
#serverM:
#	@./serverMoutput
#clean:
#	@rm -f *.o *output client
#	@rm -f *.o *output monitor