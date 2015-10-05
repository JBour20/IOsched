/*
 * Jeffrey Bour
 * iosched_objects.h defines the initialization
 * functions used to create new scheduling objects
 */

#ifndef _IOSCHED_OBJECTS_H_INCLUDE_
#define _IOSCHED_OBJECTS_H_INCLUDE_

#include "operation.h"
#include "queue.h"

void new_fifo_obj();

void new_sstf_obj();

void new_scan_obj();

void new_cscan_obj();

void new_fscan_obj();

#endif