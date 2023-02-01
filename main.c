#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <stdbool.h>

#define NUM 50 // Variable that decides how many car need to produce.

// Variables that decide how many threads(workers) are created for specific type.
#define NUM_THREAD_TYPE_A 2 
#define NUM_THREAD_TYPE_B 2
#define NUM_THREAD_TYPE_C 2
#define NUM_THREAD_TYPE_D 2
 

// Variable to check how many car produced.
int total_cars=0;
// ------------------------------------------------- Synch. Elements Definitions -------------------------------------------------
pthread_mutex_t chassismutex[NUM];
pthread_mutex_t seatmutex[NUM];
pthread_mutex_t enginemutex[NUM];
pthread_mutex_t tiremutex[NUM];

sem_t chassis_sem[NUM];
sem_t seat_sem[NUM];
sem_t tire_sem[NUM];
sem_t paint_sem[NUM];

// ------------------------------------------------- Car Sturcture and Array Definitions -------------------------------------------------

// For the project, car is designed as a struct that have desired features(e.g. tires, chassis, engine, etc.)
struct car {
    int chassis;
    int tires;
    int seats;
    int engine;
    int top_cover;
    int painting;
};
// This array below, represents a unconstructed car array. All elements/integers in it, initialized as 0 in the main function.
struct car cars[NUM];

// ------------------------------------------------- B Type Thread Routine -------------------------------------------------
/*
In this method:
 - There is a thread id information in arg of the routine function.
 - for loop helps to iterate through the array of unconstructed cars. In other words, car array which has all features(e.g. chassis, tires, seats, etc.) are 0.
 - The function uses 'pthread_mutex_trylock' to prevent multiple threads of type B from waiting for the same mutex. Which means, more than 1 threads of B dont wait for same car/same index of array.
 - Since threads work concurrently on the car array, function contains a if statement to check whether same type of thread already completed the job or not.
 - Job will be done by changing current car's related element. For this routine, it is 'chassis'.
*/
void* type_b_technician(void* arg) {
    sleep(2); // sleep for observing concurrency better.
    int id = *(int*)arg;
    printf("[ THREAD INFO ] ---> ID=%d B TYPE THREAD CAME\n",id);
    int i = 0;
    for (i=0;i<NUM;i++){
        if (pthread_mutex_trylock(&chassismutex[i]) == 0) {
            
            if (cars[i].chassis == 1){
                pthread_mutex_unlock(&chassismutex[i]);
                continue;}
            
            cars[i].chassis=1;
            sleep(3); // sleep to indicate the time that take to complete worker's job.
            printf("( JOB ) ---> __CAR_ID=%d__   __CHASSIS__   __B TYPE THREAD_ID=%d__\n",i,id);
            
            sem_post(&chassis_sem[i]);
            sem_post(&chassis_sem[i]);
            sem_post(&chassis_sem[i]);
            
            pthread_mutex_unlock(&chassismutex[i]);
        }else{
           // Thread tried to lock but it could not. Therefore, skips the other index.
        }
    }
    return NULL;
}

// ------------------------------------------------- A Type Thread Routine -------------------------------------------------
/*
In this method:
 - There is a thread id information in arg of the routine function.
 - for loop helps to iterate through the array of unconstructed cars. In other words, car array which has all features(e.g. chassis, tires, seats, etc.) are 0.
 - The function uses 'pthread_mutex_trylock' to prevent multiple threads of type A from waiting for the same mutex. Which means, more than 1 threads of A dont wait for same car/same index of array.
 - Since threads work concurrently on the car array, function contains a if statement to check whether same type of thread already completed the job or not.
 - Job will be done by changing current car's related element/elements. For this routine, the job is completing 'tire' and 'paint'.
*/
void* type_a_technician(void* arg) {
    sleep(2); // sleep for observing concurrency better.
    int id = *(int*)arg;
    printf("[ THREAD INFO ] ---> ID=%d A THREAD CAME \n",id);
    int i = 0;
    for (i=0;i<NUM;i++){
        if (pthread_mutex_trylock(&tiremutex[i]) == 0){

            if(cars[i].tires == 1){
                pthread_mutex_unlock(&tiremutex[i]);
                continue;
            }
            
            sem_wait(&chassis_sem[i]);
            sem_wait(&seat_sem[i]);

            cars[i].tires=1;
            sleep(3); // sleep to indicate the time that take to complete worker's job.
            printf("( JOB ) ---> __CAR_ID=%d__   __TIRES__   __A TYPE THREAD_ID=%d__\n",i,id);

            sem_post(&tire_sem[i]);
            sem_wait(&paint_sem[i]);

            cars[i].painting=1;
            sleep(3); // sleep to indicate the time that take to complete worker's job.
            printf("( JOB ) ---> __CAR_ID=%d__   __PAINT__   __A TYPE THREAD_ID=%d__\n",i,id);

            total_cars++;
            printf("( FINAL ) ---> %d Amount of car produced\n",total_cars);

            pthread_mutex_unlock(&tiremutex[i]);
        }else{
            // Thread tried to lock but it could not. Therefore, skips the other.
        }
        
    }
    return NULL;
}

// ------------------------------------------------- C Type Thread Routine -------------------------------------------------
/*
In this method:
 - There is a thread id information in arg of the routine function.
 - for loop helps to iterate through the array of unconstructed cars. In other words, car array which has all features(e.g. chassis, tires, seats, etc.) are 0.
 - The function uses 'pthread_mutex_trylock' to prevent multiple threads of type C from waiting for the same mutex. Which means, more than 1 threads of C dont wait for same car/same index of array.
 - Since threads work concurrently on the car array, function contains a if statement to check whether same type of thread already completed the job or not.
 - Job will be done by changing current car's related element/elements. For this routine, it is 'seat'.
*/
void* type_c_technician(void* arg) {
    sleep(2); // sleep for observing concurrency better.
    int id = *(int*)arg;
    printf("[ THREAD INFO ] ---> ID=%d C THREAD CAME \n",id);
    int i = 0;
    for (i=0;i<NUM;i++){
        if (pthread_mutex_trylock(&seatmutex[i]) == 0){

            if(cars[i].seats == 1){
                pthread_mutex_unlock(&seatmutex[i]);
                continue;
            }

            sem_wait(&chassis_sem[i]);
            
            cars[i].seats=1;
            sleep(3); // sleep to indicate the time that take to complete worker's job.
            printf("( JOB ) ---> __CAR_ID=%d__   __SEATS__   __C TYPE THREAD_ID=%d__\n",i,id);

            sem_post(&seat_sem[i]);
            sem_post(&seat_sem[i]);

            pthread_mutex_unlock(&seatmutex[i]);
        }else{
            // Thread tried to lock but it could not. Therefore, skips the other.
        }
    }
    return NULL;
}

// ------------------------------------------------- D Type Thread Routine -------------------------------------------------
/*
In this method:
 - There is a thread id information in arg of the routine function.
 - for loop helps to iterate through the array of unconstructed cars. In other words, car array which has all features(e.g. chassis, tires, seats, etc.) are 0.
 - The function uses 'pthread_mutex_trylock' to prevent multiple threads of type D from waiting for the same mutex. Which means, more than 1 threads of D dont wait for same car/same index of array.
 - Since threads work concurrently on the car array, function contains a if statement to check whether same type of thread already completed the job or not.
 - Job will be done by changing current car's related element/elements. For this routine, the job is completing 'engine' and 'top cover'.
*/
void* type_d_technician(void* arg) {
    sleep(2); // sleep for observing concurrency better.
    int id = *(int*)arg;
    printf("[THREAD INFO ] --->  ID=%d D THREAD CAME \n",id);
    int i = 0;
    for (i=0;i<NUM;i++){
        if (pthread_mutex_trylock(&enginemutex[i]) == 0){

            if(cars[i].engine == 1){
                pthread_mutex_unlock(&enginemutex[i]);
                continue;
            }

            sem_wait(&chassis_sem[i]);
            sem_wait(&seat_sem[i]);
            sem_wait(&tire_sem[i]);

            cars[i].engine=1;
            sleep(3); // sleep to indicate the time that take to complete worker's job.
            printf("( JOB ) ---> __CAR_ID=%d__   __ENGINE__   __D TYPE THREAD_ID=%d__\n",i,id);

            cars[i].top_cover=1;
            sleep(3); // sleep to indicate the time that take to complete worker's job.
            printf("( JOB ) ---> __CAR_ID=%d__   __TOP COVER__   __D TYPE THREAD_ID=%d__\n",i,id);

            sem_post(&paint_sem[i]);

            pthread_mutex_unlock(&enginemutex[i]);
        }else{
            // Thread tried to lock but it could not. Therefore, skips the other.
        }

    }
    return NULL;
}

// ------------------------------------------------- Main Method -------------------------------------------------
int main() {
    
    pthread_t type_a_threads[NUM_THREAD_TYPE_A];
    pthread_t type_b_threads[NUM_THREAD_TYPE_B];
    pthread_t type_c_threads[NUM_THREAD_TYPE_C];
    pthread_t type_d_threads[NUM_THREAD_TYPE_D];

//---------------------------------------> INITIALIZE THE CARS
    int j;

    // Initialize the cars array
    for (j = 0; j < NUM; j++) {
        // Initialize fields of the car
        cars[j].chassis = 0;
        cars[j].tires = 0;
        cars[j].seats = 0;
        cars[j].engine = 0;
        cars[j].top_cover = 0;
        cars[j].painting = 0;
    }
//--------------------------------------->  CREATE THREADS
int i=0;
    for ( i = 0; i < NUM_THREAD_TYPE_A; i++) {
        int* x = malloc(sizeof(int));
        *x = i;
        pthread_create(&type_a_threads[i], NULL, type_a_technician, x);
    }
    for ( i = 0; i < NUM_THREAD_TYPE_C; i++) {
        int* z = malloc(sizeof(int));
        *z = i;
        pthread_create(&type_c_threads[i], NULL, type_c_technician, z);
    }
    for ( i = 0; i < NUM_THREAD_TYPE_D; i++) {
        int* t = malloc(sizeof(int));
        *t = i;
        pthread_create(&type_d_threads[i], NULL, type_d_technician, t);
    }
    for ( i = 0; i < NUM_THREAD_TYPE_B; i++) {
        int* y = malloc(sizeof(int));
        *y = i;
        pthread_create(&type_b_threads[i], NULL, type_b_technician, y);
    }

//--------------------------------------->  CREATE MUTEX AND SEMAPHORES
    for ( i = 0; i < NUM; i++) {
        pthread_mutex_init(&chassismutex[i], NULL);
    }for ( i = 0; i < NUM; i++) {
        pthread_mutex_init(&seatmutex[i], NULL);
    }
    for ( i = 0; i < NUM; i++) {
        pthread_mutex_init(&tiremutex[i], NULL);
    }for ( i = 0; i < NUM; i++) {
        pthread_mutex_init(&enginemutex[i], NULL);
    }

    for ( i = 0; i < NUM; i++) {
        sem_init(&chassis_sem[i], 0, 0);
    }
    for ( i = 0; i < NUM; i++) {
        sem_init(&seat_sem[i], 0, 0);
    }
    for ( i = 0; i < NUM; i++) {
        sem_init(&tire_sem[i], 0, 0);
    }
    for ( i = 0; i < NUM; i++) {
        sem_init(&paint_sem[i], 0, 0);
    }
//--------------------------------------->  JOIN THREAD
    for ( i = 0; i < NUM_THREAD_TYPE_B; i++) {
                pthread_join(type_b_threads[i], NULL);
    }
    for ( i = 0; i < NUM_THREAD_TYPE_D; i++) {
                pthread_join(type_d_threads[i], NULL);
    }
    for ( i = 0; i < NUM_THREAD_TYPE_A; i++) {
                pthread_join(type_a_threads[i], NULL);
    }
    for ( i = 0; i < NUM_THREAD_TYPE_C; i++) {
                pthread_join(type_c_threads[i], NULL);
    }
//--------------------------------------->  DESTROY MUTEX AND SEMAPHORES 
    for ( i = 0; i < NUM; i++) {
        pthread_mutex_destroy(&chassismutex[i]);
    }for ( i = 0; i < NUM; i++) {
        pthread_mutex_destroy(&seatmutex[i]);
	}
    for ( i = 0; i < NUM; i++) {
        pthread_mutex_destroy(&tiremutex[i]);
    }for ( i = 0; i < NUM; i++) {
        pthread_mutex_destroy(&enginemutex[i]);
    }
    for ( i = 0; i < NUM; i++) {
        sem_destroy(&chassis_sem[i]);
    }
    for ( i = 0; i < NUM; i++) {
        sem_destroy(&seat_sem[i]);
    }
    for ( i = 0; i < NUM; i++) {
        sem_destroy(&tire_sem[i]);
    }
    for ( i = 0; i < NUM; i++) {
        sem_destroy(&paint_sem[i]);
    }
    
    // Information log.
    for ( i = 0; i < NUM; i++) {
        printf("Car %d:\n", i);
        printf("  Chassis: %d\n", cars[i].chassis);
        printf("  Tires: %d\n", cars[i].tires);
        printf("  Seats: %d\n", cars[i].seats);
        printf("  Engine: %d\n", cars[i].engine);
        printf("  Top Cover: %d\n", cars[i].top_cover);
        printf("  Painting: %d\n", cars[i].painting);
    }

    return 0;
}
