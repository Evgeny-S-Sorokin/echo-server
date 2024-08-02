#include <iostream>

#include <cstring>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
	int n;
	int c;
	int sockfd;
	char buf[1024];
	sockaddr_in srv_addr;

	std::memset(&srv_addr, 0, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	srv_addr.sin_port = htons(12555);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
		perror("connect()");
		exit(1);
	}

	for (;;) {
		printf("input: ");
		fgets(buf, sizeof(buf), stdin);
		c = strlen(buf) - 1;
		buf[c] = '\0';
		write(sockfd, buf, c + 1);

		bzero(buf, sizeof(buf));
		while (errno != EAGAIN
		       && (n = read(sockfd, buf, sizeof(buf))) > 0) {
			printf("echo: %s\n", buf);
			bzero(buf, sizeof(buf));

			c -= n;
			if (c <= 0) {
				break;
			}
		}
	}
	close(sockfd);
    return 0;
}