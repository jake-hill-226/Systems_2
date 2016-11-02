#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_N_INPUT 6000
#define MAX_M_INPUT 3000
#define MAX_P_INPUT 1000

//Used to pass multiple arguments to pthread_create
struct arg_struc {
  int num_threads;
  int seq_num;
  int *result_matrix;
};

int A[MAX_N_INPUT][MAX_M_INPUT];
int B[MAX_M_INPUT][MAX_P_INPUT];
//int C[MAX_N_INPUT][MAX_P_INPUT];
//int C_prime[MAX_N_INPUT][MAX_P_INPUT];


int m, n, p;

void * mult_matrix(void * arg_struc);

int main(int argc, char *argv[] ){
  
  int num_threads = atoi(argv[1]);
  pthread_t tid[num_threads];
  int i, j;
  float start_time, end_time;
  printf("\nEnter n(<=6000), m(<=3000), p(<=1000): ");
  scanf(" %d %d %d", &n, &m, &p);
  printf("\nYou entered: %d %d %d", n, m, p);
  
  //Initialize A and B values
  for(i = 0; i < n; i++){
    for(j = 0; j < m; j++){
      A[i][j] = i * j;
    }
  }
  
  for( i = 0; i < m; i++){
    for( j = 0; j < p; j++){
      B[i][j] = i + j;
    }
  }
  
  int C[n][p];
  
  //Baseline
  start_time = gettimeofday();
  struct arg_struc baseline = {.num_threads = 1, .seq_num = 0, .result_matrix = &(C[0][0])};
  if( pthread_create(&tid[0], NULL, mult_matrix, (void *) &baseline) < 0){
    printf("\nError Creating Thread. Terminating Program");
    return -1;
  }
  pthread_join(tid[0], NULL);
  end_time = gettimeofday();
  
  printf("\nThread\t\tSeconds");
  printf("\n1\t\t %.2f", (end_time - start_time));
  
  if(num_threads <= 1){
    return 0;
  }
  
  //Multi-Threading starting with 2 threads
  int k, comp_error;
  int C_prime[n][p];
  
  for(i = 1; i < num_threads; i++){
    start_time = gettimeofday();
    
    //create i threads to compute product
    for(j = 0; j < i; j++){
      struct arg_struc args = {.num_threads = i,  .seq_num = j, .result_matrix = &(C_prime[0][0])};
      if( pthread_create(&tid[j], NULL, mult_matrix, (void *)&args ) < 0){
        printf("\nError Creating Thread. Terminating Program");
        return -1;
      }
    }
    //wait for all i threads to terminate
    for(j = 0; j < i; j++){
      pthread_join(tid[j],NULL);
    }
    end_time = gettimeofday();
    
    //compare C to C_prime
    comp_error = 0;
    for(j = 0; j < n; j ++){
      for(k = 0; k < p; k++){
        if( C[j][k] != C_prime[j][k]){
          comp_error = 1;
          break;
        }
      }
      if(comp_error){
        break;
      }
    }
    
    printf("/n%.2f\t\t", (end_time - start_time));
    printf("\ncomparison: ");
    if(comp_error){ printf("Error\n\n"); }
    else{ printf("No Error\n\n"); }
  }
  
  return 0;
  
}

/*
  *Multiplies 1/num_threads of Matricies A and B.
  *
  *@num_threads: number of threads being used to compute the multiplication
  *@sequence: the ordered number of the thread computing the product
  *   i.e. 1st thread, 2nd thread, ... , nth thread in a n-thread product
  *   calculation.
  *@result_matrix: The matrix to store the result in.
  *
  *In order to yield a proper product of matrices A and B, @num_threads threads must
  *   call this function such that for threads 1 to @num_threads, 
  *   thread 1 has sequence number 1, thread 2 has sequence number 2, etc.
*/
void * mult_matrix( void *arguments){
  int i, j, k;
  struct arg_struc *args = (struct arg_struc*) arguments;
  
  for(i = args->seq_num; i < n; i += args->num_threads){
    for( j = 0; j < p; j++){
      *(args->result_matrix + i*n + j) = 0;
      
      for(k = 0; k < m; k++){
        *(args->result_matrix + i*n + j) += A[i][k] * B[k][j];
      }
    }
  }
  
  pthread_exit(NULL);
}

















