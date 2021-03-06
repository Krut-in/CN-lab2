/* CSD 304 Computer Networks, Fall 2016
   Lab 2, server
   Team: KR
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define SERVER_PORT 5432
// #define BUF_SIZE 4096

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    printf("Please provide 2 additional arguments : Payload size & delay in nanoseconds \n");
    return 0;
  }
  struct timespec tim, tim2;
  tim.tv_sec = 1;
  tim.tv_nsec = atoi(argv[2]);

  struct sockaddr_in sin;
  struct sockaddr_storage client_addr;
  char clientIP[INET_ADDRSTRLEN]; /* For IPv4 addresses */
  socklen_t client_addr_len;
  int bufSize = atoi(argv[1]);
  // bufSize /= 1000; // Convert into Kilo Bytes
  char buf[bufSize];
  // char GETFOUND[BUF_SIZE] = "File is Sent!\n";
  int len;
  int s;
  char *host;
  struct hostent *hp;

  // Declarations for file(s) to be sent
  // Relative location of the file
  FILE *videoFile = fopen("./Files/Our Story in 1 Minute.mp4", "rb");

  /* For inserting delays, use nanosleep()
     struct timespec ... */

  /* To get filename from commandline */
  /* if (argc==...) {} 3rd argument as file name */

  /* Create a socket */
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("server: socket");
    exit(1);
  }

  /* build address data structure and bind to all local addresses*/
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;

  /* If socket IP address specified, bind to it. */
  if (argc == 2)
  {
    host = argv[1];
    hp = gethostbyname(host);
    if (!hp)
    {
      fprintf(stderr, "server: unknown host %s\n", host);
      exit(1);
    }
    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
  }
  /* Else bind to 0.0.0.0 */
  else
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
  // sin.sin_addr.s_addr = inet_addr("10.0.2.15");

  sin.sin_port = htons(SERVER_PORT);

  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0)
  {
    perror("server: bind");
    exit(1);
  }
  else
  {
    /* Add code to parse IPv6 addresses */
    inet_ntop(AF_INET, &(sin.sin_addr), clientIP, INET_ADDRSTRLEN);
    printf("Server is listening at address %s:%d\n", clientIP, SERVER_PORT);
  }

  printf("Client needs to send \"GET\" to receive the file %s\n", argv[1]);

  client_addr_len = sizeof(client_addr);

  /* Receive messages from clients*/
  while (len = recvfrom(s, buf, sizeof(buf), 0,
                        (struct sockaddr *)&client_addr, &client_addr_len))
  {

    inet_ntop(client_addr.ss_family,
              &(((struct sockaddr_in *)&client_addr)->sin_addr),
              clientIP, INET_ADDRSTRLEN);
    printf("Server got message from %s: %s [%d bytes]\n", clientIP, buf, len);
    /* Send to client */
    /* Add code to send file if the incoming message is GET */
    buf[len] = '\0';
    if (strcmp(buf, "GET\n") == 0)
    {
      for (;;) // loop for ever
      {
        size_t bytesread = fread(buf, 1, sizeof(buf), videoFile);

        // bytesread contains the number of bytes actually read
        if (bytesread == 0)
        {
          // no bytes read => end of file
          strcpy(buf, "BYE\n");
        }
        if ((len = sendto(s, buf, sizeof(buf), 0, (const struct sockaddr *)&client_addr, client_addr_len)) == -1)
        {
          perror("server: sendto");
          exit(1);
        }
        nanosleep(&tim, &tim2);
        if (bytesread == 0)
        {
          break;
        }
      }
    }
    memset(buf, 0, sizeof(buf));
  }

  /* Send BYE to signal termination */
  strcpy(buf, "BYE\n");
  sendto(s, buf, sizeof(buf), 0,
         (struct sockaddr *)&client_addr, client_addr_len);
}

// vlc /Users/krutinrahtod/Downloads/Wishlist.mkv