/*
 * Jeffrey Bour
 * iosched_objects.c initializes queues and
 * assigns function pointers used for each
 * scheduling algorithm
 */

#include <stdio.h>
#include <stdlib.h>
#include "operation.h"
#include "queue.h"

extern operation (*get_next_operation)(queue q);
extern void (*add_to_queue)(operation o);
extern void (*check_queue)(void);

extern int current_track;
extern int direction;

extern queue waiting;
extern queue processing;
extern queue complete;

queue backup;//fscan only

void standard_add_to_queue(operation o) {
	enqueue(processing, o);
}

void fscan_add_to_queue(operation o) {
	enqueue(backup, o);
}

void standard_check_queue() {
	//do nothing
}

void fscan_check_queue() {
	if(is_empty(processing)) {
		queue hold = processing;
		processing = backup;
		backup = hold;
		direction = 1;
	}
}

void queue_init() {
	waiting = new_queue();
	processing = new_queue();
	complete = new_queue();
}

void new_fifo_obj() {
	queue_init();
	get_next_operation = &fifo;
	add_to_queue = &standard_add_to_queue;
	check_queue = &standard_check_queue;
}

void new_sstf_obj() {
	queue_init();
	get_next_operation = &sstf;
	add_to_queue = &standard_add_to_queue;
	check_queue = &standard_check_queue;
}

void new_scan_obj() {
	queue_init();
	get_next_operation = &scan;
	add_to_queue = &standard_add_to_queue;
	check_queue = &standard_check_queue;
}

void new_cscan_obj() {
	queue_init();
	get_next_operation = &cscan;
	add_to_queue = &standard_add_to_queue;
	check_queue = &standard_check_queue;
}

void new_fscan_obj() {
	queue_init();
	backup = new_queue();
	get_next_operation = &fscan;
	add_to_queue = &fscan_add_to_queue;
	check_queue = &fscan_check_queue;
}