//Sleeping Teaching Assistant Problem
#include <pthread.h>		//Create POSIX threads.
#include <time.h>			//Wait for a random time.
#include <unistd.h>			//Thread calls sleep() for specified number of seconds.
#include <semaphore.h>		//To create semaphores
#include <stdlib.h>			
#include <stdio.h>	
#include <string.h>

const int number_of_chairs = 3;
const int maximum_programming_time = 7;
const int TA_student_teaching_time = 5;

//Declaration of Mutex Locks
pthread_mutex_t ChairAccess;

//Declaration of Semaphores
sem_t Chairs_Sem[3];   //An array of 3 semaphores to signal and wait chair to wait for the TA
sem_t TA_Sleep;        //A semaphore to signal and wait TA's sleep
sem_t Student_Sem;     //A semaphore to signal and wait for TA's next student

pthread_t *Students;    //N threads running as Students.
pthread_t TA;			//A separate Thread for TA.


//Declaration of Functions
void *TA_Activity();
void *Student_Activity(void *threadID);

int ChairsCount = 0;
int CurrentIndex = 0;

void delay(float number_of_seconds)
{
    int ms = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while (clock() < start_time + ms);
}

int main()
{
	int number_of_students;		
	//srand() is to prevent sequence repetition between runs
	srand(time(NULL));

	//Initializing Mutex Lock and Semaphores.
	sem_init(&TA_Sleep, 0, 0);
	sem_init(&Student_Sem, 0, 0);
	for(int id = 0; id < number_of_chairs; id++)
	{
		sem_init(&Chairs_Sem[id], 0, 0);
	}

	pthread_mutex_init(&ChairAccess, NULL);
	
	printf("Enter the number of students : ");
	scanf("%d",&number_of_students);
	
	printf("********************************************************\n");
	char *welcome_str = "***********WELCOME TO JOY'S PROGRAMMING CLASS***********";
	for(int i=0;i<strlen(welcome_str);i++)
	{
	    printf("%c",welcome_str[i]);
	    delay(1);
	}
	printf("\n********************************************************\n");
	printf("LET US SEE WHAT THE STUDENTS ARE DOING!!\n\n");
	
	//Allocate memory for Students
	Students = (pthread_t*) malloc(sizeof(pthread_t)*number_of_students);

	//Creating TA thread and N Student threads.
	pthread_create(&TA, NULL, TA_Activity, NULL);	
	for(int id = 0; id < number_of_students; id++)
		pthread_create(&Students[id], NULL, Student_Activity,(void*)(long)id);

	//Waiting for TA thread and N Student threads.
	pthread_join(TA, NULL);
	for(int id = 0; id < number_of_students; id++)
	{
		pthread_join(Students[id], NULL);
	}

	free(Students); 
	return 0;
}

void *TA_Activity()
{
	while(1)
	{
	    //TA is currently sleeping
		sem_wait(&TA_Sleep);     
		printf("**********Student wakes up the TA.**********\n");
		while(1)
		{
			pthread_mutex_lock(&ChairAccess);
			
			//CRITICAL SECTION BEGINS
			if(ChairsCount == 0) 
			{
				//If chairs are empty, break the loop and allow the next student to enter
				pthread_mutex_unlock(&ChairAccess);
				break;
			}
			//TA gets next student on chair.
			sem_post(&Chairs_Sem[CurrentIndex]);
			ChairsCount--;
			printf("Student left his/her chair.\nRemaining Chairs :  %d\n", 3 - ChairsCount);
			CurrentIndex = (CurrentIndex + 1) % 3;
			//CRITICAL SECTION ENDS
			
			pthread_mutex_unlock(&ChairAccess);

			printf(":::::::::TA is currently helping the student!:::::::::\n");
			sleep(TA_student_teaching_time);
			sem_post(&Student_Sem);
			sleep(1000);
		}
	}
}

void *Student_Activity(void *threadID) 
{

	while(1)
	{
		printf("Student %ld is doing programming assignment for %d minutes.\n", (long)threadID,maximum_programming_time);
		sleep(maximum_programming_time);	

		printf("Student %ld needs help from the TA\n", (long)threadID);
		
		pthread_mutex_lock(&ChairAccess);
		int count = ChairsCount;
		pthread_mutex_unlock(&ChairAccess);

		if(count < 3)		//Student tried to sit on a chair.
		{
			if(count == 0)		//If student sits on first empty chair, he/she wakes up the TA.
				sem_post(&TA_Sleep);
			else
				printf("Student %ld sat on a chair, waiting for the TA to finish.\n", (long)threadID);

			// lock
			pthread_mutex_lock(&ChairAccess);
			
			int index = (CurrentIndex + ChairsCount) % 3;
			ChairsCount++;
			printf("Student sat on the chair.\nRemaining Chairs : %d\n", 3 - ChairsCount);
			
			pthread_mutex_unlock(&ChairAccess);
			// unlock

			sem_wait(&Chairs_Sem[index]);		//Student leaves his/her chair.
			printf("\t Student %ld is getting help from the TA. \n", (long)threadID);
			sem_wait(&Student_Sem);		//Student waits to go next.
			printf("Student %ld left TA room.\n",(long)threadID);
		}
		else 
		{
		    //If student doesn't find any chair to sit on
			printf("Student %ld will return at another time for help.\n", (long)threadID);
		}
			
	}
}