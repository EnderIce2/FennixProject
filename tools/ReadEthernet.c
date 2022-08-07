/* I don't remember where I got this code from, but it seems to work fine for me. */

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

enum FrameType
{
	TYPE_IPV4 = 0x0800,
	TYPE_ARP = 0x0806,
	TYPE_RARP = 0x8035,
	TYPE_IPV6 = 0x86DD
};

#define ETHER_TYPE TYPE_ARP
#define DEFAULT_IF "enp7s0"
#define BUF_SIZ 1024

int main(int argc, char *argv[])
{
	char sender[INET6_ADDRSTRLEN];
	int sockfd, ret, i;
	int sockopt;
	ssize_t numbytes;
	struct ifreq ifopts;
	struct ifreq if_ip;
	struct sockaddr_storage their_addr;
	uint8_t buf[BUF_SIZ];
	char ifName[IFNAMSIZ];
	int ether_type = TYPE_ARP;

	if (argc > 1)
	{
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
		{
			printf("Usage: sudo %s [type] [interface]\nTypes:\n    0 - IPv4    1 - ARP    2 - RARP    3 - IPv6\nExample: sudo %s 1 enp7s0", argv[0], argv[0]);
			return 0;
		}
		if (argv[1] > '9' || argv[1] < '0')
		{
			switch (atoi(argv[1]))
			{
			case 0:
				ether_type = TYPE_IPV4;
				break;
			case 1:
				ether_type = TYPE_ARP;
				break;
			case 2:
				ether_type = TYPE_RARP;
				break;
			case 3:
				ether_type = TYPE_IPV6;
				break;
			default:
				printf("Invalid type.\n");
				return -1;
			}
		}
		else
		{
			printf("Invalid ethernet type \"%d\".", atoi(argv[1]));
			return -1;
		}
	}

	if (argc > 2)
		strcpy(ifName, argv[2]);
	else
		strcpy(ifName, DEFAULT_IF);

	printf("Using interface \"%s\" with type %#lx \n", ifName, ether_type);

	if (geteuid() != 0)
	{
		perror("You must be root to run this program.");
		return -1;
	}

	struct ether_header *eh = (struct ether_header *)buf;
	struct iphdr *iph = (struct iphdr *)(buf + sizeof(struct ether_header));
	struct udphdr *udph = (struct udphdr *)(buf + sizeof(struct iphdr) + sizeof(struct ether_header));

	memset(&if_ip, 0, sizeof(struct ifreq));

	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1)
	{
		perror("Error creating socket");
		return -1;
	}

	strncpy(ifopts.ifr_name, ifName, IFNAMSIZ - 1);
	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1)
	{
		perror("setsockopt failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ - 1) == -1)
	{
		perror("Failed to bind socket to device");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

repeat:
	printf("Waiting to recvfrom... ");
	numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
	printf("got packet %lu bytes\n", numbytes);

	if (eh->ether_dhost[0] == 0 && eh->ether_dhost[1] == 0 && eh->ether_dhost[2] == 0 && eh->ether_dhost[3] == 0 && eh->ether_dhost[4] == 0 && eh->ether_dhost[5] == 0)
		printf("MAC <: %x:%x:%x:%x:%x:%x\n", eh->ether_dhost[0], eh->ether_dhost[1], eh->ether_dhost[2], eh->ether_dhost[3], eh->ether_dhost[4], eh->ether_dhost[5]);
	else
	{
		printf("MAC X: %x:%x:%x:%x:%x:%x\n", eh->ether_dhost[0], eh->ether_dhost[1], eh->ether_dhost[2], eh->ether_dhost[3], eh->ether_dhost[4], eh->ether_dhost[5]);
		ret = -1;
		goto done;
	}

	((struct sockaddr_in *)&their_addr)->sin_addr.s_addr = iph->saddr;
	inet_ntop(AF_INET, &((struct sockaddr_in *)&their_addr)->sin_addr, sender, sizeof sender);

	strncpy(if_ip.ifr_name, ifName, IFNAMSIZ - 1);
	if (ioctl(sockfd, SIOCGIFADDR, &if_ip) >= 0)
	{
		printf("Source IP: %s    My IP: %s\n", sender, inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
		if (strcmp(sender, inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr)) == 0)
		{
			ret = -1;
			goto done;
		}
	}

	ret = ntohs(udph->len) - sizeof(struct udphdr);
done:
	goto repeat;
	close(sockfd);
	return ret;
}