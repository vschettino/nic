
/* udpClient.c */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> /* memset() */
#include <sys/time.h> /* select() */

#define REMOTE_SERVER_PORT 2017
#define MAX_MSG 100
#define LOCAL_SERVER_PORT 2018

int waitForACK(){
  int sd, rc, n, cliLen;
  struct sockaddr_in cliAddr, servAddr;
  char msg[MAX_MSG];

  /* socket creation */
  sd=socket(AF_INET, SOCK_DGRAM, 0);
  if(sd<0) {
    printf("waitForAck: cannot open socket \n");
    exit(1);
  }

  /* bind local server port */
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(LOCAL_SERVER_PORT);
  rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
  if(rc<0) {
    printf("waitForAck: cannot bind port number %d \n",
     LOCAL_SERVER_PORT);
    exit(1);
  }

  printf("waiting the ACK...\n");

  /* server infinite loop */
  while(1) {

    /* init buffer */
    memset(msg,0x0,MAX_MSG);


    /* receive message */
    cliLen = sizeof(cliAddr);
    n = recvfrom(sd, msg, MAX_MSG, 0,
     (struct sockaddr *) &cliAddr, &cliLen);

    if(n<0) {
      printf("waitForAck: cannot receive data \n");
      continue;
    }
    if (strncmp(msg, "ACK", 3) == 0){
      printf("ACK recebido!\n");
      close(sd);
      return 0;
    }
    /* print received message */
    printf("waitForAck: from %s:UDP%u : %s \n",
     inet_ntoa(cliAddr.sin_addr),
     ntohs(cliAddr.sin_port),msg);

  }/* end of server infinite loop */

return 0;
shutdown(sd, 2);
}

int main(int argc, char *argv[]) {

  int sd, rc, i;
  struct sockaddr_in cliAddr, remoteServAddr;
  struct hostent *h;
  char data[10];
  /* check command line args */
  if(argc<2) {
    printf("usage : %s <server> \n", argv[0]);
    exit(1);
  }
  while(1){
  scanf("%s",&data);
  /* get server IP address (no check if input is IP address or DNS name */
  h = gethostbyname(argv[1]);
  if(h==NULL) {
    printf("%s: unknown host '%s' \n", argv[0], argv[1]);
    exit(1);
  }

  remoteServAddr.sin_family = h->h_addrtype;
  memcpy((char *) &remoteServAddr.sin_addr.s_addr,
	 h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

  /* socket creation */
  sd = socket(AF_INET,SOCK_DGRAM,0);
  if(sd<0) {
    printf("%s: cannot open socket \n",argv[0]);
    exit(1);
  }

  /* bind any port */
  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(0);

  rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if(rc<0) {
    printf("%s: cannot bind port\n", argv[0]);
    exit(1);
  }


  /* send data */
    rc = sendto(sd, data, strlen(data)+1, 0,
		(struct sockaddr *) &remoteServAddr,
		sizeof(remoteServAddr));
    if(rc<0) {
      printf("%s: cannot send '%s' \n",argv[0], data);
      close(sd);
      exit(1);
    }
    printf("%s: sent '%s' to '%s' (IP : %s) \n", argv[0],data, h->h_name,
     inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));
     waitForACK();
     if(strncmp(data, "bye",3) == 0){
       printf("envio terminado! Bye\n");
       return 1;
     }
}

}
/*

./udpClient localhost Hello World

*/
