/*
 * Jeffrey Bour
 * operation.h defines the basic operation
 * structure used throughout my program
 */

#ifndef _OPERATION_H_INCLUDE_
#define _OPERATION_H_INCLUDE_

typedef struct operation_obj {
	int id;
	int time;
	int track;
	int start;
	int end;
} operation;

#endif