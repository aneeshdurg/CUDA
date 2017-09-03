#include "task_queue.h"

task_queue* queue_create(){
  task_queue *Q = (task_queue*)malloc(sizeof(task_queue));
  //initialize mutex
  Q -> _m = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(Q->_m, NULL);
  //initialize conditional variable
  Q -> _cv = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(Q->_cv, NULL);
  //empty queue
  Q->q = new std::queue<svm_task*>;
  return Q; 
}
//destroys queue and frees memory
void queue_destroy(task_queue *Q){
  pthread_mutex_destroy(Q->_m);
  pthread_cond_destroy(Q->_cv);
  //printf("DESTROYED LOCK\n");  
  delete Q->q;
  //printf("DESTROYED Q\n");
  free(Q);
  //printf("FREED Q\n");
}
//gets the next task on the queue
//blocks if the queue is empty
svm_task* queue_get(task_queue *Q){
  pthread_mutex_lock(Q->_m);
  while (queue_empty(Q)){
    pthread_cond_wait(Q->_cv, Q->_m);
  }
  svm_task* ret = Q->q->front(); 
  Q->q->pop();
  pthread_mutex_unlock(Q->_m);
  return ret;
}
//checks if the queue is NULL (=> empty)
bool queue_empty(task_queue *Q){
  return Q->q->empty();
}
//data must be heap allocated!!!
void queue_put(task_queue *Q, svm_task* data){
  pthread_mutex_lock(Q->_m);
  Q->q->push(data);
  pthread_cond_broadcast(Q->_cv);
  pthread_mutex_unlock(Q->_m);
}


