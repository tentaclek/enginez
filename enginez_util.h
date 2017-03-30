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

#ifndef ENGINEZ_UTIL_H_
#define ENGINEZ_UTIL_H_

const char usage_str[] =
        "\nUsage: enginez [-s|-c host] [options]\n"
        "enginez [-h|--help]\n"
        "Service mode:Server/Client(TCP as default protocol)\n"
        " -s 	--server	run as server mode, can accept both IPv4 and IPv6 on port 9000\n"
        " -c 	--client	run as client mode\n"
        " -d    --debug         run as single packet UDP C/S mode\n"
        " -b    --bidirection   run as bidirection mode\n"
        " -U	--udp		use UDP protocol\n"
        " -T	--tcp		use TCP protocol\n"
        " -p	--port		protocol port for client connecting or server listening\n"
        " -t	--time		time for enginez client running\n"
        " -l	--len		buffer length for enginez client sending\n"
        " -i	--interval	time interval for enginez client mode display\n"
        " -P	--thread	thread numbers for enginez client mode, default is 1, max is 9\n"
        " -h	--help		display help messages\n"
        " -v	--version	display enginez version number\n"
        "\n\nEXAMPLE\n"
        "	Running TCP Server:  enginez -s -T	Running TCP Client:  enginez -c 127.0.0.1 -T\n"
        "	Running UDP Server:  enginez -s -U	Running UDP Client:  enginez -c 127.0.0.1 -U\n"
        "\n	You can use -P number set client thread number, -l number set buffer length, greater\n"
        "	length can produce more benchmark speed, especially using UDP protocol.\n"
        "\n	3 client threads is a balanced number, for 2-core 4-thread i5 CPU platform with 1400\n"
        "	bytes buffer length, client mode can reach 35Gbits per second.\n"
        "\n	Running UDP protocol in 50000 buffer length enginez can reach 45Gbits per second.\n"
        "\n	Report bugs:	arcueid.chang@yahoo.com\n";


void show_help();
int parse_paras(struct paras *paras_in, int argc, char **argv);


#endif /* ENGINEZ_UTIL_H_ */
