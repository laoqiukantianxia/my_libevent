#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define SERVER_PORT 50000
#define SERVER_IP   "10.60.25.26"

char rot13_char(char c)
{
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
		return c + 13;

    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
		return c - 13;
    else 
		return c;
}

void child(int fd)
{
    char outbuf[10240 + 1];
	size_t outbuf_used = 0;
	ssize_t result;

	while(1) {
	    char ch;
		result = recv(fd, &ch, 1, 0);
		if (result == 0) {
		    break;
		}
		else if (result == -1) {
		    perror("read");
			break;
		}

		if (outbuf_used < sizeof(outbuf)) {
		    outbuf[outbuf_used++] = rot13_char(ch);
		}

		if (ch == '\n') {
		    send(fd, outbuf, outbuf_used, 0);
			outbuf_used = 0;
			continue;
		}
	}
	printf("%s\n",outbuf);
}

void run(void)
{
    int listener;
	struct sockaddr_in sin;

	sin.sin_family = AF_INET;
	sin.sin_port = htons(50000);
	sin.sin_addr.s_addr = inet_addr(SERVER_IP);

	listener = socket(AF_INET, SOCK_STREAM, 0);

	if (bind(listener, (struct sockaddr*)&sin, sizeof(sin))){
	    perror("bind");
		return;
	}

	if (listen(listener, 16) < 0) {
	    perror("listen");
		return;
	}

	while(1) {
	    struct sockaddr_storage ss;
		socklen_t slen = sizeof(ss);
		int fd = accept(listener, (struct sockaddr*)&ss, &slen);
		if (fd < 0) {
		    perror("accept");
		}
		else {
		    if (fork() == 0) {
			    child(fd);
				exit(0);
			}
		}
	}
}

int main(int argc, const char *argv[])
{
    run();
	return 0;
}
