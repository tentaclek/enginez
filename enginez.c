/*
 * Copyright (C) 2015 Robin K.
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

static inline int live_meter(int service_mod, int thread_no,
        int para_in_time_spent, const double bytes_in_out) {
    double bits_in_out = bytes_in_out * 8;
    double time_spent = (double) para_in_time_spent;

    if (bits_in_out / time_spent >= (double) 1000 * 1000 * 1000 * 1000) {
        if (service_mod == SERVER_MOD) {
            printf("enginez server running at %8.2lf Tbits/s\b\r",
                    (bits_in_out / time_spent) / ((double) 1000 * 1000 * 1000 * 1000));
        } else {
            printf("enginez client thread[%d] running at %8.2lf Tbits/s\n",
                    thread_no, (bits_in_out / time_spent) / ((double) 1000 * 1000 * 1000 * 1000));
        }
    } else if (bits_in_out / time_spent >= (double) 1000 * 1000 * 1000) {
        if (service_mod == SERVER_MOD) {
            printf("enginez server running at %8.2lf Gbits/s\b\r",
                    bits_in_out / time_spent / ((double) 1000 * 1000 * 1000));
        } else {
            printf("enginez client thread[%d] running at %8.2lf Gbits/s\n",
                    thread_no, (bits_in_out / time_spent) / ((double) 1000 * 1000 * 1000));
        }
    } else if (bits_in_out / time_spent >= 1000 * 1000) {
        if (service_mod == SERVER_MOD) {
            printf("enginez server running at %8.2lf Mbits/s\b\r",
                    bits_in_out / time_spent / (1000 * 1000));
        } else {
            printf("enginez client thread[%d] running at %8.2lf Mbits/s\n",
                    thread_no, (bits_in_out / time_spent) / ((double) 1000 * 1000));
        }
    } else if (bits_in_out / time_spent >= 1000) {
        if (service_mod == SERVER_MOD) {
            printf("enginez server running at %8.2lf Kbits/s\b\r",
                    (bits_in_out / time_spent) / ((double) 1000));
        } else {
            printf("enginez client thread[%d] running at %8.2lf Kbits/s\n",
                    thread_no, (bits_in_out / time_spent) / ((double) 1000));
        }
    } else {
        if (service_mod == SERVER_MOD) {
            printf("enginez server running at %8.2lf Bits/s\b\r",
                    bits_in_out / time_spent);
        } else {
            printf("enginez client thread[%d] running at %8.2lf Bits/s\n",
                    thread_no, bits_in_out / time_spent);
        }
    }
    fflush(stdout);
    return RET_SUCCESS;
}

static int epoll_non_blocking(int sockfd) {
    int flags;

    flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl err");
        return RET_FAILURE;
    }

    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        perror("fcntl err");
        return RET_FAILURE;
    }

    return RET_SUCCESS;
}

int dump_send_func(const void *ptr) {
    thdata *data = NULL;
    data = (thdata *) ptr;
    int sockfd = 0;
    int iret = 0;
    int protocol = 0;
    int s = 0;
    char c_buffer[data->paras_in->buff_len + 1];
    double time_perform = 0;
    double bytes_sent = 0;
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp = NULL;


    if (data->paras_in->protocol == UDP_DGRAM) {
        protocol = SOCK_DGRAM;
    } else {
        protocol = SOCK_STREAM;
    }

    memset(&hints, 0, sizeof (struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = protocol;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    s = getaddrinfo(data->paras_in->peer_addr, data->paras_in->port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) {
            continue;
        }

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            break;
        } else {
            perror("socket connect err");
        }

        close(sockfd);
        freeaddrinfo(result);
        exit(EXIT_FAILURE);
    }

    /* No address succeeded */
    if (NULL == rp) {
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }


    /* we just send data with every byte in 'c' */
    memset(c_buffer, 'c', data->paras_in->buff_len);

    printf("enginez client thread[%d] peer:%s:%s packet len:%d\n",
            data->thread_no, data->paras_in->peer_addr, data->paras_in->port, data->paras_in->buff_len);

    if (data->paras_in->protocol == UDP_DGRAM) {
        time_perform = (double) time(NULL);

        while (1) {
            iret = sendto(sockfd, c_buffer, strlen(c_buffer), 0, rp->ai_addr, rp->ai_addrlen);
            if (iret < 0) {
                perror("Sendto err");

                /* This is the result we use connect call within UDP protocol */
                if (errno == ECONNREFUSED) {
                    close(sockfd);
                    freeaddrinfo(result);
                    return RET_SUCCESS;
                }
                continue;
            }
            if (data->paras_in->debug == 1) {
                char recv_buff[32] = "\0";
                ssize_t len = 0;
                socklen_t addlen = 0;
                struct sockaddr_in6 peer_addr;
                memset((struct sockaddr_in6 *) &peer_addr, 0x00, sizeof ( struct sockaddr_in6));
                addlen = sizeof (peer_addr);

                len = recvfrom(sockfd, recv_buff, 32, MSG_DONTWAIT, (struct sockaddr *) &peer_addr, &addlen);
                if (len > 0) {
                    printf("thread[%d] recv udp ack:%s\n", data->thread_no, recv_buff);
                    fflush(stdout);
                }
                sleep(2);
                continue;
            }

            bytes_sent += iret;
            /* 
             * Actually, time(NULL) is a bad idea in this place, it's a time 
             * consuming function which can be tune better in the future 
             */
            if (data->paras_in->time_perform == ((double) time(NULL) - time_perform)) {
                live_meter(data->paras_in->service_mod, data->thread_no, data->paras_in->time_perform, bytes_sent);
                close(sockfd);
                freeaddrinfo(result);
                return RET_SUCCESS;
            }
        }
    } else {

        time_perform = (double) time(NULL);

        while (1) {
            iret = send(sockfd, c_buffer, strlen(c_buffer), 0);
            bytes_sent += iret;

            if (data->paras_in->time_perform == ((double) time(NULL) - time_perform)) {
                live_meter(data->paras_in->service_mod, data->thread_no, data->paras_in->time_perform, bytes_sent);
                close(sockfd);
                freeaddrinfo(result);
                return RET_SUCCESS;
            }

            if (iret < 0) {
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

static inline ssize_t dump_recv_func(int debug, int protocol, int sockfd,
        ssize_t *bytes_count, double *bytes_recv, double *time_start) {

    socklen_t len;

    if (TCP_STREAM == protocol) {
        *bytes_count = recv(sockfd, tcp_buffer, TCP_RECV_BUFFER, MSG_DONTWAIT);
    } else {
        *bytes_count = recvfrom(sockfd, udp_buffer,
                UDP_RECV_BUFFER, MSG_DONTWAIT, (struct sockaddr *) &peer_addr, &len);

        if (debug == 1) {

            char *pack = "hullo enginez client";
            ssize_t sent_len = 0;
            char test[64] = "\0";

            printf("recvfrom-[%s:%d]\n", inet_ntop(AF_INET6, (void *)
                    &peer_addr.sin6_addr, test, INET6_ADDRSTRLEN), ntohs(peer_addr.sin6_port));
            fflush(stdout);

            sent_len = sendto(sockfd, pack, strlen(pack) + 1,
                    0, (struct sockaddr *) &peer_addr, sizeof (struct sockaddr_in6));
            if (-1 == sent_len)
                perror("sendto err");
            sleep(1);
        }
    }

    *bytes_recv += *bytes_count;

    if (1 == ((double) time(NULL) - *time_start)) {
        live_meter(SERVER_MOD, 0, 1, *bytes_recv);
        *bytes_recv = 0;
        *time_start = (double) time(NULL);
    }

    return *bytes_count;
}

void epoll_func(void *ptr) {
    thdata *data = NULL;
    data = (thdata *) ptr;

    if (data->paras_in->service_mod == SERVER_MOD) {
        struct epoll_event ev, events[MAX_EVENTS];
        int listen_sock, conn_sock, nfds, epollfd, n, s, flags, test_fd;
        struct addrinfo hints;
        struct addrinfo *result, *rp;
        double bytes_recv = 0;
        ssize_t bytes_count = 0;
        ssize_t ret_len = 0;
        double time_start = 0;
        int protocol = 0;

        if (data->paras_in->protocol == UDP_DGRAM) {
            protocol = SOCK_DGRAM;
        } else {
            protocol = SOCK_STREAM;
        }

        memset(&hints, 0, sizeof (struct addrinfo));

        hints.ai_family = AF_INET6;
        hints.ai_socktype = protocol;
        hints.ai_flags = AI_PASSIVE;

        /* well, when OS do not support AF_INET6 we should set ai_family to AF_INET */
        test_fd = socket(hints.ai_family, hints.ai_socktype, 0);
        if (test_fd < 0) {
            if (errno == EAFNOSUPPORT) {
                printf("System do not support AF_INET6, we have to change to AF_INET\n");
                hints.ai_family = AF_INET;
            }
        } else {
            close(test_fd);
        }

        s = getaddrinfo(NULL, data->paras_in->port, &hints, &result);
        if (s != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
            exit(EXIT_FAILURE);
        }

        for (rp = result; rp != NULL; rp = rp->ai_next) {
            listen_sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (listen_sock == -1) {
                continue;
            }

            flags = 1;
            if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &flags, sizeof (flags)) < 0) {
                perror("setsockopt SO_REUSEADDR err");
                close(listen_sock);
                freeaddrinfo(result);
                exit(EXIT_FAILURE);
            }

            if (rp->ai_family == AF_INET6) {
                /* Yes, this setsockopt is the solution to create IPv4/IPv6 socket. */
                flags = 0;
                if (setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, (char *) &flags, sizeof (flags)) < 0) {
                    perror("setsockopt IPV6_V6ONLY err");
                    close(listen_sock);
                    freeaddrinfo(result);
                    exit(EXIT_FAILURE);
                }
            }

            if (bind(listen_sock, rp->ai_addr, rp->ai_addrlen) == 0) {
                break;
            } else {
                perror("Socket bind err");
            }

            close(listen_sock);
            freeaddrinfo(result);
            exit(EXIT_FAILURE);
        }

        if (rp == NULL) {
            fprintf(stderr, "Could not bind\n");
            exit(EXIT_FAILURE);
        }

        freeaddrinfo(result);

        if (data->paras_in->protocol == TCP_STREAM) {
            if (listen(listen_sock, SOMAXCONN) < 0) {
                perror("Listen err");
                close(listen_sock);
                exit(EXIT_FAILURE);
            }
            printf("enginez server running TCP protocol on port %s\n\n", data->paras_in->port);
        } else {
            printf("enginez server running UDP protocol on port %s\n\n", data->paras_in->port);
        }


        epollfd = epoll_create(10);
        if (epollfd == -1) {
            perror("epoll_create err");
            close(listen_sock);
            exit(EXIT_FAILURE);
        }

        ev.events = EPOLLIN;
        ev.data.fd = listen_sock;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
            perror("epoll_ctl: listen_sock err");
            close(listen_sock);
            exit(EXIT_FAILURE);
        }

        for (;;) {

            time_start = (double) time(NULL);

            nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
            if (nfds == -1) {
                perror("epoll_pwait err");
                close(listen_sock);
                exit(EXIT_FAILURE);
            }

            for (n = 0; n < nfds; ++n) {
                if ((events[n].events & EPOLLERR) ||
                        (events[n].events & EPOLLHUP) ||
                        (!(events[n].events & EPOLLIN))) {
                    perror("epoll err");
                    continue;
                } else if (events[n].data.fd == listen_sock) {
                    if (data->paras_in->protocol == TCP_STREAM) {
                        struct sockaddr_in6 in_addr;
                        socklen_t in_len = 0;
                        char temp_addr[64] = "\0";
                        in_len = sizeof (in_addr);

                        conn_sock = accept(listen_sock, (struct sockaddr *) &in_addr, &in_len);
                        printf("Accepted connection coming from %s:%d\n",
                                inet_ntop(AF_INET6, (void *) &in_addr.sin6_addr, temp_addr, INET6_ADDRSTRLEN),
                                ntohs(in_addr.sin6_port));

                        if (conn_sock == -1) {
                            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                                /* We have processed all incoming connections. */
                                break;
                            } else {
                                perror("accept err");
                                break;
                            }
                        }
                    }

                    if (data->paras_in->protocol == TCP_STREAM) {
                        if (epoll_non_blocking(conn_sock) == -1) {
                            close(conn_sock);
                            exit(EXIT_FAILURE);
                        }
                    } else {

                        if (epoll_non_blocking(listen_sock) == -1) {
                            close(listen_sock);
                            exit(EXIT_FAILURE);
                        }
                        dump_recv_func(data->paras_in->debug, UDP_DGRAM, events[n].data.fd,
                                &bytes_count, &bytes_recv, &time_start);

                    }

                    if (data->paras_in->protocol == TCP_STREAM) {
                        ev.events = EPOLLIN | EPOLLET;
                        ev.data.fd = conn_sock;
                        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                            perror("epoll_ctl: conn_sock err");
                            close(conn_sock);
                            exit(EXIT_FAILURE);
                        }
                    }
                } else {
                    if (data->paras_in->protocol == TCP_STREAM) {
                        ret_len = dump_recv_func(data->paras_in->debug, TCP_STREAM,
                                events[n].data.fd, &bytes_count, &bytes_recv, &time_start);
                    } else {
                        ret_len = dump_recv_func(data->paras_in->debug, UDP_DGRAM,
                                events[n].data.fd, &bytes_count, &bytes_recv, &time_start);
                    }

                    if (ret_len == -1) {
                        if (errno != EAGAIN) {
                            perror("read err");
                        }
                        break;
                    } else if (ret_len == 0) {
                        break;
                    }
                }
            }
        }
        pthread_exit(0);
    } else {
        dump_send_func(ptr);
    }
}

void re_entry_func(void *ptr) {
    thdata *data;
    data = (thdata *) ptr;

    if (data->paras_in->service_mod == CLIENT_MOD) {
        dump_send_func(ptr);
    }
    pthread_exit(0);
}

int thread_pool_create(struct paras *paras_in) {
    int i = 0;
    int n = paras_in->thread_num;
    thdata data[n];
    int s;
    pthread_attr_t attr;

    if (paras_in->service_mod == SERVER_MOD) {
        data[n].paras_in = paras_in;
        data[n].thread_no = 0;

        s = pthread_attr_init(&attr);
        if (s != 0) {
            handle_error_en(s, "pthread_attr_init");
        }

        s = pthread_create(&data[n].thread_id, NULL, (void *) &epoll_func, (void *) &data[n]);
        if (s != 0) {
            handle_error_en(s, "pthread_create");
        }

        s = pthread_attr_destroy(&attr);
        if (s != 0) {
            handle_error_en(s, "pthread_attr_destroy");
        }

        s = pthread_join(data[n].thread_id, NULL);
        if (s != 0) {
            handle_error_en(s, "pthread_join");
        }
        return RET_SUCCESS;
    }

    s = pthread_attr_init(&attr);
    if (s != 0) {
        handle_error_en(s, "pthread_attr_init");
    }

    for (; i < n; i++) {
        data[i].paras_in = paras_in;
        data[i].thread_no = i;

        s = pthread_create(&data[i].thread_id, &attr, (void *) &re_entry_func, (void *) &data[i]);
        if (s != 0) {
            handle_error_en(s, "pthread_create");
        }
    }

    s = pthread_attr_destroy(&attr);
    if (s != 0) {
        handle_error_en(s, "pthread_attr_destroy");
    }

    /* why we start a new loop for pthread_join? */
    for (i = 0; i < n; i++) {
        s = pthread_join(data[i].thread_id, NULL);
        if (s != 0) {
            handle_error_en(s, "pthread_join");
        }
    }

    return RET_SUCCESS;
}

int main(int argc, char *argv[]) {
    struct paras paras_in;
    struct sockaddr_in addr_4;
    struct sockaddr_in6 addr_6;

    memset(&addr_4, 0x0, sizeof (struct sockaddr_in));
    memset(&addr_6, 0x0, sizeof (struct sockaddr_in6));
    memset(&paras_in, 0x0, sizeof (struct paras));

    if (argc < 2) {
        show_help();
        exit(EXIT_FAILURE);
    }

    if (parse_paras(&paras_in, argc, argv) < 0) {
        show_help();
        exit(EXIT_FAILURE);
    }

    /* When enginez running as client We should check input IP address first */
    if (paras_in.service_mod == CLIENT_MOD) {
        if ((inet_pton(AF_INET6, paras_in.peer_addr, &addr_6.sin6_addr) <= 0)
                && (inet_aton(paras_in.peer_addr, &addr_4.sin_addr) == 0)) {
            printf("enginez need a proper IP address!\n");
            exit(EXIT_FAILURE);
        }
    }

    thread_pool_create(&paras_in);
    exit(EXIT_SUCCESS);
}
