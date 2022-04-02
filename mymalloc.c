// Lab 7: mymalloc.c
// The goal of this lab is to write my_malloc() and my_free(), 
//      which should work just like malloc() and free()
//      See http://web.eecs.utk.edu/~jplank/plank/classes/cs360/360/labs/Lab-7-Malloc/index.html
//      for more information and lab specifications. 

// COSC 360
// Swasti Mishra 
// Mar 28, 2022
// James Plank 

// Libraries
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

// Our one global variable 
void *head ;

// Our node, modelled after Plank's
struct Node{
	size_t size ;
	struct Node *blink ;
	struct Node *flink ;
} ;

// Converts to int and compares 
int compare( void *a, void *b ){
    int *first  = (int*)(a) ;
	int *second = (int*)(b) ;
    return *first - *second ; 
}

void *my_malloc( size_t size ){
	
    // Make sure the variable is divisible by 8 
    if( ( size % 8 ) != 0 ){
        size = ( ( size + 8 ) - ( size % 8 ) ) ;
    }
    size = ( size + 8 ) ;

	// Start allocating memory
    if(head == NULL){
		long int ptr ;

		// Allocates space in case size is larger than 8192
		if( size > 8192 ){
			ptr = size ;
		}
		else{ 
			ptr = 8192 ;
		}
        head = sbrk( ptr ) ;
        void * temp = head ;

        // Check size
		if( ptr >= ( size + 4 ) ){

			// When the freelist is empty set chunk to head
            head = ( head + size ) ;
			struct Node *node = head ;
			node->size = ( ptr - size ) ;
			node->blink = NULL ;
			node->flink = NULL ;
        } 

		// Otherwise, place at the top of the list 
		else{
			head = NULL ;
		}

		// Add padding 
		int *i = temp ;
		*i = size ;
		return (void*)( (int)( temp + 8 ) ) ;
	}

	// Reset back to head 
	struct	Node *node = head ;
	int check = -1 ;
	
    // When the size doesn't need the whole chunk 
    while(1){
        if( node->size >= ( size + 4 ) ){
			check = 0 ;
			break ;
		}
		if( node->size >= ( size ) ){
            check = 1 ;
			break ;
		}
		if( node->flink == NULL ){
            check = 2 ;
			break ;
		}
		node = node->flink ;
	}

    // Make a new node if check returns zero 
	if( check == 0 ){
		
        struct Node *new = (void*)( (int)node + size ) ;
		
        new->size  = node->size - size ;
		new->blink = node->blink ;
		new->flink = node->flink ;
		
		if( node->blink != NULL ){
			node->blink->flink = new ;
		}
        if( node->flink != NULL ){
			node->flink->blink = new ;
		}
		
        node->size = size ;
		
        if( node == head ){
			head = new ;
		}

		return (void*)( (int) node + 8 ) ;
	}

    // Delete a node 
	if( check == 1 ){
		if(node->blink != NULL){
			node->blink->flink = node->flink ;
		}
        if(node->flink != NULL){
			node->flink->blink = node->blink ;
		}
        node->size = size ;
        if( head == node ){
			if( node->flink != NULL ){
				node->flink->blink = NULL ;
			}
            head = node->flink ;
		}

		return (void*)( (int) node + 8 ) ;
	}

	// If we need more memory- same procedure as above 
    long int ptr ; 
	if( size > 8192 ){
		ptr = size ;
	}
	else{ 
		ptr = 8192 ;
	}
	void *temp = sbrk( ptr ) ;

    if( ptr >= ( size + 4 ) ){
		struct Node *new = (void*)( ( (int)temp ) + size) ;
		
        new->size = ptr - size ;
		new->blink = node ;
		new->flink = NULL ;
        node->flink = new ;
	}
	
    int *i = temp ;
	*i = size ;
	
    return (void*)( (int)temp + 8 ) ;
}

// Put nodes on the free list 
void my_free( void *ptr ){
    struct	Node *node ;
    
	// Finds the next free node 
	if( head != NULL ){ 
		struct Node *new = (void*)(int)( ptr - 8 ) ;
		node = head ;
        while( node->flink != NULL ){
			node = node->flink ;
		}
		node->flink = new ;
		new->blink = node ;
		new->flink = NULL ;
	} 
	// Otherwise, add a new node 
	else{
        struct Node *new = ( ptr - 8 ) ;
		new->flink = new->blink = NULL ;
		head = new ;
	}
}

// First free node 
void *free_list_begin(){
	return head ;
}

// Finds the next node given a node 
void *free_list_next( void *node ){
    struct Node *new = node ;
    return new->flink ;
}

// Put it all into one list 
void coalesce_free_list(){
	
    int counter = 0 ;
	int k = 0 ;
	struct Node *node = head ;

	// Go through list 
	if( head != NULL ){
		int i ;
		while( node != NULL ){
			node = node->flink ;
			i++ ;
		}

		// Initializes array for pointers and sorts 
		int j ;
		int array[i];
		node = head ;
		for( j = 0 ; j < i ; j++ ){
            array[j] = (int)(node) ;
			node = node->flink ;
		}
		qsort( array, i, sizeof(int), (__compar_fn_t)(compare) ) ;
		node = head ;
		
        while(1){
            counter = 0 ;
            for( j = 0; j < ( i - 1 ) ; j++ ){
                struct Node *a = (struct Node*)array[j] ;
                for( k = ( j + 1 ) ; k < i ; k++ ){
					
                    if (array[j] > 0 && array[k] > 0 && array[j] + a->size == array[k]) {
						counter++;
						struct Node * b = (struct Node*)array[k];
						a->size += b->size;
						b->size = 0;
						array[k] = -1;
					}
				}
			}

			if (counter == 0){
				break ;
			}
		}
		struct Node *m;
		head = (void *) array[0];
		m = head;
		m->flink = m->blink = NULL;
		
        for (counter = 1; counter < i; counter++){

			if (array[counter] != -1){
				node = (void *)array[counter];
				m->flink = node;
				node->blink = m;
				m = node;
			}
		}
		m->flink = NULL;
	}
}