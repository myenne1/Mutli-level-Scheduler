
// Illustrates basic usage of some prioque.c functions.
// Written 9/2017 by Golden G. Richard III (@nolaforensix)
//
// Updated 8/2021 to include additional tests.
// Updated and tweaks by AAGombe 9/2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prioque.h"

typedef struct _SomeType {
  int a;
  char buf[10];
} SomeType;


// comparison function for queue of SomeType elements
int some_type_compare(const void *e1, const void *e2) {
  SomeType *s1=(SomeType *)e1;
  SomeType *s2=(SomeType *)e2;
  if (s1->a == s2->a && !strcmp(s1->buf, s2->buf)) {
    return 1;
  }
  else {
    return 0;
  }
}


int main(int argc, char *argv[]) {
  Queue q, another_q;
  SomeType s1, s2, s3, s4, e;
  int element;
  
  s1.a=5;
  strcpy(s1.buf, "hello");
  s2.a=7;
  strcpy(s2.buf, "yo");
  s3.a=3;
  strcpy(s3.buf, "hola");
  s4.a=5;
  strcpy(s4.buf, "whereyat");

  printf("Testing priority queue functionality.\n");

  printf("Initializing queue.\n");
  init_queue(&q, sizeof(SomeType), TRUE, some_type_compare, FALSE);

  printf("Adding %d / %s.\n", s1.a, s1.buf);
  add_to_queue(&q, &s1, s1.a);
  printf("Adding %d / %s.\n", s2.a, s2.buf);
  add_to_queue(&q, &s2, s2.a);
  printf("Adding %d / %s.\n", s3.a, s3.buf);
  add_to_queue(&q, &s3, s3.a);
  printf("Adding %d / %s.\n", s4.a, s4.buf);
  add_to_queue(&q, &s4, s4.a);

  element=0;
  rewind_queue(&q);
  printf("Queue contains:\n");
  while (! end_of_queue(&q)) {
    printf("%d --> %d / %s with priority %d.\n",
	   ++element,
	   ((SomeType *)pointer_to_current(&q))->a,
	   ((SomeType *)pointer_to_current(&q))->buf,
	   current_priority(&q));
    next_element(&q);
  }
  
  printf("Removing element at head of queue.\n");
  remove_from_front(&q, &e);
  printf("Element is  %d / %s.\n", e.a, e.buf);

  element=0;
  rewind_queue(&q);
  printf("Queue contains:\n");
  while (! end_of_queue(&q)) {
    printf("%d --> %d / %s with priority %d.\n",
	   ++element,
	   ((SomeType *)pointer_to_current(&q))->a,
	   ((SomeType *)pointer_to_current(&q))->buf,
	   current_priority(&q));
    next_element(&q);
  }

  printf("Destroying queue with destroy_queue().\n");
  destroy_queue(&q);

  element=0;
  rewind_queue(&q);
  printf("Queue contains:\n");
  while (! end_of_queue(&q)) {
    printf("%d --> %d / %s with priority %d.\n",
	   ++element,
	   ((SomeType *)pointer_to_current(&q))->a,
	   ((SomeType *)pointer_to_current(&q))->buf,
	   current_priority(&q));
    next_element(&q);
  }

  printf("Testing FIFO queue functionality.\n");

  printf("Initializing queue.\n");
  init_queue(&q, sizeof(SomeType), TRUE, some_type_compare, TRUE);
  
  printf("Adding %d / %s\n", s1.a, s1.buf);
  add_to_queue(&q, &s1, s1.a);
  printf("Adding %d / %s\n", s2.a, s2.buf);
  add_to_queue(&q, &s2, s2.a);
  printf("Adding %d / %s\n", s3.a, s3.buf);
  add_to_queue(&q, &s3, s3.a);
  printf("Adding %d / %s\n", s4.a, s4.buf);
  add_to_queue(&q, &s4, s4.a);

  element=0;
  rewind_queue(&q);
  printf("Queue contains:\n");
  while (! end_of_queue(&q)) {
    printf("%d --> %d / %s with tag %d.\n",
	   ++element,
	   ((SomeType *)pointer_to_current(&q))->a,
	   ((SomeType *)pointer_to_current(&q))->buf,
	   current_priority(&q));
    next_element(&q);
  }
  

  printf("Searching for and removing elements with tag %d from queue.\n", e.a);
  rewind_queue(&q);
  while (! end_of_queue(&q)) {
    printf("Current element is %d / %s with tag %d.\n",
	   ((SomeType *)pointer_to_current(&q))->a,
	   ((SomeType *)pointer_to_current(&q))->buf,
	   current_priority(&q));
    if (((SomeType *)pointer_to_current(&q))->a == e.a) {
      printf("Found element with tag %d!  Deleting.\n", e.a);
      delete_current(&q);
    }
    else {
      next_element(&q);
    }
  }  

  element=0;
  rewind_queue(&q);
  printf("Queue contains:\n");
  while (! end_of_queue(&q)) {
    printf("%d --> %d / %s with tag %d.\n",
	   ++element,
	   ((SomeType *)pointer_to_current(&q))->a,
	   ((SomeType *)pointer_to_current(&q))->buf,
	   current_priority(&q));
    next_element(&q);
  }
  
  printf("Removing the rest of the elements one by one.\n");

  while (queue_length(&q)) {
    rewind_queue(&q);
    delete_current(&q);
    rewind_queue(&q);

    element=0;
    printf("Queue contains:\n");
    while (! end_of_queue(&q)) {
      printf("%d --> %d / %s with tag %d.\n",
	     ++element,
	     ((SomeType *)pointer_to_current(&q))->a,
	     ((SomeType *)pointer_to_current(&q))->buf,
	     current_priority(&q));
      next_element(&q);
    }
  }
  
  printf("Testing copy_queue() and equal_queues().\n");

  printf("Initializing queues.\n");
  init_queue(&q, sizeof(SomeType), TRUE, some_type_compare, TRUE);
  init_queue(&another_q, sizeof(SomeType), TRUE, some_type_compare, TRUE);
  
  printf("Adding %d / %s to queue q.\n", s1.a, s1.buf);
  add_to_queue(&q, &s1, s1.a);
  printf("Adding %d / %s to queue q.\n", s2.a, s2.buf);
  add_to_queue(&q, &s2, s2.a);
  printf("Adding %d / %s to queue q.\n", s3.a, s3.buf);
  add_to_queue(&q, &s3, s3.a);
  printf("Adding %d / %s to queue q.\n", s4.a, s4.buf);
  add_to_queue(&q, &s4, s4.a);

  element=0;
  printf("Queue q contains:\n");
  while (! end_of_queue(&q)) {
    printf("%d --> %d / %s with tag %d.\n",
	   ++element,
	   ((SomeType *)pointer_to_current(&q))->a,
	   ((SomeType *)pointer_to_current(&q))->buf,
	   current_priority(&q));
    next_element(&q);
  }

  printf("Copying q to another_q.\n");

  copy_queue(&another_q, &q);

  element=0;
  printf("Queue another_q contains:\n");
  while (! end_of_queue(&another_q)) {
    printf("%d --> %d / %s with tag %d.\n",
	   ++element,
	   ((SomeType *)pointer_to_current(&another_q))->a,
	   ((SomeType *)pointer_to_current(&another_q))->buf,
	   current_priority(&another_q));
    next_element(&another_q);
  }
  
  if (equal_queues(&q, &another_q)) {
    printf("Queues are equal.\n");
  }
  else {
    printf("Something went wrong!  Queues aren't equal.\n");
  }
}

