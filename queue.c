/*
 * Jeffrey Bour
 * queue.c contains my implementation of
 * the basic data structure used to store
 * incoming and outgoing io operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "operation.h"
#include "queue.h"

extern int current_track;
extern int direction;

typedef struct node_obj {
	operation op;
	struct node_obj* next;
} node_obj;

typedef struct node_obj* node;

node new_node(operation o) {
	node n = malloc(sizeof(node_obj));
	assert(n!=NULL);
	n->op = o;
	n->next = NULL;
	return n;
}

void free_node(node* pn) {
	if(pn!=NULL && *pn!=NULL) {
		free(*pn);
		*pn = NULL;
	}
}

typedef struct queue_obj {
	node head;
	node tail;
	int count;
} queue_obj;

queue new_queue() {
	queue q = malloc(sizeof(queue_obj));
	assert(q!=NULL);
	q->head = NULL;
	q->tail = NULL;
	q->count = 0;
	return q;
}

void free_queue(queue *pq) {
	if(pq!=NULL && *pq!=NULL) {
		if(!is_empty(*pq)) make_empty(*pq);
		free(*pq);
		*pq = NULL;
	}
}

int is_empty(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling is_empty() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	return (q->count==0);
}

int size(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling size() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	return q->count;
}

void move_to_front(queue q, operation o) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling move_to_front() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	node n = new_node(o);
	if(is_empty(q)) q->head = q->tail = n;
	else {
		n->next = q->head;
		q->head = n;
	}
	q->count++;
}

void enqueue(queue q, operation o) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling enqueue() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	node n = new_node(o);
	if(q->count==0) q->head = q->tail = n;
	else {
		q->tail->next = n;
		q->tail = q->tail->next;
	}
	q->count++;
}

void id_insert(queue q, operation o) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling id_insert() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	node n = new_node(o);
	if(q->count==0) q->head = q->tail = n;
	else if(o.id<(q->head->op).id) {
		n->next = q->head;
		q->head = n;
	} else if(o.id>(q->tail->op).id) {
		q->tail->next = n;
		q->tail = q->tail->next;
	} else {
		node y = q->head;
		node x;
		while((y->op).id<o.id) {
			x = y;
			y = y->next;
		}
		x->next = n;
		n->next = y;
	}
	q->count++;
}

operation dequeue(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling dequeue() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	if(is_empty(q)) {
		fprintf(stderr, "queue error: calling dequeue() on empty queue\n");
		exit(EXIT_FAILURE);
	}
	node n = q->head;
	operation o = n->op;
	if(q->count==1) {
		free_node(&n);
		q->head = q->tail = NULL;
	} else {
		q->head = q->head->next;
		n->next = NULL;
		free_node(&n);
	}
	q->count--;
	return o;
}

operation fifo(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling dequeue() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	if(is_empty(q)) {
		fprintf(stderr, "queue error: calling dequeue() on empty queue\n");
		exit(EXIT_FAILURE);
	}
	node n = q->head;
	operation o = n->op;
	if(q->count==1) {
		free_node(&n);
		q->head = q->tail = NULL;
	} else {
		q->head = q->head->next;
		n->next = NULL;
		free_node(&n);
	}
	q->count--;
	if(o.track>current_track) direction = 1;
	else if(o.track<current_track) direction = 0;
	return o;
}

operation sstf(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling sstf() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	if(is_empty(q)) {
		fprintf(stderr, "queue error: calling sstf() on empty queue\n");
		exit(EXIT_FAILURE);
	}
	node current = q->head;
	node prev = NULL;
	int dist = abs((current->op).track-current_track);
	node y = q->head->next;
	node x = q->head;
	while(y!=NULL) {
		if(abs((y->op).track-current_track)<dist) {
			prev = x;
			current = y;
			dist = abs((current->op).track-current_track);
		}
		x = y;
		y = y->next;
	}
	operation o = current->op;
	if(prev==NULL) {
		q->head = q->head->next;
		current->next = NULL;
	} else if(current->next==NULL) {
		q->tail = prev;
		prev->next = NULL;
	} else {
		prev->next = current->next;
		current->next = NULL;
	}
	free_node(&current);
	q->count--;
	if(o.track>current_track) direction = 1;
	else if(o.track<current_track) direction = 0;
	return o;
}

operation scan(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling scan() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	if(is_empty(q)) {
		fprintf(stderr, "queue error: calling scan() on empty queue\n");
		exit(EXIT_FAILURE);
	}
	node current;
	node prev;
	current = prev = NULL;
	int dist = INT_MAX;
	node y = q->head;
	node x = NULL;
	while(y!=NULL) {
		if((direction==1 && ((y->op).track)>=current_track) || (direction==0 && ((y->op).track<=current_track))) {
			if(abs((y->op).track-current_track)<dist) {
				prev = x;
				current = y;
				dist = abs((current->op).track-current_track);
			}
		}
		x = y;
		y = y->next;
	}
	if(current==NULL) {
		direction = (direction==1) ? 0 : 1;
		y = q->head;
		x = NULL;
		while(y!=NULL) {
			if((direction==1 && ((y->op).track)>=current_track) || (direction==0 && ((y->op).track<=current_track))) {
				if(abs((y->op).track-current_track)<dist) {
					prev = x;
					current = y;
					dist = abs((current->op).track-current_track);
				}
			}
			x = y;
			y = y->next;
		}
	}
	operation o = current->op;
	if(prev==NULL) {
		q->head = q->head->next;
		current->next = NULL;
	} else if(current->next==NULL) {
		q->tail = prev;
		prev->next = NULL;
	} else {
		prev->next = current->next;
		current->next = NULL;
	}
	free_node(&current);
	q->count--;
	if(o.track>current_track) direction = 1;
	else if(o.track<current_track) direction = 0;
	return o;
}

operation cscan(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling cscan() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	if(is_empty(q)) {
		fprintf(stderr, "queue error: calling cscan() on empty queue\n");
		exit(EXIT_FAILURE);
	}
	node n;
	node m;
	m = n = NULL;
	int dist = INT_MAX;
	node y = q->head;
	node x = NULL;
	while(y!=NULL) {
		if((y->op).track>=current_track && ((y->op).track-current_track)<dist) {
			n = y;
			m = x;
			dist = (y->op).track-current_track;
		}
		x = y;
		y = y->next;
	}
	if(n==NULL) {
		y = q->head;
		x = NULL;
		while(y!=NULL) {
			if((y->op).track<dist) {
				n = y;
				m = x;
				dist = (y->op).track;
			}
			x = y;
			y = y->next;
		}
	}
	operation o = n->op;
	if(m==NULL) {
		q->head = q->head->next;
		n->next = NULL;
	} else if((n->next)==NULL) {
		q->tail = m;
		m->next = NULL;
	} else {
		m->next = n->next;
		n->next = NULL;
	}
	free_node(&n);
	q->count--;
	return o;
}

operation fscan(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling scan() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	if(is_empty(q)) {
		fprintf(stderr, "queue error: calling scan() on empty queue\n");
		exit(EXIT_FAILURE);
	}
	node current;
	node prev;
	current = prev = NULL;
	int dist = INT_MAX;
	node y = q->head;
	node x = NULL;
	if(direction==1) {
		while(y!=NULL) {
			if((y->op).track>=current_track && abs((y->op).track-current_track)<dist) {
				prev = x;
				current = y;
				dist = abs((y->op).track-current_track);
			}
			x = y;
			y = y->next;
		}
	}
	if(current==NULL) {
		direction = 0;
		y = q->head;
		x = NULL;
		while(y!=NULL) {
			if((y->op).track<=current_track && abs((y->op).track-current_track)<dist) {
				prev = x;
				current = y;
				dist = abs((y->op).track-current_track);
			}
			x = y;
			y = y->next;
		}
	}
	operation o = current->op;
	if(prev==NULL) {
		q->head = q->head->next;
		current->next = NULL;
	} else if(current->next==NULL) {
		q->tail = prev;
		prev->next = NULL;
	} else {
		prev->next = current->next;
		current->next = NULL;
	}
	free_node(&current);
	q->count--;
	return o;
}

operation peek(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling peek() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	if(is_empty(q)) {
		fprintf(stderr, "queue error: calling peek() on empty queue\n");
		exit(EXIT_FAILURE);
	}
	return q->head->op;
}

void make_empty(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling make_empty() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	node n;
	while(q->count>0) {
		n = q->head;
		q->head = q->head->next;
		n->next = NULL;
		free_node(&n);
		q->count--;
	}
	q->head = q->tail = NULL;
}

void print_summary(queue q) {
	if(q==NULL) {
		fprintf(stderr, "queue error: calling print_summary() on NULL queue reference\n");
		exit(EXIT_FAILURE);
	}
	printf("IOREQS INFO\n");
	node n = q->head;
	operation o;
	while(n!=NULL) {
		o = n->op;
		printf("%5d:%6d%6d%6d\n", o.id, o.time, o.start, o.end);
		n = n->next;
	}
}