// Including necessary built-in libraries:
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#define BufferSize 100    // Defining Size of shared memory which is 100.

// Declaring Necessary items:
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

int *arr1dDynamic[100];   // This dynamic array is shared among Producer and Consumer. 
int arr2d[100][100];
int totalPrime = 0;
int out = 0;
int in = 0;

// Thread from Producer:
void *producer(void *pno){
    for(int i = 0; i < 100; i++) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        
        // Initializing Address of first item of each row of 2d array to dynamic 1d array:
    	arr1dDynamic[in] = &arr2d[i][0];
        printf("Producer %d: Insert Adress of Value: %d, From Row: %d\n\n", *((int *)pno),*arr1dDynamic[in],in);
        
        in = (in + 1) % BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

// Thread from Consumer:
void *consumer(void *cno){  
    for(int i = 0; i < 100; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int totalPrimeCon = 0;
        
        /*
           Printing all prime numbers in each row in this loop,
           out contains the row number on which currently working:
        */
        for(int j = 0; j < 100; j++){
            int n = *((arr1dDynamic[out]) + j);
            int flag = 0;
            
            if (n == 0 || n == 1)
               flag = 1;
            for (int k = 2; k <= n / 2; k++) {
                if (n % k == 0) {
                   flag = 1;
                   break;
                }
            }
            if (flag == 0){
               printf("%d ", n);
               totalPrimeCon += 1;
               totalPrime += 1;
            }
               
        }
        
        printf("\nConsumer %d: Finds Prime Number from Row: %d, Ttoal Prime Numbers Found in a Row: %d\n\n",*((int *)cno), out, totalPrimeCon);
        out = (out + 1) % BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main(){  
    // Initializing each row of 2d array from 0 to 99:    
    for(int i = 0; i < 100; i++){
    	for(int j = 0; j < 100; j++)
    	    arr2d[i][j] = j;
    }
    
    pthread_t pro[5], con[5];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BufferSize);
    sem_init(&full, 0, 0); 
    /*
    	proConNum array contains number of Producer and Consumers Created 
    	So that to print on output Screen Producer or Consumer Number:
    */
    int proConNum[2] = {1, 2};
    
    /*
    	Creating Threads for Producer and Consumer, 
    	Number of iterations of each loop will decide
    	how much producer and consumer has to be created,
    	Right now creating 2 Producers and 2 Consumers:
    */
    for(int i = 0; i < 2; i++) 
        pthread_create(&pro[i], NULL, (void *)producer, (void *)&proConNum[i]);
    for(int i = 0; i < 2; i++) 
        pthread_create(&con[i], NULL, (void *)consumer, (void *)&proConNum[i]);
        
    // Applying join so that program will not terminate before completion of thread:
    for(int i = 0; i < 2; i++)
        pthread_join(pro[i], NULL);
    for(int i = 0; i < 2; i++) 
        pthread_join(con[i], NULL);
    
    printf("Total Prime Number found in 2d-Array: %d\n\n", totalPrime);
    // Destroying empty, full and mutex lock created before:
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);   
    return 0;
}
