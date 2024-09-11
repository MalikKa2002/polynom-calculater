#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <semaphore.h>
#include<fcntl.h>
#include <pthread.h>

#define MAX_INPUT_LENGTH 128
#define MAX_POLYNOMS 10
#define MAX_DEGREE 120


typedef struct {
    int degree;
    int coefficients[MAX_DEGREE];
} Polynom;

typedef struct {
    int *arr1;
    int *arr2;
    int *result;
    int start;
    int end;
} ThreadData;

pthread_mutex_t mutex;

void* threadAdd(void* arg);
bool checkSubstring(const char* string, const char* substring);
Polynom addPolynom(Polynom p1, Polynom p2);
Polynom subtractPolynom(Polynom p1, Polynom p2);
Polynom multiplyPolynom(Polynom p1, Polynom p2);
void separateStrings(const char* input, char* firstString, char* secondString);
void strtokString(char *input_string, char **strings, int max_strings);
Polynom PolynomCalculate(Polynom p1, Polynom p2,Polynom (*callback)(Polynom,Polynom));
void printPolynom(Polynom p);


int main()
{
 
    Polynom p1, p2, result;
    int shmid;
    key_t key;
   char (*sharedMemory)[MAX_INPUT_LENGTH];
    sem_t* empty;
    sem_t* full;
    int n= 0 ;
   



    key = ftok("SHARED_MEMORY_KEY", 1234);
    if ((shmid=shmget(key,MAX_INPUT_LENGTH*MAX_POLYNOMS,IPC_CREAT|0644))==-1){
        perror("faild to get memory ");
        exit(1);
    }

    sharedMemory = shmat(shmid, NULL, 0);
    if (sharedMemory == (char (*)[MAX_INPUT_LENGTH]) -1) {
        perror("failed to attach memory...");
        exit(1);
    }


 
     empty = sem_open("/empty_semaphore", 0);
    full = sem_open("/full_semaphore", 0);

    if (full == SEM_FAILED)
    {
        perror("parent sem_open failed");
    }
    if (empty == SEM_FAILED)
    {
        perror("parent sem_open failed");
    }
    while (1) {
        
        sem_wait(full);
        char temp[MAX_INPUT_LENGTH] = "\0";
        strcpy(temp, sharedMemory[n++]);
        sem_post(empty);
        if (checkSubstring(temp,"END"))
       {
        break;
       }
       
        char firstString[MAX_INPUT_LENGTH] = "\0";
        char secondString[MAX_INPUT_LENGTH] = "\0";
        char *result1[100];
        char *result2[100];
        separateStrings(temp, firstString, secondString);
        strtokString(firstString, result1, 100);
        strtokString(secondString, result2, 100);
        p1.degree = atoi(result1[0]);
        p2.degree = atoi(result2[0]);
        int maxSize = (p1.degree > p2.degree) ? p1.degree : p2.degree;
        int j = p1.degree;
           for (int i = 1; i <= (p1.degree + 1); i++) {//to take the coefficients of the first polynom
               p1.coefficients[j] = atoi(result1[i]);
               j--;
               if (j < 0) {
                   break;
               }
           }
           j = p2.degree;
           for (int i = 1; i <= (p2.degree + 1); i++) {//to take the coefficients of the second polynom
               p2.coefficients[j] = atoi(result2[i]);
               j--;
               if (j < 0) {
                   break;
               }
           }
         if (checkSubstring(temp, "MUL"))
        {
            printPolynom(PolynomCalculate(p1,p2,multiplyPolynom));
        }
        else{
        if (checkSubstring(temp,"ADD"))
        {
        
        pthread_t threads[maxSize];
        ThreadData threadData[maxSize];
        pthread_mutex_init(&mutex, NULL);
        for (int i = 0; i < maxSize; i++) {
        threadData[i].arr1 = p1.coefficients;
        threadData[i].arr2 = p2.coefficients;
        threadData[i].result = result.coefficients;
        threadData[i].start = i;
        threadData[i].end = i + 1;

        if (i < p1.degree && i < p2.degree) {
            pthread_create(&threads[i], NULL, threadAdd, (void*)&threadData[i]);
        } else if (i < p1.degree) {
            threadData[i].arr2 = NULL;
            pthread_create(&threads[i], NULL, threadAdd, (void*)&threadData[i]);
        } else if (i < p2.degree) {
            threadData[i].arr1 = NULL;
            pthread_create(&threads[i], NULL, threadAdd, (void*)&threadData[i]);
        }
    }
     for (int i = 0; i < maxSize; i++) {
        if (i < p1.degree && i < p2.degree) {
            pthread_join(threads[i], NULL);
        }
    }
    printPolynom(result);

     pthread_mutex_destroy(&mutex);
    
    }
  
    
   
    
    
    else if (checkSubstring(temp,"SUB"))
    {
       printPolynom(result);
    }
        
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
void* threadAdd(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    for (int i = data->start; i < data->end; i++) {
        pthread_mutex_lock(&mutex);
        data->result[i] = data->arr1[i] + data->arr2[i];
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void printPolynom(Polynom p) {//print polynoms
    int hasTerms = 0;
    for (int i = p.degree; i >= 0; i--) {
        if (p.coefficients[i] != 0) {
             hasTerms = 1;
            if (i == p.degree) {
                if (i==1)
                {
                  printf("%dx", p.coefficients[i]);
                }
                else
                     printf("%dx^%d", p.coefficients[i], i);
               
            } else if (i == 0) {
                if (p.coefficients[i] < 0) {
                    printf(" - %d", -p.coefficients[i]);
                } else {
                    printf(" + %d", p.coefficients[i]);
                }
            } else {
                if (p.coefficients[i] < 0) {
                    if (i!=1)
                    {
                        printf(" - %dx^%d", -p.coefficients[i], i);
                    }
                    else{
                        printf(" - %dx", -p.coefficients[i]);
                    }
                } else {
                       if (i!=1)
                    {
                        printf(" + %dx^%d", p.coefficients[i], i);
                    }
                    else{
                        printf(" + %dx", p.coefficients[i]);
                    }
                    
                }
            }
        }
    }
    if (!hasTerms) {
        printf("0");
    }
    printf("\n");
}



//to check if the string have sub strings of "ADD","SUB","MUL"
bool checkSubstring(const char* string, const char* substring) {
    if (string == NULL || substring == NULL) {
        return false; // Invalid input
    }

    size_t stringLength = strlen(string);
    size_t substringLength = strlen(substring);

    if (substringLength > stringLength) {
        return false; // Substring is longer than the string
    }

    for (size_t i = 0; i <= stringLength - substringLength; i++) {
        if (strncmp(string + i, substring, substringLength) == 0) {
            return true; // Substring found
        }
    }

    return false; // Substring not found
}
Polynom multiplyPolynom(Polynom p1, Polynom p2) {//multiply of polynom
    int resultDegree = p1.degree + p2.degree;
    Polynom result;
    result.degree = resultDegree;
    for (int i = 0; i < resultDegree; i++)
    {
        result.coefficients[i]=0;
    }
    

    int i, j;
    for (i = 0; i <= p1.degree; i++) {
        for (j = 0; j <= p2.degree; j++) {
            result.coefficients[i + j] += p1.coefficients[i] * p2.coefficients[j];
        }
    }

    return result;
}
   


void separateStrings(const char* input, char* firstString, char* secondString) {//to get the string between the parentheses in our case ther is two
    int i = 0;
    int len = strlen(input);

    // Find the first string enclosed in parentheses
    while (i < len && input[i] != '(') {
        i++;
    }
    if (input[i] == '(') {
        int j = 0;
        i++;
        while (i < len && input[i] != ')') {
            firstString[j] = input[i];
            i++;
            j++;
        }
        firstString[j] = '\0';
    }

    // Find the second string after the first string
    while (i < len && (input[i] != '(' || input[i] == ')')) {
        i++;
    }
    if (input[i] == '(') {
        int j = 0;
        i++;
        while (i < len && input[i] != ')') {
            secondString[j] = input[i];
            i++;
            j++;
        }
        secondString[j] = '\0';
    }
}

void strtokString(char *input_string, char **strings, int max_strings) {//strtok on (:)and (,)
    char *token;
    int i = 0;

    token = strtok(input_string, ":");
    while (token != NULL && i < max_strings) {
        strings[i++] = token;
        token = strtok(NULL, ",");
    }

}

Polynom PolynomCalculate(Polynom p1, Polynom p2,Polynom (*callback)(Polynom,Polynom)){//callback function
    return callback(p1,p2);
}