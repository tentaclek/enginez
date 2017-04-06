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

void show_help() {
    fputs(usage_str, stderr);
}

int parse_paras(struct paras *paras_in, int argc, char **argv) {
    static struct option long_opts[] = {
        {"client", required_argument, NULL, 'c'},
        {"server", no_argument, NULL, 's'},
        {"debug", no_argument, NULL, 'd'},
        {"bidirection", no_argument, NULL, 'b'},
        {"udp", no_argument, NULL, 'U'},
        {"tcp", no_argument, NULL, 'T'},
        {"port", required_argument, NULL, 'p'},
        {"time", required_argument, NULL, 't'},
        {"len", required_argument, NULL, 'l'},
        {"interval", required_argument, NULL, 'i'},
        {"thread", required_argument, NULL, 'P'},
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0}
    };

    int c = 0;

    paras_in->thread_num = DEFAULT_THREAD_NUM;
    paras_in->buff_len = DEFAULT_BUFF_LEN;
    paras_in->time_interval = 1;
    paras_in->protocol = TCP_STREAM;
    paras_in->debug = 0;
    paras_in->bidirection = 0;
    paras_in->time_perform = 10;
    strncpy(paras_in->port, DEFAULT_PORT, 8);

    while ((c = getopt_long(argc, argv, "c:sdbTUp:t:l:i:P:hv", long_opts, NULL)) != -1) {
        switch (c) {
            case 's':
                if (paras_in->service_mod == CLIENT_MOD) {
                    return RET_FAILURE;
                }
                paras_in->service_mod = SERVER_MOD;
                /* For now we just need 1 thread handle the test */
                paras_in->thread_num = 1;
                break;

            case 'c':
                if (paras_in->service_mod == SERVER_MOD) {
                    return RET_FAILURE;
                }

                paras_in->service_mod = CLIENT_MOD;
                strncpy(paras_in->peer_addr, optarg, 64);
                break;

            case 'd':
                paras_in->debug = 1;
                break;

            case 'b':
                paras_in->bidirection = 1;
                break;

            case 'p':
                /* Default UDP/TCP port number is 9000 */
                if (atoi(optarg) > 0 && atoi(optarg) < 65535) {
                    strncpy(paras_in->port, optarg, 8);
                }
                break;

            case 't':
                if (atoi(optarg) > 0) {
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
                if (atoi(optarg) > 0 && atoi(optarg) < 65535) {
                    paras_in->buff_len = atoi(optarg);
                }
                break;

            case 'i':
                if (atoi(optarg) > 0 && atoi(optarg) < 11) {
                    paras_in->time_interval = atoi(optarg);
                }
                break;

            case 'P':
                if (atoi(optarg) > 0 && atoi(optarg) < MAX_THREADS) {
                    paras_in->thread_num = atoi(optarg);
                } else {
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

