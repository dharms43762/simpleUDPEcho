# Makefile for the simpelUDPEcho client/server project
#

all : UDPEchoServer UDPEchoClient

UDPEchoServer: UDPEchoServer.c
	gcc UDPEchoServer.c -o UDPEchoServer

UDPEchoClient: UDPEchoClient.c
	gcc UDPEchoClient.c -o UDPEchoClient



clean:
	rm -f *.o UDPEchoServer UDPEchoClient
