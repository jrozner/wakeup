#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "wakeup.h"

int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in theiraddr;
  struct hostent *he;
  uint8_t mac[6];
  uint8_t packet[102] = {'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF'};
  int broadcast = 1;

  if (argc != 3)
  {
    fprintf(stderr, "Invalid Arguments\nUseage: wakeup <host> <mac>\n");
    exit(1);
  }
  
  if ((he = gethostbyname(argv[1])) == NULL)
  {
    perror("gethostbyname");
    exit(1);
  }

  if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }

  if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1)
  {
    perror("setsockopt (SO_BROADCAST)");
    exit(1);
  }

  memset(&theiraddr, 0, sizeof(theiraddr));
  theiraddr.sin_family = PF_INET;
  theiraddr.sin_port = htons(PORT);
  theiraddr.sin_addr = *((struct in_addr *) he->h_addr);

  sscanf(argv[2], "%2x:%2x:%2x:%2x:%2x:%2x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

  generatePacket(mac, packet);

  if ((sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&theiraddr, sizeof(struct sockaddr))) == -1)
  {
    perror("sendto");
    exit(1);
  }

    printf("Sent a wakeup call to %s.\n", inet_ntoa(theiraddr.sin_addr));

  close(sockfd);
  return 0;
}

void generatePacket(uint8_t *mac, uint8_t *packet)
{
  int i, j, k;
  k = 6;

  for (i = 0; i <= 15; i++)
  {
    for (j = 0; j <= 5; j++, k++)
    {
      packet[k] = mac[j];
    }
  }
}

