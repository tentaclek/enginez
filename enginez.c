/*
 * Copyright (C) 2015-2016 Robin K.
 *
 * Engine Zero, enginez would just be a tcp/udp benchmark tool
 * which support basic epoll implement and running as a multi-thread mode
 *
 * enginez is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * enginez is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "enginez.h"

extern void show_help();
extern int parse_paras(struct paras *paras_in, int argc, char **argv);
extern int tcp_service_latency(struct paras *paras_in);
extern int tcp_scan(struct paras *paras_in);

static inline int live_meter(int service_mod, int bi, int thread_no,
		int para_in_time_spent, const double bytes_in_out,
		const double pack_count)
{
	double bits_in_out = bytes_in_out * 8;
	double time_spent = (double) para_in_time_spent;

	if (bits_in_out / time_spent >= (double) 1000 * 1000 * 1000)
	{
		if (service_mod == SERVER_MOD)
		{
			if (1 == bi)
			{
				printf(SERVER_BI_GBITS_STRING,
						bits_in_out / time_spent
								/ ((double) 1000 * 1000 * 1000),
						pack_count / (1000),
						bits_in_out / time_spent
								/ ((double) 1000 * 1000 * 1000),
						pack_count / (1000));
			}
			else
			{
				printf(SERVER_GBITS_STRING,
						bits_in_out / time_spent
								/ ((double) 1000 * 1000 * 1000),
						pack_count / (1000));
			}
		}
		else
		{
			if (1 == bi)
			{
				printf(CLIENT_BI_GBITS_STRING, thread_no,
						(bits_in_out / time_spent)
								/ ((double) 1000 * 1000 * 1000),
						pack_count / time_spent / (1000));
			}
			else
			{
				printf(CLIENT_GBITS_STRING, thread_no,
						(bits_in_out / time_spent)
								/ ((double) 1000 * 1000 * 1000),
						pack_count / time_spent / (1000));

			}
		}
	}
	else if (bits_in_out / time_spent >= 1000 * 1000)
	{
		if (service_mod == SERVER_MOD)
		{
			if (1 == bi)
			{
				printf(SERVER_BI_MBITS_STRING,
						bits_in_out / time_spent / (1000 * 1000),
						pack_count / (1000),
						bits_in_out / time_spent / (1000 * 1000),
						pack_count / (1000));

			}
			else
			{
				printf(SERVER_MBITS_STRING,
						bits_in_out / time_spent / (1000 * 1000),
						pack_count / (1000));
			}
		}
		else
		{
			if (1 == bi)
			{
				printf(CLIENT_BI_MBITS_STRING, thread_no,
						(bits_in_out / time_spent) / ((double) 1000 * 1000),
						pack_count / time_spent / (1000));
			}
			else
			{
				printf(CLIENT_MBITS_STRING, thread_no,
						(bits_in_out / time_spent) / ((double) 1000 * 1000),
						pack_count / time_spent / (1000));
			}
		}
	}
	else if (bits_in_out / time_spent >= 1000)
	{
		if (service_mod == SERVER_MOD)
		{
			if (1 == bi)
			{
				printf(SERVER_BI_KBITS_STRING,
						(bits_in_out / time_spent) / ((double) 1000),
						pack_count / 1000,
						(bits_in_out / time_spent) / ((double) 1000),
						pack_count / 1000);

			}
			else
			{
				printf(SERVER_KBITS_STRING,
						(bits_in_out / time_spent) / ((double) 1000),
						pack_count / 1000);
			}
		}
		else
		{
			if (1 == bi)
			{
				printf(CLIENT_BI_KBITS_STRING, thread_no,
						(bits_in_out / time_spent) / ((double) 1000),
						pack_count / time_spent / 1000);
			}
			else
			{
				printf(CLIENT_KBITS_STRING, thread_no,
						(bits_in_out / time_spent) / ((double) 1000),
						pack_count / time_spent / 1000);
			}
		}
	}
	else if (bits_in_out / time_spent >= (double) 1000 * 1000 * 1000 * 1000)
	{
		if (service_mod == SERVER_MOD)
		{
			if (1 == bi)
			{
				printf(SERVER_BI_TBITS_STRING,
						(bits_in_out / time_spent)
								/ ((double) 1000 * 1000 * 1000 * 1000),
						pack_count / (1000),
						(bits_in_out / time_spent)
								/ ((double) 1000 * 1000 * 1000 * 1000),
						pack_count / (1000));

			}
			else
			{
				printf(SERVER_TBITS_STRING,
						(bits_in_out / time_spent)
								/ ((double) 1000 * 1000 * 1000 * 1000),
						pack_count / (1000));
			}
		}
		else
		{
			if (1 == bi)
			{
				printf(CLIENT_BI_TBITS_STRING, thread_no,
						(bits_in_out / time_spent)
								/ ((double) 1000 * 1000 * 1000 * 1000),
						pack_count / time_spent / (1000));
			}
			else
			{
				printf(CLIENT_TBITS_STRING, thread_no,
						(bits_in_out / time_spent)
								/ ((double) 1000 * 1000 * 1000 * 1000),
						pack_count / time_spent / (1000));
			}
		}
	}
	else
	{
		if (service_mod == SERVER_MOD)
		{
			if (1 == bi)
			{
				printf(SERVER_BI_BITS_STRING, bits_in_out / time_spent,
						pack_count, bits_in_out / time_spent, pack_count);
			}
			else
			{
				printf(SERVER_BITS_STRING, bits_in_out / time_spent,
						pack_count);
			}
		}
		else
		{
			if (1 == bi)
			{
				printf(CLIENT_BI_BITS_STRING, thread_no,
						bits_in_out / time_spent, pack_count / time_spent);
			}
			else
			{
				printf(CLIENT_BITS_STRING, thread_no, bits_in_out / time_spent,
						pack_count / time_spent);
			}
		}
	}
	fflush(stdout);
	return RET_SUCCESS;
}

static int epoll_non_blocking(int sockfd)
{
	int flags;

	flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl err");
		return RET_FAILURE;
	}

	flags |= O_NONBLOCK;
	if (fcntl(sockfd, F_SETFL, flags) == -1)
	{
		perror("fcntl err");
		return RET_FAILURE;
	}

	return RET_SUCCESS;
}

void bi_recv(void *ptr)
{
	thdata *data = NULL;
	data = (thdata *) ptr;
	char recv_buff[UDP_RECV_BUFFER] = "\0";
	ssize_t len = 0;
	socklen_t addlen = 0;
	double time_start = 0;
	double bytes_sent = 0;
	double pack_count = 0;
	struct sockaddr_in6 peer_addr;
	memset((struct sockaddr_in6 *) &peer_addr, 0x00,
			sizeof(struct sockaddr_in6));
	addlen = sizeof(peer_addr);

	time_start = (double) time(NULL);

	while (1)
	{

		len = recvfrom(data->paras_in->sockfd, recv_buff, UDP_RECV_BUFFER, 0,
				(struct sockaddr *) &peer_addr, &addlen);
		bytes_sent += len;
		pack_count++;

		if (data->paras_in->time_interval == ((double) time(NULL) - time_start))
		{
			live_meter(data->paras_in->service_mod, 1, data->thread_no,
					data->paras_in->time_interval, bytes_sent, pack_count);
			pack_count = 0;
			bytes_sent = 0;
			time_start = (double) time(NULL);

		}

	}
}

int dump_send_func(void *ptr)
{
	thdata *data = NULL;
	data = (thdata *) ptr;
	int sockfd = 0;
	pthread_attr_t attr;
	pthread_t thread_id;
	int s = 0;
	int iret = 0;
	char c_buffer[data->paras_in->buff_len + 1];
	double time_perform = 0;
	double time_start = 0;
	double bytes_sent = 0;
	double pack_count = 0;
	struct addrinfo hints;
	struct addrinfo *result = NULL;
	struct addrinfo *rp = NULL;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if (data->paras_in->protocol == UDP_DGRAM)
	{
		hints.ai_socktype = SOCK_DGRAM;
	}
	else
	{
		hints.ai_socktype = SOCK_STREAM;
	}

	s = getaddrinfo(data->paras_in->peer_addr, data->paras_in->port, &hints,
			&result);
	if (s != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1)
		{
			continue;
		}

		if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
		{
			break;
		}
		else
		{
			perror("socket connect err");
		}

		close(sockfd);
		freeaddrinfo(result);
		exit(EXIT_FAILURE);
	}

	/* No address succeeded */
	if (NULL == rp)
	{
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}

	data->paras_in->sockfd = sockfd;

	if (1 == data->paras_in->bidirection)
	{

		s = pthread_attr_init(&attr);
		if (s != 0)
		{
			handle_error_en(s, "pthread_attr_init");
		}

		s = pthread_create(&thread_id, NULL, (void *) &bi_recv, (void *) data);
		if (s != 0)
		{
			handle_error_en(s, "pthread_create");
		}

		s = pthread_attr_destroy(&attr);
		if (s != 0)
		{
			handle_error_en(s, "pthread_attr_destroy");
		}
	}

	/* we just send data with every byte in 'c' */
	memset(c_buffer, 'c', data->paras_in->buff_len);

	printf("Z client thread[%d] peer:%s:%s packet len:%d\n", data->thread_no,
			data->paras_in->peer_addr, data->paras_in->port,
			data->paras_in->buff_len);

	time_start = (double) time(NULL);
	time_perform = (double) time(NULL);
	if (data->paras_in->protocol == UDP_DGRAM)
	{

		while (1)
		{
			iret = sendto(sockfd, c_buffer, strlen(c_buffer), 0, rp->ai_addr,
					rp->ai_addrlen);
			if (iret < 0)
			{
				perror("Sendto err");

				/* This is the result we use connect call within UDP protocol */
				if (errno == ECONNREFUSED)
				{
					close(sockfd);
					freeaddrinfo(result);
					return RET_FAILURE;
				}
				continue;
			}

			if (data->paras_in->debug == 1)
			{
				char recv_buff[32] = "\0";
				ssize_t len = 0;
				socklen_t addlen = 0;
				struct sockaddr_in6 peer_addr;
				memset((struct sockaddr_in6 *) &peer_addr, 0x00,
						sizeof(struct sockaddr_in6));
				addlen = sizeof(peer_addr);

				len = recvfrom(sockfd, recv_buff, 32, MSG_DONTWAIT,
						(struct sockaddr *) &peer_addr, &addlen);
				if (len > 0)
				{
					printf("thread[%d] recv udp ack:%s\n", data->thread_no,
							recv_buff);
					fflush(stdout);
				}
				sleep(2);
				continue;
			}

			bytes_sent += iret;
			pack_count++;
			/*
			 * Actually, time(NULL) is a bad idea in this place, it's
			 * a time consuming function which can be tune better
			 */
			if (data->paras_in->time_interval
					== ((double) time(NULL) - time_start))
			{
				live_meter(data->paras_in->service_mod, 0, data->thread_no,
						data->paras_in->time_interval, bytes_sent, pack_count);
				pack_count = 0;
				bytes_sent = 0;
				time_start = (double) time(NULL);

			}
			if (data->paras_in->time_perform
					== ((double) time(NULL) - time_perform))
			{
				close(sockfd);
				freeaddrinfo(result);
				return RET_FAILURE;
			}
		}
	}
	else
	{

		while (1)
		{
			iret = send(sockfd, c_buffer, strlen(c_buffer), 0);
			bytes_sent += iret;
			pack_count++;

			if (data->paras_in->time_interval
					== ((double) time(NULL) - time_start))
			{
				live_meter(data->paras_in->service_mod, 0, data->thread_no,
						data->paras_in->time_interval, bytes_sent, pack_count);
				pack_count = 0;
				bytes_sent = 0;
				time_start = (double) time(NULL);
			}

			if (data->paras_in->time_perform
					== ((double) time(NULL) - time_perform))
			{
				close(sockfd);
				freeaddrinfo(result);
				return RET_FAILURE;
			}

			if (iret < 0)
			{
				perror("Send err");
				continue;
			}
		}
	}

	return RET_SUCCESS;
}

char tcp_buffer[TCP_RECV_BUFFER + 1] = "\0";
char udp_buffer[UDP_RECV_BUFFER + 1] = "\0";
struct sockaddr_in6 peer_addr;
socklen_t sock_len;

static inline ssize_t dump_recv_func(int bi, int debug, int protocol,
		int sockfd, ssize_t *bytes_count, double *bytes_recv,
		double *time_start, double *pack_count)
{

	if (TCP_STREAM == protocol)
	{
		*bytes_count = recv(sockfd, tcp_buffer, TCP_RECV_BUFFER, MSG_DONTWAIT);
	}
	else
	{
		*bytes_count = recvfrom(sockfd, udp_buffer,
		UDP_RECV_BUFFER, MSG_DONTWAIT, (struct sockaddr *) &peer_addr,
				&sock_len);

		if (1 == bi)
		{

			if (0
					> sendto(sockfd, udp_buffer, *bytes_count, 0,
							(struct sockaddr *) &peer_addr,
							sizeof(struct sockaddr_in6)))

				perror("sendto err");
		}

		if (debug == 1)
		{

			char *pack = "hullo Z client";
			ssize_t sent_len = 0;
			char test[64] = "\0";

			printf("recvfrom-[%s:%hu]\n",
					inet_ntop(AF_INET6, (void *) &peer_addr.sin6_addr, test,
					INET6_ADDRSTRLEN), ntohs(peer_addr.sin6_port));
			fflush(stdout);

			sent_len = sendto(sockfd, pack, strlen(pack) + 1, 0,
					(struct sockaddr *) &peer_addr,
					sizeof(struct sockaddr_in6));
			if (-1 == sent_len)
				perror("sendto err");
			sleep(1);
		}
	}

	*bytes_recv += *bytes_count;

	if (1.0 == ((double) time(NULL) - *time_start))
	{

		live_meter(SERVER_MOD, bi, 0, 1, *bytes_recv, *pack_count);
		*bytes_recv = 0;
		*pack_count = 0;
		*time_start = (double) time(NULL);
	}

	return *bytes_count;
}

void epoll_func(void *ptr)
{
	thdata *data = NULL;
	data = (thdata *) ptr;

	if (data->paras_in->service_mod == SERVER_MOD)
	{
		struct epoll_event ev, events[MAX_EVENTS];
		int listen_sock, nfds, epollfd, n, s, flags, test_fd;
		int conn_sock = 0;
		struct addrinfo hints;
		struct addrinfo *result, *rp;
		double bytes_recv = 0;
		ssize_t bytes_count = 0;
		ssize_t ret_len = 0;
		double time_start = 0;
		double pack_count = 0;

		memset(&hints, 0, sizeof(struct addrinfo));

		if (data->paras_in->protocol == UDP_DGRAM)
		{
			hints.ai_socktype = SOCK_DGRAM;
		}
		else
		{
			hints.ai_socktype = SOCK_STREAM;
		}

		hints.ai_family = AF_INET6;
		hints.ai_flags = AI_PASSIVE;

		/* well, when OS do not support AF_INET6 we should set ai_family to AF_INET */
		test_fd = socket(hints.ai_family, hints.ai_socktype, 0);
		if (test_fd < 0)
		{
			if (errno == EAFNOSUPPORT)
			{
				printf(
						"System do not support AF_INET6, we have to change to AF_INET\n");
				hints.ai_family = AF_INET;
			}
		}
		else
		{
			close(test_fd);
		}

		s = getaddrinfo(NULL, data->paras_in->port, &hints, &result);
		if (s != 0)
		{
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
			exit(EXIT_FAILURE);
		}

		for (rp = result; rp != NULL; rp = rp->ai_next)
		{
			listen_sock = socket(rp->ai_family, rp->ai_socktype,
					rp->ai_protocol);
			if (listen_sock == -1)
			{
				continue;
			}

			flags = 1;
			if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
					(char *) &flags, sizeof(flags)) < 0)
			{
				close(listen_sock);
				freeaddrinfo(result);
				handle_error_en(0 , "setsockopt SO_REUSEADDR err");
			}

			if (rp->ai_family == AF_INET6)
			{
				/* Yes, this setsockopt is the solution to create IPv4/IPv6 socket. */
				flags = 0;
				if (setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY,
						(char *) &flags, sizeof(flags)) < 0)
				{
					close(listen_sock);
					freeaddrinfo(result);
					handle_error_en(0, "setsockopt IPV6_V6ONLY err");
				}
			}

			if (bind(listen_sock, rp->ai_addr, rp->ai_addrlen) == 0)
			{
				break;
			}
			else
			{
				perror("Socket bind err");
			}

			close(listen_sock);
			freeaddrinfo(result);
			exit(EXIT_FAILURE);
		}

		if (rp == NULL)
		{
			handle_error_en(0, "Could not bind");
		}

		freeaddrinfo(result);

		if (data->paras_in->protocol == TCP_STREAM)
		{
			if (listen(listen_sock, SOMAXCONN) < 0)
			{
				close(listen_sock);
				handle_error_en(0, "listen error");
			}
			printf("Z server running TCP protocol on port %s\n\n",
					data->paras_in->port);
		}
		else
		{
			printf("Z server running UDP protocol on port %s\n\n",
					data->paras_in->port);
		}

		epollfd = epoll_create(10);
		if (epollfd == -1)
		{
			close(listen_sock);
			handle_error_en(epollfd, "epoll_create err");
		}

		ev.events = EPOLLIN;
		ev.data.fd = listen_sock;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
		{
			close(listen_sock);
			handle_error_en(0, "epoll_ctl: listen_sock err");
		}

		for (;;)
		{

			time_start = (double) time(NULL);

			nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
			if (nfds == -1)
			{
				close(listen_sock);
				handle_error_en(0, "epoll_pwait err");
			}

			for (n = 0; n < nfds; ++n)
			{
				if ((events[n].events & EPOLLERR)
						|| (events[n].events & EPOLLHUP)
						|| (!(events[n].events & EPOLLIN)))
				{
					perror("epoll err");
					continue;
				}
				else if (events[n].data.fd == listen_sock)
				{
					if (data->paras_in->protocol == TCP_STREAM)
					{
						struct sockaddr_in6 in_addr;
						socklen_t in_len = 0;
						char temp_addr[64] = "\0";
						in_len = sizeof(in_addr);

						conn_sock = accept(listen_sock,
								(struct sockaddr *) &in_addr, &in_len);
						printf("Accepted connection coming from %s:%hu\n",
								inet_ntop(AF_INET6, (void *) &in_addr.sin6_addr,
										temp_addr, INET6_ADDRSTRLEN),
								ntohs(in_addr.sin6_port));

						if (conn_sock == -1)
						{
							if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
							{
								/* We have processed all incoming connections. */
								break;
							}
							else
							{
								perror("accept err");
								break;
							}
						}

						if (epoll_non_blocking(conn_sock) == -1)
						{
							close(conn_sock);
							exit(EXIT_FAILURE);
						}
					}
					else
					{

						if (epoll_non_blocking(listen_sock) == -1)
						{
							close(listen_sock);
							handle_error_en(0, "epoll set error");
						}
						dump_recv_func(data->paras_in->bidirection,
								data->paras_in->debug, UDP_DGRAM,
								events[n].data.fd, &bytes_count, &bytes_recv,
								&time_start, &pack_count);
						pack_count++;

					}

					if (data->paras_in->protocol == TCP_STREAM)
					{
						ev.events = EPOLLIN | EPOLLET;
						ev.data.fd = conn_sock;
						if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev)
								== -1)
						{
							close(conn_sock);
							handle_error_en(0, "epoll_ctl: conn_sock err");
						}
					}
				}
				else
				{
					if (data->paras_in->protocol == TCP_STREAM)
					{
						ret_len = dump_recv_func(data->paras_in->bidirection,
								data->paras_in->debug, TCP_STREAM,
								events[n].data.fd, &bytes_count, &bytes_recv,
								&time_start, &pack_count);
						pack_count++;
					}
					else
					{
						ret_len = dump_recv_func(data->paras_in->bidirection,
								data->paras_in->debug, UDP_DGRAM,
								events[n].data.fd, &bytes_count, &bytes_recv,
								&time_start, &pack_count);
						pack_count++;
					}

					if (ret_len == -1)
					{
						if (errno != EAGAIN)
						{
							perror("read err");
						}
						break;
					}
					else if (ret_len == 0)
					{
						break;
					}
				}
			}
		}
	}
	else
	{
		dump_send_func(ptr);
	}
	pthread_exit(0);
}

int thread_pool_create(struct paras *paras_in)
{
	int i = 0;
	int n = paras_in->thread_num;
	thdata data[n];
	int s = 0;
	pthread_attr_t attr;

	s = pthread_attr_init(&attr);
	if (s != 0)
	{
		handle_error_en(s, "pthread_attr_init");
	}

	for (; i < n; i++)
	{
		data[i].paras_in = paras_in;
		data[i].thread_no = i;

		s = pthread_create(&data[i].thread_id, &attr, (void *) &epoll_func,
				(void *) &data[i]);
		if (s != 0)
		{
			handle_error_en(s, "pthread_create");
		}
	}

	s = pthread_attr_destroy(&attr);
	if (s != 0)
	{
		handle_error_en(s, "pthread_attr_destroy");
	}

	/* why we start a new loop for pthread_join? */
	for (i = 0; i < n; i++)
	{
		s = pthread_join(data[i].thread_id, NULL);
		if (s != 0)
		{

			handle_error_en(s, "pthread_join");
		}
	}

	return RET_SUCCESS;
}

int main(int argc, char *argv[])
{
	struct paras paras_in;
	memset(&paras_in, 0x0, sizeof(struct paras));

	if (argc < 2)
	{
		printf("%s",VERSION);
		show_help();
		exit(EXIT_FAILURE);
	}

	if (!parse_paras(&paras_in, argc, argv))
	{
		printf("\nunsupported option\n");
		show_help();
		exit(EXIT_FAILURE);
	}

	if (paras_in.quality == 1)
	{
		if (tcp_service_latency(&paras_in))
		{
			exit(EXIT_SUCCESS);
		}
		else
		{
			exit(EXIT_FAILURE);
		}
	}

	if (paras_in.scan == 1)
	{
		tcp_scan(&paras_in);
		exit(EXIT_SUCCESS);
	}

	if (paras_in.protocol == TCP_STREAM)
	{
		if (paras_in.bidirection == 1)
		{
			handle_error_en(0, "bidirection mode only for UDP protocol!");
		}

		if (paras_in.debug == 1)
		{
			handle_error_en(0, "debug mode only for UDP protocol!");
		}
	}

	if ((paras_in.service_mod == CLIENT_MOD) && (paras_in.buff_len > 1472))
	{
		printf("\nIP packet bigger than MTU, which "
				"cause pps figure will not equal to wire!\n\n");
	}

	thread_pool_create(&paras_in);
	exit(EXIT_SUCCESS);
}
