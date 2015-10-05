/*
 * Jeffrey Bour
 * iosched.c contains the main portion of my solution
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "operation.h"
#include "queue.h"
#include "iosched_objects.h"

char algo;
int l_opt;

queue waiting;
queue processing;
queue complete;

int total_time;
int tot_movement;
int tot_turnaround;
int tot_waittime;
int max_waittime;

int current_track;
int direction;//0==down, 1==up

operation (*get_next_operation)(queue q);
void (*add_to_queue)(operation o);
void (*check_queue)(void);

void print_sum() {
	double avg_turnaround = (double)tot_turnaround/size(complete);
	double avg_waittime = (double)tot_waittime/size(complete);
	printf("SUM: %d %d %.2lf %.2lf %d\n",total_time,tot_movement,avg_turnaround,avg_waittime,max_waittime);
}

void finish_operation() {
	operation o = dequeue(processing);
	total_time+=abs(current_track-o.track);
	o.end = total_time;
	tot_turnaround+=(o.end-o.time);
	tot_movement+=(abs(o.track-current_track));
	current_track = o.track;
	if(l_opt) printf("%d:     %d finish %d\n", total_time, o.id, total_time-o.time);
	id_insert(complete, o);
	check_queue();
	if(!is_empty(processing)) {
		o = get_next_operation(processing);
		tot_waittime+=(total_time-o.time);
		if((total_time-o.time)>max_waittime) max_waittime = (total_time-o.time);
		o.start = total_time;
		move_to_front(processing, o);
		if(l_opt) printf("%d:     %d issue %d %d\n", total_time, o.id, o.track, current_track);
	}
}

void run_simulation() {
	current_track = 0;
	direction = 1;
	int num_ops = size(waiting);
	if(l_opt) printf("TRACE\n");
	while(size(complete)<num_ops) {
		if(!is_empty(waiting) && !is_empty(processing)) {
			if(peek(waiting).time<=(total_time+abs(current_track-peek(processing).track))) {
				operation o = dequeue(waiting);
				operation p = peek(processing);
				tot_movement+=(abs(total_time-o.time));
				current_track = (p.track>current_track) ? current_track+(o.time-total_time) : current_track-(o.time-total_time);
				total_time = o.time;
				if(l_opt) printf("%d:     %d add %d\n", total_time, o.id, o.track);
				add_to_queue(o);
			}
			else finish_operation();
		}
		else if(is_empty(waiting)) finish_operation();
		else if(is_empty(processing)) {//use waiting queue
			operation o = dequeue(waiting);
			total_time = o.time;
			o.start = total_time;
			if(l_opt) printf("%d:     %d add %d\n", total_time, o.id, o.track);
			if(l_opt) printf("%d:     %d issue %d %d\n", total_time, o.id, o.track, current_track);
			enqueue(processing, o);
		}
	}
	if(l_opt) print_summary(complete);
}

char advance_file_reader(char c, FILE* in) {
	c = fgetc(in);
	while(c=='#') {
		while(c!='\n' && c!='\t') c = fgetc(in);
		c = fgetc(in);
	}
	return c;
}

void read_file(char* in) {
	FILE* infile = fopen(in, "r");
	if(infile==NULL) {
		printf("Unable to read from file \"%s\"\n", in);
		exit(EXIT_FAILURE);
	}
	char c;
	int i = 0;
	c = advance_file_reader(c, infile);
	while(!feof(infile)) {
		operation o;
		o.id = i;
		int num = c-48;
		while((c=fgetc(infile))>=48 && c<=57) num = (num*10) + (c-48);
		o.time = num;
		num = 0;
		while((c=fgetc(infile))>=48 && c<=57) num = (num*10) + (c-48);
		o.track = num;
		i++;
		enqueue(waiting, o);
		c = advance_file_reader(c, infile);
	}
}

void iosched_init() {
	if(algo=='i') new_fifo_obj();
	else if(algo=='j') new_sstf_obj();
	else if(algo=='s') new_scan_obj();
	else if(algo=='c') new_cscan_obj();
	else if(algo=='f') new_fscan_obj();
	total_time = 0;
	tot_movement = 0;
	tot_turnaround = 0;
	tot_waittime = 0;
	max_waittime = 0;
}

void parse_args(int argc, char* argv[]) {
	l_opt = 0;
	int sflag = 0;
	char *svalue = NULL;
	int c;
	opterr = 0;
	while ((c = getopt (argc, argv, "ls:")) != -1) {
		switch (c) {
			case 'l':
				l_opt = 1;
				break;
			case 's':
				svalue = optarg;
				if(strlen(svalue)!=1) {
					fprintf(stderr, "Error: illegal algorithm specification\n");
					exit(EXIT_FAILURE);
				}
				if(svalue[0]=='i' || svalue[0]=='j' || svalue[0]=='s' || svalue[0]=='c' || svalue[0]=='f') {
					algo = svalue[0];
					sflag = 1;
				} else {
					fprintf(stderr, "Error: illegal algorithm specification\n");
					exit(EXIT_FAILURE);
				}
				break;
			case '?':
				if (optopt=='s') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt)) fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				exit(EXIT_FAILURE);
			default:
				abort ();
		}
	}
	if(sflag==0) {
		fprintf(stderr, "Please provide algorithm specification\n");
		exit(EXIT_FAILURE);
	}
	if((argc-optind)!=1) {
		fprintf(stderr, "Usage: ./iosched -s<schedalgo> [-l] <inputfile>\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char* argv[]) {
	parse_args(argc, argv);
	iosched_init();
	read_file(argv[argc-1]);
	run_simulation();
	print_sum();
	return 0;
}