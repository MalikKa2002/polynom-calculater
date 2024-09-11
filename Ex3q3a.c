#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <stdbool.h>
#include<fcntl.h>

#define MAX_INPUT_LENGTH 128
#define MAX_POLYNOMS 10


int main()
{
    char user_in[MAX_INPUT_LENGTH];
    int shmid;
    key_t key;
    // Declare the shared memory as an array of strings
    char (*sharedMemory)[MAX_INPUT_LENGTH];
    sem_t* empty;
    sem_t* mutex;
    sem_t* full;
    int m =0;


     key = ftok("SHARED_MEMORY_KEY", 1234);

     // Create the shared memory segment
     if ((shmid=shmget(key,MAX_INPUT_LENGTH*MAX_POLYNOMS,IPC_CREAT|0666))==-1)
     {
        perror("faild to get memory....");
        exit(1);
     }

  // Attach to the shared memory segment
sharedMemory = shmat(shmid, NULL, 0);
if (sharedMemory == (char (*)[MAX_INPUT_LENGTH]) -1) {
    perror("failed to attach memory...");
    exit(1);
}

    empty = sem_open("/empty_semaphore", O_CREAT, 0644, (MAX_POLYNOMS-1));
    full = sem_open("/full_semaphore", O_CREAT, 0644, 0);

    if (full == SEM_FAILED)
    {
        perror("parent sem_open failed");
    }
    if (empty == SEM_FAILED)
    {
        perror("parent sem_open failed");
    }




    while (1) {
        
        sem_wait(empty);
        printf("Enter a two polynoms and between of them one of (ADD,SUB,MUL): ");
        // Read the user input
        fgets(user_in,MAX_INPUT_LENGTH,stdin);
        // Remove the newline character
        user_in[strcspn(user_in, "\n")] = '\0';
        strcpy(sharedMemory[m++], user_in);
        sem_post(full);

        if (strcmp(user_in, "END") == 0) {
            break;
        }
        else if (strlen(user_in) > MAX_INPUT_LENGTH + 1) {
            printf("Input exceeds maximum length of %d characters.\n", MAX_INPUT_LENGTH);
        }

    }


        sem_close(empty);
        sem_close(full);
      

        sem_unlink("/empty_semaphore");
        sem_unlink("/full_semaphore");

        shmdt(sharedMemory);
        shmctl(shmid,IPC_RMID,NULL);






    return 0;
}

