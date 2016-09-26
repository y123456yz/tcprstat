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
 
#include "output.h"
#include "stats.h"
#include "capture.h"
#include "tcprstat.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static int output(time_t current, char format[], unsigned long iterations),
    output_header(char header[], int verbatim);

// Last output timeval for offline captures
struct timeval last_output;

// Options copy for the same thing
static struct output_options *output_options;

// Iterations
unsigned long iterations;

void *
output_thread(void *arg) {
    struct output_options *options;
    struct timespec ts;
    time_t current;
    unsigned long iterations;
    
    options = arg;
    
    ts = (struct timespec) { options->interval, 0 };
    
    if (!check_format(options->format))
        abort();
    
    if (options->show_header) {
        if (options->header)
            output_header(options->header, 1);
        else
            output_header(options->format, 0);
        
    }
    
    for (iterations = 0; !options->iterations || iterations < options->iterations;
            iterations ++)
    {
        nanosleep(&ts, NULL);
        
        time(&current);
        output(current, options->format, iterations);
        
    }
    
    // Iterations finished, signal capturing process
    endcapture();
    
    return NULL;
    
}

int
output_offline_start(struct output_options *options) {
    // TODO: Make common code with output_offline_start()
    if (!check_format(options->format))
        abort();
    
    if (options->show_header) {
        if (options->header)
            output_header(options->header, 1);
        else
            output_header(options->format, 0);
        
    }
    
    output_options = options;

    iterations = 0;
    
    return 0;
    
}

int
output_offline_update(struct timeval tv) {
    struct timeval next;
    
    // Set last output if it's at zero
    if (!last_output.tv_sec)
        last_output = tv;
    
    do {
        next.tv_sec = last_output.tv_sec + output_options->interval;
        next.tv_usec = last_output.tv_usec;
/*        printf("Last output was %lu:%lu, next %lu:%lu. Packet is %lu:%lu\n",
               last_output.tv_sec, last_output.tv_usec, next.tv_sec, next.tv_usec,
               tv.tv_sec, tv.tv_usec);*/
               
    
        if (tv.tv_sec > next.tv_sec ||
                (tv.tv_sec == next.tv_sec && tv.tv_usec > next.tv_usec))
        {
            output(next.tv_sec, output_options->format, iterations);
            last_output = next;
            
            iterations ++;
            
        }
        else
            break;
        
    }
    while (1);
    
    return 0;
        
}

//打印格式， 默认为
/*
#define DEFAULT_OUTPUT_FORMAT \
    "%T\\t%n\\t%M\\t%m\\t%a\\t%h\\t%S\\t" \
    "%95M\\t%95a\\t%95S\\t%99M\\t%99a\\t%99S\\n"
    T             n      M       m       a       h        S       95M     95a     95S     99M     99a     99S
timestamp       count   max     min     avg     med     stddev  95_max  95_avg  95_std  99_max  99_avg  99_std
1470125657      0       0       0       0       0       0       0       0       0       0       0       0
*/ //赋值给变量output_options   该字符串转换为输出在output_header   //时延统计关键函数见见get_flush_stats  outbound
static int
output(time_t current, char format[], unsigned long iterations) {
    char *c;
    
    struct stats_results *results;
    
    results = get_flush_stats(); 
    
    for (c = format; c[0]; c ++)
        if (c[0] == '%') {
            int r = 100;
            c ++;
            
            if (c[0] >= '0' && c[0] <= '9') {
                r = 0;
                while (c[0] >= '0' && c[0] <= '9') { //%95转换为数字95存入r,如果没有%95这些数字，则r=100
                    r *= 10;
                    r += c[0] - '0';
                    
                    c ++;
                    
                }
                
            }

/*
    T             n      M       m       a       h        S       95M     95a     95S     99M     99a     99S
timestamp       count   max     min     avg     med     stddev  95_max  95_avg  95_std  99_max  99_avg  99_std
*/
            if (c[0] == 'n')
                printf("%u", stats_count(results, r)); //包数
            else if (c[0] == 'a') //%r部分的平均时延
                printf("%lu", stats_avg(results, r)); //总的平均时延
            else if (c[0] == 's') 
                printf("%lu", stats_sum(results, r)); //
            else if (c[0] == 'x')
                printf("%lu", stats_sqs(results, r));
            else if (c[0] == 'm')
                printf("%lu", stats_min(results, r)); //最小时延
            else if (c[0] == 'M') //M就是最大时延 95M就是数组中第95%号成员的时延
                printf("%lu", stats_max(results, r)); //时间排序数组中第%r大的时延，例如数组为100，则这里为数组第95号成员的值
            else if (c[0] == 'h') //数组中前面%r部分，最中间元素的时延
                printf("%lu", stats_med(results, r)); 
            else if (c[0] == 'S')
                printf("%lu", stats_std(results, r));
            else if (c[0] == 'v')
                printf("%lu", stats_var(results, r));
            else if (c[0] == 'C')
                printf("%lu", stats_delay_count(results, r));
            
            // Timestamping
            else if (c[0] == 'I')
                printf("%lu", iterations);
            else if (c[0] == 't' || c[0] == 'T')
                printf("%lu", current - (c[0] == 't' ? timestamp : 0));
            
            // Actual %
            else if (c[0] == '%')
                fputc(c[0], stdout);
            
        }
        else if (c[0] == '\\')
            if (c[1] == 'n') {
                c ++;
                fputc('\n', stdout);
            }
            else if (c[1] == 't') {
                c ++;
                fputc('\t', stdout);
            }
            else if (c[1] == 'r') {
                c ++;
                fputc('\r', stdout);
            }
            else if (c[1] == '\\') {
                c ++;
                fputc('\\', stdout);
            }
            else
                fputc('\\', stdout);
            
        else
            fputc(c[0], stdout);
        
    fflush(stdout);
    
    free_results(results);
    
    return 0;

}

//打印格式， 默认为
/*DEFAULT_OUTPUT_FORMAT
timestamp       count   max     min     avg     med     stddev  95_max  95_avg  95_std  99_max  99_avg  99_std
1470125657      0       0       0       0       0       0       0       0       0       0       0       0
*/ //赋值给变量output_options   该字符串转换为输出在output_header   统计信息输出在output


//把数组header中的每个字符转换为对应的字符串，例如把DEFAULT_OUTPUT_FORMAT转为
/*
timestamp       count   max     min     avg     med     stddev  95_max  95_avg  95_std  99_max  99_avg  99_std
*/ //时延统计关键函数见见get_flush_stats  outbound
static int
output_header(char header[], int verbatim) {
    char *c;
    
    for (c = header; c[0]; c ++)
        if (c[0] == '%') {
            int r = 100;
            c ++;
            
            if (c[0] >= '0' && c[0] <= '9') {
                r = 0;
                while (c[0] >= '0' && c[0] <= '9') {
                    r *= 10;
                    r += c[0] - '0';
                    
                    c ++;
                    
                }
                
            }
            
            if (c[0] == 'n')
                if (r != 100)
                    printf("%d_cnt", r);
                else
                    fputs("count", stdout);
            else if (c[0] == 'a')
                if (r != 100)
                    printf("%d_avg", r);
                else
                    fputs("avg", stdout);
            else if (c[0] == 's')
                if (r != 100)
                    printf("%d_sum", r);
                else
                    fputs("sum", stdout);
                   
            else if (c[0] == 'x')
                if (r != 100)
                    printf("%d_sqs", r);
                else
                    fputs("sqs", stdout);
            else if (c[0] == 'm')
                if (r != 100)
                    printf("%d_min", r);
                else
                    fputs("min", stdout);
            else if (c[0] == 'M')
                if (r != 100)
                    printf("%d_max", r);
                else
                    fputs("max", stdout);
            else if (c[0] == 'h')
                if (r != 100)
                    printf("%d_med", r);
                else
                    fputs("med", stdout);
            else if (c[0] == 'S')
                if (r != 100)
                    printf("%d_std", r);
                else
                    fputs("stddev", stdout);
            else if (c[0] == 'v')
                if (r != 100)
                    printf("%d_var", r);
                else
                    fputs("var", stdout);
            else if (c[0] == 'C')
                if (r != 100)
                    printf("%d_tc", r);
                else
                    fputs("tc", stdout);
            
            // Timestamping
            else if (c[0] == 'I')
                fputs("iter#", stdout);
            else if (c[0] == 't')
                fputs("elapsed", stdout);
            else if (c[0] == 'T')
                fputs("timestamp", stdout);
                            
        }
        else if (c[0] == '\\') {
            if (c[1] == 'n') {
                c ++;
                fputc('\n', stdout);
            }
            else if (c[1] == 't') {
                c ++;
                fputc('\t', stdout);
            }
            else if (verbatim) {
                if (c[1] == 'r') {
                    c ++;
                    fputc('\r', stdout);
                }
                else if (c[1] == '\\') {
                    c ++;
                    fputc('\\', stdout);
                }
                else
                    fputc('\\', stdout);
            }
        }
        else if (verbatim)
            fputc(c[0], stdout);
        
            
    fflush(stdout);
    
    return 0;

}

int
check_format(char format[]) {
    char *c;
    
    for (c = format; *c; c ++)
        if (c[0] == '%') {
            int r = -1;
            c ++;
            
            switch (c[0]) {
            
            case '0' ... '9':       // GNU extension
                r = 0;
                while (c[0] >= '0' && c[0] <= '9') {
                    r *= 10;
                    r += c[0] - '0';
                    
                    c ++;
                    
                }
                
                if (r <= 0 || r > 100)
                    return 0;
                
                break;
                
            case '%':
                if (r != -1)
                    return 0;
                
            case 'n':
            case 'a':
            case 's':
            case 'x':
            case 'r':
            case 'I':
            case 't':
            case 'T':
            case 'm':
            case 'M':
            case 'h':
            case 'S':
            case 'v':
            case 'C':
                break;

            default:
                return 0;
                
            }
            
        }
        
    return 1;
    
}

