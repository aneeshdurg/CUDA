#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "svm.h"
//
// decision_function
//
typedef struct{
	double *alpha;
	double rho;	
} decision_function;

typedef struct{
  int id;
  int sj;
  int cj;
  int si;
  int ci;
  int i;
  int j;
} svm_task;

typedef struct{
  pthread_cond_t* _cv;
  pthread_mutex_t* _m;
  std::queue<svm_task*> *q;
} task_queue;

typedef struct{
  double **weighted_C;
  const svm_parameter *param;
  double **probA, **probB;
  decision_function **f;
#ifdef _DENSE_REP
  svm_node *x;
#else
  svm_node **x;
#endif
  task_queue *q;
} svm_common_info;
 
task_queue* queue_create();
void queue_destroy(task_queue* Q);
svm_task* queue_get(task_queue* Q);
bool queue_empty(task_queue *Q);
void queue_put(task_queue *Q, svm_task* data);
