//
// File: UDPEchoClient.c
//
// Description: UDP client that sends a UDP packet to a server and waits
// for the packet to be echoed back.
//

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define ECHOMAX 255     /* Longest string to echo */
#define DEFAULT_PORT 5555  /* default port, if none specified oncommand line */

/* Print error message and exit */
void DieWithError(char *errorMessage)
{
  perror(errorMessage);
  exit(1);
}

/* Read a string from the keyboard.  Reading stops when a newline or eof is entered.
   At most len-1 characters are read and placed in buf.  When eof is entered, NULL
   is returned; otherwise buf is returned.
*/ 
char* readString (char *buf,int len){
  char *s;
  // read from stdin until newline or eof entered
  while ((s=fgets(buf,len,stdin))==0 && !feof(stdin));
  
  // if eof not entered, remove newline from buffer
  if( s != NULL )
    s[strlen(s) - 1] = '\0';
  
  // return the string (or NULL if eof)
  return s;
}

int main(int argc, char *argv[])
{
  int sock;                        /* Socket descriptor */
  struct sockaddr_in echoServAddr; /* Echo server address */
  struct sockaddr_in fromAddr;     /* Source address of echo */
  unsigned short echoServPort;     /* Echo server port */
  unsigned int fromSize;           /* In-out of address size for recvfrom() */
  char *servIP;                    /* IP address of server */
  char echoString[ECHOMAX+1];      /* string to send to echo server */
  char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
  int echoStringLen;               /* Length of string to echo */
  int respStringLen;               /* Length of received response */
  
  if (argc < 2 || argc > 3 )       /* Test for correct number of arguments */
    {
      fprintf(stderr,"Usage: %s <Server IP> [<Echo Port>]\n", 
	      argv[0]);
      exit(1);
    }
  
  servIP = argv[1];           /* First arg: server IP address (dotted quad) */
  
  if( argc == 3 )
    echoServPort = atoi(argv[2]);  /* Use specified port */
  else
    echoServPort = DEFAULT_PORT;
 
  /* Create a datagram/UDP socket */
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    DieWithError("socket() failed");
  
  /* Construct the server address structure */
  memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
  echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
  echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
  echoServAddr.sin_port   = htons(echoServPort);     /* Server port */
  
  // read strings and send them to the server until eof (^d) entered
  printf("Enter strings to send to the echo server.  Terminate with ^d.\n");
  while (readString(echoString,ECHOMAX) != NULL )
  {
    // calculate the length of the string
    echoStringLen = strlen(echoString);
    
    /* Send the string to the server (including the nul character) */
    if (sendto(sock, echoString, echoStringLen+1, 0, 
	       (struct sockaddr *) &echoServAddr, 
	       sizeof(echoServAddr)) != echoStringLen+1)
      DieWithError("sendto() sent a different number of bytes than expected");
  
    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, 
				  (struct sockaddr *) &fromAddr, 
				  &fromSize)) != echoStringLen+1)
      DieWithError("received message was not the expected size");
  
    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
      fprintf(stderr,"Error: received a packet from unknown source.\n");
      exit(1);
    }
  
    printf("Received: %s\n", echoBuffer);    /* Print the echoed arg */
  
    /* check that the received string was the same as we sent */
    for( int i=0; i<echoStringLen; i++ )
      if( echoString[i] != echoBuffer[i] )
        DieWithError( "echo'd string not what we expected" );
    printf("Received string is correct\n");
  }
  
  close(sock);
  exit(0);
}
