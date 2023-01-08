#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

// codul de eroare returnat de anumite apeluri 
extern int errno;

//portul de conectare la server/
int port;

int main(int argc, char *argv[])
{
  int sd;                    // descriptorul de socket
  struct sockaddr_in server; // structura folosita pentru conectare
  char cmd[1000];             // comanda transmisa

  // exista toate argumentele in linia de comanda? 
  if (argc != 3)
  {
    printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
    return -1;
  }

  // stabilim portul 
  port = atoi(argv[2]);

  // cream socketul 
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Eroare la socket().\n");
    return errno;
  }

  // umplem structura folosita pentru realizarea conexiunii cu serverul /
  // familia socket-ului /
  server.sin_family = AF_INET;
  // adresa IP a serverului /
  server.sin_addr.s_addr = inet_addr(argv[1]);
  //portul de conectare /
  server.sin_port = htons(port);

  // ne conectam la server /
  if (connect(sd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[client]Eroare la connect().\n");
    return errno;
  }
  while (1)
  {
    // citirea mesajului 
    bzero(cmd, 100);
    printf("[client]Introduceti o comanda: \n");
    fflush(stdout);
    read(0, cmd, 100);

    printf("[client] Am citit comanda %s\n", cmd); // ati apelat comanda

    


    //trimiterea mesajului la server /
    if (write(sd, cmd, 100) <= 0)
    {
      perror("[client]Eroare la write() spre server.\n");
      return errno;
    }

    //citirea raspunsului dat de server 
    //apel blocant paana cind serverul raspunde) /
    if (read(sd, cmd, 1000) < 0)
    {
      perror("[client]Eroare la read() de la server.\n");
      return errno;
    }
    if (strcmp(cmd, "disconnect\n") == 0) // trb parse cmd
          exit(0);
    
    // afisam mesajul primit 
    printf("[client]Comanda primita este:\n%s\n", cmd);
  }
  // inchidem conexiunea, am terminat 
  //close(sd);
}