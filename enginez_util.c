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
#include "enginez_util.h"

void show_help()
{
	fputs(usage_str, stderr);
}

/* When enginez running as client We should check input IP address first */
int ip_addr_check(const char *ip_addr)
{
	struct sockaddr_in addr_4;
	struct sockaddr_in6 addr_6;

	memset(&addr_4, 0x0, sizeof(struct sockaddr_in));
	memset(&addr_6, 0x0, sizeof(struct sockaddr_in6));

	if ((inet_pton(AF_INET6, ip_addr, &addr_6.sin6_addr) <= 0)
			&& (inet_aton(ip_addr, &addr_4.sin_addr) == 0))
	{
		printf("enginez need a proper IP address!\n");
		exit(EXIT_FAILURE);
	}

	return RET_SUCCESS;
}

int parse_paras(struct paras *paras_in, int argc, char **argv)
{
	static struct option long_opts[] =
	{
	{ "client", required_argument, NULL, 'c' },
	{ "server", no_argument, NULL, 's' },
	{ "debug", no_argument, NULL, 'd' },
	{ "quality", no_argument, NULL, 'q' },
	{ "scan", no_argument, NULL, 'S' },
	{ "bidirection", no_argument, NULL, 'b' },
	{ "udp", no_argument, NULL, 'U' },
	{ "tcp", no_argument, NULL, 'T' },
	{ "port", required_argument, NULL, 'p' },
	{ "time", required_argument, NULL, 't' },
	{ "len", required_argument, NULL, 'l' },
	{ "interval", required_argument, NULL, 'i' },
	{ "thread", required_argument, NULL, 'P' },
	{ "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'v' },
	{ NULL, 0, NULL, 0 } };

	int c = 0;

	paras_in->thread_num = DEFAULT_THREAD_NUM;
	paras_in->buff_len = DEFAULT_BUFF_LEN;
	paras_in->time_interval = 1;
	paras_in->protocol = TCP_STREAM;
	paras_in->debug = 0;
	paras_in->bidirection = 0;
	paras_in->time_perform = 10;
	strncpy(paras_in->port, DEFAULT_PORT, 8);

	while ((c = getopt_long(argc, argv, "c:sdqSbTUp:t:l:i:P:hv", long_opts,
	NULL)) != -1)
	{
		switch (c)
		{
		case 's':
			if (paras_in->service_mod == CLIENT_MOD)
			{
				return RET_FAILURE;
			}
			paras_in->service_mod = SERVER_MOD;
			/* For now we just need 1 thread handle the test */
			paras_in->thread_num = 1;
			break;

		case 'c':
			if (paras_in->service_mod == SERVER_MOD)
			{
				return RET_FAILURE;
			}

			//if(ip_addr_check(optarg))
			{
				paras_in->service_mod = CLIENT_MOD;
				strncpy(paras_in->peer_addr, optarg, 64);
			}
			break;

		case 'd':
			paras_in->debug = 1;
			break;

		case 'q':
			paras_in->quality = 1;
			break;

		case 'S':
			paras_in->scan = 1;
			break;

		case 'b':
			paras_in->bidirection = 1;
			break;

		case 'p':
			/* Default UDP/TCP port number is 9000 */
			if (atoi(optarg) > 0 && atoi(optarg) < 65535)
			{
				strncpy(paras_in->port, optarg, 8);
			}
			break;

		case 't':
			if (atoi(optarg) > 0)
			{
				paras_in->time_perform = atoi(optarg);
			}
			break;

		case 'T':
			paras_in->protocol = TCP_STREAM;
			break;

		case 'U':
			paras_in->protocol = UDP_DGRAM;
			break;

		case 'l':
			if (atoi(optarg) > 0 && atoi(optarg) < 65535)
			{
				paras_in->buff_len = atoi(optarg);
			}
			break;

		case 'i':
			if (atoi(optarg) > 0 && atoi(optarg) < 11)
			{
				paras_in->time_interval = atoi(optarg);
			}
			break;

		case 'P':
			if (atoi(optarg) > 0 && atoi(optarg) < MAX_THREADS)
			{
				paras_in->thread_num = atoi(optarg);
			}
			else
			{
				printf("Only max 9 threads allowed, running at 1 thread\n");
			}
			break;

		case 'v':
			printf(VERSION);
			exit(EXIT_SUCCESS);

		case 'h':
		default:
			show_help();
			exit(EXIT_SUCCESS);
		}
	}

	return RET_SUCCESS;
}

int tcp_service_latency(struct paras *paras_in)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sock_fd, s;
	struct timeval s_tv, e_tv;
	char service[12] = "\0";

	/* Obtain address(es) matching host/port */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0; /* Any protocol */

	!strncmp(DEFAULT_PORT, paras_in->port, 4) ?
			strncpy(service, "80", 6) : strncpy(service, paras_in->port, 8);

	s = getaddrinfo(paras_in->peer_addr, service, &hints, &result);

	if (s != 0)
	{
		fprintf(stderr, "tcp_service_latency getaddrinfo: %s\n",
				gai_strerror(s));
		return RET_FAILURE;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sock_fd == -1)
			continue;

		/* just wait for 1 more time retry */
		int synretryies = 1;
		setsockopt(sock_fd, IPPROTO_TCP, TCP_SYNCNT, &synretryies,
				sizeof(synretryies));

		gettimeofday(&s_tv, NULL);
		if (connect(sock_fd, rp->ai_addr, rp->ai_addrlen) != -1)
		{
			gettimeofday(&e_tv, NULL);
			if (e_tv.tv_sec > s_tv.tv_sec)
			{
				if (e_tv.tv_usec > s_tv.tv_usec)
				{
					printf("service @%6s latency is about %ld.%3.0lf s\n",
							service, e_tv.tv_sec - s_tv.tv_sec,
							(double) (e_tv.tv_usec - s_tv.tv_usec) / 1000);
				}
				else
				{
					printf("service @%6s latency is about %8.3lf ms\n", service,
							(double) (1000 * (e_tv.tv_sec - s_tv.tv_sec))
									+ (double) (e_tv.tv_usec - s_tv.tv_usec)
											/ 1000);
				}
			}
			else if (e_tv.tv_sec == s_tv.tv_sec)
			{
				printf("service @%6s latency is about %8.3lf ms\n", service,
						(double) (e_tv.tv_usec - s_tv.tv_usec) / 1000);
			}
			else
			{
				printf("this SHOULD NOT happened\n");
			}
			break; /* Success */
		}
	}

	close(sock_fd);

	if (rp == NULL)
	{ /* No address succeeded */
		fprintf(stderr, "service @%s not response\n", service);
		freeaddrinfo(result); /* No longer needed */
		return RET_FAILURE;
	}

	freeaddrinfo(result); /* No longer needed */
	return RET_SUCCESS;
}

int tcp_scan(struct paras *paras_in)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	char service[12] = "\0";
	int i = 0;
	int j = 1;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0; /* Any protocol */

	for (i = 0; i < 65536; i++)
	{
		memset(service, 0x0, 12);
		sprintf(service, "%d", j);
		j++;

		s = getaddrinfo(paras_in->peer_addr, service, &hints, &result);

		if (s != 0)
		{
			fprintf(stderr, "tcp scan getaddrinfo: %s\n", gai_strerror(s));
			exit(EXIT_FAILURE);
		}

		for (rp = result; rp != NULL; rp = rp->ai_next)
		{
			sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (sfd == -1)
			{
				freeaddrinfo(result);
				continue;
			}
			/*
			 * test for TCP_USER_TIMEOUT opt
			 */
			int user_timeout = 1;
			setsockopt(sfd, IPPROTO_TCP, TCP_USER_TIMEOUT, &user_timeout,
					sizeof(user_timeout));

			int synretryies = 1;
			setsockopt(sfd, IPPROTO_TCP, TCP_SYNCNT, &synretryies,
					sizeof(synretryies));
			/*
			 * tcp timeout and retries combination sockopt
			 */

			if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			{
				printf("\033[32mservice @%6s OK\n", service);
				//break; /* Success */
			}
			close(sfd);
		}

		freeaddrinfo(result);
	}

	return RET_SUCCESS;
}
