/* udpServer.c */

#include<stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */

#define LOCAL_SERVER_PORT 2017
#define MAX_MSG 100
#define REMOTE_SERVER_PORT 2018
int sendACK(){

  int sdACK, rc, i;
  struct sockaddr_in cliAddr, remoteServAddr;
  struct hostent *h;

  h = gethostbyname("localhost");
  if(h==NULL) {
    printf("unknown host");
    exit(1);
  }

  printf("sending ACK '%s' (IP : %s) \n", h->h_name,
   inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));

  remoteServAddr.sin_family = h->h_addrtype;
  memcpy((char *) &remoteServAddr.sin_addr.s_addr,
   h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

  /* socket creation */
  sdACK = socket(AF_INET,SOCK_DGRAM,0);
  if(sdACK<0) {
    printf("cannot open socket \n");
    exit(1);
  }

  /* bind any port */
  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(0);

  rc = bind(sdACK, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if(rc<0) {
    printf("cannot bind port\n");
    exit(1);
  }


  /* send data */
    rc = sendto(sdACK, "ACK", 3, 0,
    (struct sockaddr *) &remoteServAddr,
    sizeof(remoteServAddr));

    if(rc<0) {
      printf("cannot send ACK\n");
      close(sdACK);
      exit(1);
    }


}
int main(int argc, char *argv[]) {

  int sd, rc, n, cliLen;
  struct sockaddr_in cliAddr, servAddr;
  char msg[MAX_MSG];

  /* socket creation */
  sd=socket(AF_INET, SOCK_DGRAM, 0);
  if(sd<0) {
    printf("%s: cannot open socket \n",argv[0]);
    exit(1);
  }

  /* bind local server port */
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(LOCAL_SERVER_PORT);
  rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
  if(rc<0) {
    printf("%s: cannot bind port number %d \n",
	   argv[0], LOCAL_SERVER_PORT);
    exit(1);
  }

  printf("%s: waiting for data on port UDP %u\n",
	   argv[0],LOCAL_SERVER_PORT);

  /* server infinite loop */
  while(1) {

    /* init buffer */
    memset(msg,0x0,MAX_MSG);


    /* receive message */
    cliLen = sizeof(cliAddr);
    n = recvfrom(sd, msg, MAX_MSG, 0,
		 (struct sockaddr *) &cliAddr, &cliLen);

    if(n<0) {
      printf("%s: cannot receive data \n",argv[0]);
      continue;
    }
    sendACK();
    if (strncmp(msg, "quit", 5) == 0){
      printf("saindo da aplicação\n");
      return 0;
    }
    /* print received message */
    printf("%s: from %s:UDP%u : %s \n",
	   argv[0],inet_ntoa(cliAddr.sin_addr),
	   ntohs(cliAddr.sin_port),msg);

  }/* end of server infinite loop */

return 0;

}


/*
udpServer: waiting for data on port UDP 1500
from 84.242.66.126:UDP47542 : World
from 127.0.0.1:UDP32931 : Hello
from 127.0.0.1:UDP32931 : World
*/
