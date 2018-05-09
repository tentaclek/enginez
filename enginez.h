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

#ifndef ENGINEZ_H_
#define ENGINEZ_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


/*
 * when enginez running as tcp server we need increase recv_buffer to 2000000 bytes
 * otherwise linux will block stream data in system buffer and enginez will stuck.
 */
#define TCP_RECV_BUFFER   2000000
#define UDP_RECV_BUFFER   2000000

#define RET_SUCCESS 1
#define RET_FAILURE 0
#define SEND_BUFFER   65000
#define DEFAULT_PORT  "9000"
#define LOCAL_ADDR    INADDR_ANY
#define MAX_EVENTS    10
#define MAX_THREADS   10
#define DEFAULT_BUFF_LEN SEND_BUFFER
#define DEFAULT_THREAD_NUM 1
#define VERSION "enginez V1.2 B20180410\n"

#define SERVER_TBITS_STRING "Z server rx:\033[32m%8.3lf Tbits/s %8.3lf Kpps\033[0m\b\r"
#define SERVER_GBITS_STRING "Z server rx:\033[32m%8.3lf Gbits/s %8.3lf Kpps\033[0m\b\r"
#define SERVER_MBITS_STRING "Z server rx:\033[32m%8.3lf Mbits/s %8.3lf Kpps\033[0m\b\r"
#define SERVER_KBITS_STRING "Z server rx:\033[32m%8.3lf Kbits/s %8.3lf Kpps\033[0m\b\r"
#define SERVER_BITS_STRING  "Z server rx:\033[32m%8.3lf bits/s %8.3lf pps\033[0m\b\r"

#define SERVER_BI_TBITS_STRING "Z server rx:\033[32m%8.3lf Tbits/s %8.3lf Kpps\033[0m tx:\033[33m%8.3lf Tbits/s %8.3lf Kpps\033[0m\b\r"
#define SERVER_BI_GBITS_STRING "Z server rx:\033[32m%8.3lf Gbits/s %8.3lf Kpps\033[0m tx:\033[33m%8.3lf Gbits/s %8.3lf Kpps\033[0m\b\r"
#define SERVER_BI_MBITS_STRING "Z server rx:\033[32m%8.3lf Mbits/s %8.3lf Kpps\033[0m tx:\033[33m%8.3lf Mbits/s %8.3lf Kpps\033[0m\b\r"
#define SERVER_BI_KBITS_STRING "Z server rx:\033[32m%8.3lf Kbits/s %8.3lf Kpps\033[0m tx:\033[33m%8.3lf Kbits/s %8.3lf Kpps\033[0m\b\r"
#define SERVER_BI_BITS_STRING  "Z server rx:\033[32m%8.3lf Bits/s %8.3lf pps\033[0m tx:\033[33m%8.3lf Bits/s %8.3lf pps\033[0m\b\r"

#define CLIENT_TBITS_STRING "Z client thread[%d] tx:\033[33m%8.3lf Tbits/s %8.3lf Kpps\033[0m\n"
#define CLIENT_GBITS_STRING "Z client thread[%d] tx:\033[33m%8.3lf Gbits/s %8.3lf Kpps\033[0m\n"
#define CLIENT_MBITS_STRING "Z client thread[%d] tx:\033[33m%8.3lf Mbits/s %8.3lf Kpps\033[0m\n"
#define CLIENT_KBITS_STRING "Z client thread[%d] tx:\033[33m%8.3lf Kbits/s %8.3lf Kpps\033[0m\n"
#define CLIENT_BITS_STRING  "Z client thread[%d] tx:\033[33m%8.3lf Bits/s %8.3lf pps\033[0m\n"

#define CLIENT_BI_TBITS_STRING "Z client thread[%d] rx:\033[32m%8.3lf Tbits/s %8.3lf Kpps\033[0m\n"
#define CLIENT_BI_GBITS_STRING "Z client thread[%d] rx:\033[32m%8.3lf Gbits/s %8.3lf Kpps\033[0m\n"
#define CLIENT_BI_MBITS_STRING "Z client thread[%d] rx:\033[32m%8.3lf Mbits/s %8.3lf Kpps\033[0m\n"
#define CLIENT_BI_KBITS_STRING "Z client thread[%d] rx:\033[32m%8.3lf Kbits/s %8.3lf Kpps\033[0m\n"
#define CLIENT_BI_BITS_STRING  "Z client thread[%d] rx:\033[32m%8.3lf Bits/s %8.3lf pps\033[0m\n"

#define handle_error_en(en, message) \
               do { errno = en; perror(message); exit(EXIT_FAILURE); } while (0)

enum server_enum
{
	SERVER_MOD = 1, CLIENT_MOD = 2, TCP_STREAM = 3, UDP_DGRAM = 4,
};

typedef struct str_thdata
{
	int thread_no;
	pthread_t thread_id;
	struct paras *paras_in;
} thdata;

typedef struct paras
{
	int service_mod;
	int time_perform;
	int debug;
	int quality;
	int scan;
	int bidirection;
	int buff_len;
	int protocol;
	int time_interval;
	int sockfd;
	int thread_num;
	char peer_addr[64];
	char port[8];
} s_paras;

#endif /* ENGINEZ_H_ */
