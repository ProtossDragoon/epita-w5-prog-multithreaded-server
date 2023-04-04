#define _GNU_SOURCE

#include <err.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "shared_queue.h"

// Number of threads.
const size_t THREAD_COUNT = 4;

// Buffer size in bytes.
const size_t BUFFER_SIZE = 256;

void rewrite(int fd, const void *buf, size_t count)
{
	int left = count;
	const void *p_buf = buf;
	do {
		int n_written = write(fd, p_buf, count);
		if (n_written == -1) {
			errx(-1, "write()");
		}
		p_buf += n_written;
		left -= n_written;
	} while (left > 0);
}

void echo(int fd_in, int fd_out)
{
	char buf[BUFFER_SIZE];
	int n_read = 1;
	do {
		n_read = read(fd_in, buf, BUFFER_SIZE);
		if (n_read == -1) {
			errx(-1, "read()");
		}
		rewrite(fd_out, buf, n_read);
	} while (n_read > 0);
}

void *worker(void *arg)
{
	while (1) {
		shared_queue *queue = arg;
		int accepted_sfd = shared_queue_pop(queue);
		echo(accepted_sfd, accepted_sfd);
		close(accepted_sfd);
	}
	return NULL;
}

int main()
{
	shared_queue *queue = shared_queue_new();

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *result = NULL;
	int err = getaddrinfo(NULL, "2048", &hints, &result);
	if (err != 0) {
		errx(err, "getaddrinfo(): %s", gai_strerror(err));
	}

	int sfd = -1;
	for (struct addrinfo *p = result; p != NULL; p = p->ai_next) {
		sfd = socket(
			p->ai_family,
			p->ai_socktype,
			p->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		int opt_val = 1;
		err = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
		if (err != 0) {
			errx(err, "setsockopt()");
		}
		if (bind(sfd, p->ai_addr, p->ai_addrlen) == 0) {
			break;
		}
		close(sfd);
	}
	freeaddrinfo(result);
	if (sfd == -1) {
		errx(sfd, "socket(), bind()");
	}

	err = listen(sfd, 5);
	if (err != 0) {
		errx(-1, "listen()");
	}

	pthread_t thread[THREAD_COUNT];
	for (size_t i = 0; i < THREAD_COUNT; i++) {
		err = pthread_create(&(thread[i]), NULL, worker, queue);
		if (err != 0) {
			errx(err, "pthread_create()");
		}
	}
	while (1) {
		int accepted_sfd = accept(sfd, NULL, NULL);
		if (accepted_sfd == -1) {
			// When accept() recieves SIGINT,
			// every thread & server should be terminated.
			shared_queue_destroy(queue);
			close(sfd);
			// When parent process is terminated,
			// every threads are automatically terminated.
			errx(accepted_sfd, "accept()");
		}
		shared_queue_push(queue, accepted_sfd);
	}
	for (size_t i = 0; i < THREAD_COUNT; i++) {
		err = pthread_join(thread[i], NULL);
		if (err != 0) {
			errx(err, "pthread_join()");
		}
	}
	shared_queue_destroy(queue);
	close(sfd);
	return 0;
}
