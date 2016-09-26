/**
 *   tcprstat -- Extract stats about TCP response times
 *   Copyright (C) 2010  Ignacio Nin
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
**/

#if !defined(TCPRSTAT_H)
#define TCPRSTAT_H

#include <time.h>
#include <stdio.h>

//打印格式， 默认为
/*DEFAULT_OUTPUT_FORMAT
timestamp       count   max     min     avg     med     stddev  95_max  95_avg  95_std  99_max  99_avg  99_std
1470125657      0       0       0       0       0       0       0       0       0       0       0       0
*/ //赋值给变量output_options   该字符串转换为输出在output_header   统计信息输出在output  //时延统计关键函数见见get_flush_stats  outbound
#define DEFAULT_OUTPUT_FORMAT \
    "%T\\t%n\\t%M\\t%m\\t%a\\t%h\\t%S\\t%C\\t" \
    "%95M\\t%95a\\t%95S\\t%99M\\t%99a\\t%99S\\n"
#define DEFAULT_OUTPUT_INTERVAL 10
#define MAX_OUTPUT_INTERVAL 60 * 60 * 24
#define DEFAULT_OUTPUT_ITERATIONS 1

#define DEFAULT_SHOW_HEADER 1

extern int port;
extern char *program_name;
extern time_t timestamp;
extern int g_log_fd;
extern FILE *capture_file;
extern int g_delay_time;

void terminate(int signal);

#endif
