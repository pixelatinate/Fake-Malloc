// Lab 7: mymalloc.c
// The goal of this lab is to write my_malloc() and my_free(), 
//      which should work just like malloc() and free()

//      http://web.eecs.utk.edu/~jplank/plank/classes/cs360/360/labs/Lab-7-Malloc/index.html
//      for more information and lab specifications. 

// COSC 360
// Swasti Mishra 
// Mar 28, 2022
// James Plank 

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

void *head ;

struct Node{
	size_t size ;
	struct Node *blink ;
	struct Node *flink ;
} ;

void *my_malloc(size_t size){} ;
void my_free(void *ptr){} ;
void *free_list_begin(){} ;
void *free_list_next(void *node){} ;
int comparator(void * a, void * b){} ;
void coalesce_free_list(){} ;