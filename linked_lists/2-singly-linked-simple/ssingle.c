#include<stdio.h>            /* printf() */
#include<stdlib.h>           /* free(), malloc() */

			/* define singly linked list element */
typedef struct _slist {
	struct _slist *next;
	unsigned long long odd_val;
} _slist;

#define SL_SIZE sizeof(_slist)

int main(int argv, char** argc){
	_slist *root, *node, *prev;
			/* Set range.
			 * WARNING: too large gap between min & max can exhaust RAM */
	long unsigned min = 5, max = 9999;

	printf("cli %i - %s\n", argv, argc[0]);
			/* pre allocate root node */
	root = malloc(SL_SIZE);
	root->odd_val = min; prev = root;
			/* set starting point for the loop */
	long unsigned loop_min = min + 2;
			/* insert rest of the odd values to the singly linked list
			 * till we reach max. */
	for (long unsigned n = loop_min; n <= max; n = n + 2) {
		node = malloc(SL_SIZE);
		node->odd_val = n; node->next = NULL;
		prev->next = node; prev = node;
	}
			/* reset node to root */
	node = root;
			/* output the content of the list and free nodes as we go */
	while(node->next) {
		printf("Node val %lli\n", node->odd_val);
		_slist *tmp;
		tmp = node;
		node = node->next;
		free(tmp);
	}
			/* print and free the remaining node (it's next == NULL) */
	printf("Node val %lli\n", node->odd_val);
	free(node);
}

/*   Links
 *   + https://en.wikipedia.org/wiki/Linked_list
 *
 *   Notes
 *   Very archaic singly linked list implementation.
 *
 *   (C) Antti Antinoja, 2019
 */
