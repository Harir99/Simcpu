#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simcpu.h"

//function to get right child of a node of a tree
int rightChildIndex(int index) {
  if((((2*index)+1) < MAXSIZE) && (index >= 1))
    return (2*index)+1;
  return -1;
}

//function to get left child of a node of a tree
int leftChildIndex(int index) {
    if(((2*index) < MAXSIZE) && (index >= 1))
        return 2*index;
    return -1;
}

//function to get the parent of a node of a tree
int parentIndex(int index) {
  if ((index > 1) && (index < MAXSIZE)) {
    return index/2;
  }
  return -1;
}

void swap(Thread *a, Thread *b)
{
  Thread t;
  t = *a;
  *a = *b;
  *b = t;
}

void bubbleDown(Thread *thread[], int index, int *heap_size)
{
  int left_child_index =  leftChildIndex(index);
  int right_child_index = rightChildIndex(index);

  // finding smallest among index, left child and right child
  int smallest = index;

  if ((left_child_index <= *heap_size) && (left_child_index > 0))
  {
    if (thread[left_child_index]->arrivalTime < thread[smallest]->arrivalTime)
    {
      smallest = left_child_index;
    }
  }

  if ((right_child_index <= *heap_size && (right_child_index > 0)))
  {
    if (thread[right_child_index]->arrivalTime < thread[smallest]->arrivalTime)
    {
      smallest = right_child_index;
    }
  }

  // smallest is not the node, node is not a heap
  if (smallest != index)
  {
    swap(thread[index], thread[smallest]);
    bubbleDown(thread, smallest, heap_size);
  }
}

void heapify(Thread *thread[], int *heap_size)
{
  int i;
  for (i = (*heap_size) / 2; i >= 1; i--)
  {
    bubbleDown(thread, i, heap_size);
  }
}

Thread *getMin(Thread *thread[])
{
  return thread[1];
}

Thread *removeMin(Thread *thread[], int *heap_size)
{

  if (*heap_size == 0)
  {
    return NULL;
  }
  Thread *minm = thread[1];
  thread[1] = thread[*heap_size];
  *heap_size = (*heap_size) - 1;
  bubbleDown(thread, 1, heap_size);
  return minm;
}

void decrease_key(Thread *thread[], int index, Thread *key)
{
  thread[index] = key;
  while ((index > 1) && (thread[parentIndex(index)]->arrivalTime > thread[index]->arrivalTime))
  {

    swap(thread[index], thread[parentIndex(index)]);
    index = parentIndex(index);
  }
  if (index > 1 && thread[parentIndex(index)]->arrivalTime == thread[index]->arrivalTime)
  {

    while ((index > 1) && (thread[parentIndex(index)]->threadNumber > thread[index]->threadNumber))
    {
      swap(thread[index], thread[parentIndex(index)]);
      index = parentIndex(index);
    }

    if (index > 1 && thread[parentIndex(index)]->threadNumber == thread[index]->threadNumber)
    {
      while ((index > 1) && (thread[parentIndex(index)]->processNumber > thread[index]->processNumber))
      {
        swap(thread[index], thread[parentIndex(index)]);
        index = parentIndex(index);
      }
    }
  }
}
void insert(Thread *thread[], Thread *key, int *heap_size)
{
  *heap_size = (*heap_size) + 1;
  thread[*heap_size] = key;
  decrease_key(thread, *heap_size, key);
}
