#include "ssem.h"
#include "sshm.h"
#include <stdio.h>

//code of Proc1
int main(){
  int i, internal_reg;

/* here create and initialize all semaphores */
  int s1 = sem_create(66, 0);
  int s2 = sem_create(67, 0);
  int s3 = sem_create(68, 0);
  int s4 = sem_create(69, 0);
  
/* here created: shared memory array Account of size 3 */
  int *Account;
  shm_get(1256, (void**)&Account, 3*sizeof(int));
  Account[0]=10000;
  Account[1]=10000;
  Account[2]=10000;
  /* synchronize with Proc2, Proc3 and Proc4 (4 process 4 way synchronization)*/
  sem_signal(s2);
  sem_signal(s3);
  sem_signal(s4);
  sem_wait(s1);
  sem_wait(s1);
  sem_wait(s1);
  
  for (i = 0; i < 2000; i++){
      if( (i%600) == 0 && i != 0){
        usleep(200);
      }
      
      if(i % 100 == 0){
        printf("\nFrom Process1 \n Account1: %d Account2: %d Account3: %d", Account[0], Account[1], Account[2]);
        printf("\nAccounts sum: %d", (Account[0] + Account[1] + Account[2]));
      }
      // Remove $200 from account 1 for transfer.
      //TODO: Sync Point with process 3
      
      sem_wait(s1); //is p3 modifying?
      sem_wait(s2);
      internal_reg = Account [0];
      internal_reg = internal_reg - 200;
      Account[0] = internal_reg;
      sem_signal(s1); //p1 done modifying.

      // Transferring $200 to account 2.
      //TODO: Sync Point with process 2
      internal_reg = Account [1];
      internal_reg = internal_reg + 200;
      Account[1] = internal_reg;
      sem_signal(s1);
      sem_signal(s2);
  }
  
  sem_signal(s4);
  
  return 0;
}