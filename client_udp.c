/* CSD 304 Computer Networks, Fall 2016
   Lab 2, client
   Team:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5432
// #define BUF_SIZE 4096

int main(int argc, char **argv)
{
  int bufSize = atoi(argv[2]); // To convert char to int
  // bufSize /= 1000; // Convert into Kilo Bytes
  FILE *inputFile = fopen("RecievedFile.mp4", "wb");
  int bytesread;
  FILE *fp;
  struct hostent *hp;
  struct sockaddr_in sin;
  socklen_t length;
  char *host;
  char buf[bufSize];
  int s;
  int len;
  if (argc < 3)
  {
    printf("Please provide 2 additional arguments : Host name & Payload size  \n");
    return 0;
  }
  if ((argc == 2) || (argc == 3))
  {
    host = argv[1];
  }
  else
  {
    fprintf(stderr, "usage: client serverIP [download_filename(optional)]\n");
    exit(1);
  }

  // if (argc == 3)
  // {
  //   fp = fopen(argv[2], "w");
  //   if (fp == NULL)
  //   {
  //     fprintf(stderr, "Error opening output file\n");
  //     exit(1);
  //   }
  // }

  /* translate host name into peer's IP address */
  hp = gethostbyname(host);
  if (!hp)
  {
    fprintf(stderr, "client: unknown host: %s\n", host);
    exit(1);
  }
  else
    printf("Host %s found!\n", argv[1]);

  /* build address data structure */
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
  sin.sin_port = htons(SERVER_PORT);

  /* create socket */
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("client: socket");
    exit(1);
  }

  printf("Client will get data from to %s:%d\n", argv[1], SERVER_PORT);
  printf("To play the music, pipe the downlaod file to a player, e.g., ALSA, SOX, VLC: cat recvd_file.wav | vlc -\n");

  /* send message to server */
  fgets(buf, sizeof(buf), stdin);
  buf[bufSize - 1] = '\0';
  len = strlen(buf) + 1;
  if (sendto(s, buf, len, 0, (const struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    perror("Client: sendto()");
    return 0;
  }

  /* get reply, display it or store in a file*/
  /* Add code to receive unlimited data and either display the data
     or if specified by the user, store it in the specified file.
     Instead of recv(), use recvfrom() call for receiving data */

  // recv(s, buf, sizeof(buf), 0);
  // fputs(buf, stdout);

  length = sizeof(sin);
  while (1)
  {
    bytesread = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &length);
    if (strcmp(buf, "BYE\n") == 0)
    {
      fputs(buf, stdout);
      break;
    }
    fwrite(buf, bytesread, 1, inputFile);
  }
}
