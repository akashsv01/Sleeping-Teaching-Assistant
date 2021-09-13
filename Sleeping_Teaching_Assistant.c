//Sleeping Teaching Assistant Problem
#include <pthread.h>		//Create POSIX threads.
#include <time.h>			//Wait for a random time.
#include <unistd.h>			//Thread calls sleep() for specified number of seconds.
#include <semaphore.h>		//To create semaphores
#include <stdlib.h>			
#include <stdio.h>	
#include <string.h>

const int number_of_chairs = 3;

//Declaration of Mutex Lock
pthread_mutex_t ChairAccess;

//Declaration of Semaphores
sem_t TA_Sem;          //A semaphore to signal and wait TA's activities
sem_t Student_Sem;     //A semaphore to signal and wait for Student's activities

pthread_t *Students;    //N threads running as Students.
pthread_t TA;			//A separate Thread for TA.


//Declaration of Functions
int isWaiting(int student_id);  //A function to check if the student is waiting in the hallway or not
void *TA_Activity();            //A function to run the activites of TA
void *Student_Activity(void *threadID);  //A function to run the activites of Students

int Waiting_Room_Chairs[3];       
int Number_Students_Waiting = 0;
int Next_Seating_Position = 0;
int Next_Teaching_Position = 0;
int TA_Sleep = 0;

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
	sem_init(&TA_Sem, 0, 1);
	sem_init(&Student_Sem, 0, 0);
	
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
	
	int student_ids[number_of_students];
	//Allocate memory for Students
	Students = (pthread_t*) malloc(sizeof(pthread_t)*number_of_students);

	//Creating TA thread and N Student threads.
	pthread_create(&TA, NULL, TA_Activity, NULL);	
	for(int id = 0; id < number_of_students; id++)
	{
	    student_ids[id] = id+1;
		pthread_create(&Students[id], NULL, Student_Activity,(void*)&student_ids[id]);
	}

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
	printf("Checking whether students are waiting for help......\n" );
	while(1) 
	{
		//Check if students are waiting
		if(Number_Students_Waiting > 0 ) 
		{
			TA_Sleep = 0;
			sem_wait(&Student_Sem);
			
			pthread_mutex_lock(&ChairAccess);

            //CRITICAL SECTION BEGINS
			int TA_student_teaching_time = (rand()%5) + 1;   //Bwtween 1 and 5 seconds

			//TA helping the student with the programming assignment
			printf("\nTA helping a student for %d seconds with the programming assignment.",TA_student_teaching_time);
			printf("\nStudents waiting to receive help = %d\n",(Number_Students_Waiting-1));
			printf("Student %d receiving help from the TA.\n",Waiting_Room_Chairs[Next_Teaching_Position]);

			Waiting_Room_Chairs[Next_Teaching_Position] = 0;
			Number_Students_Waiting--;
			Next_Teaching_Position = (Next_Teaching_Position + 1 ) % number_of_chairs;

			sleep(TA_student_teaching_time);
			//CRITICAL SECTION ENDS

			pthread_mutex_unlock(&ChairAccess);
			sem_post(&TA_Sem);
		}
		
		//if none of the students are waiting for help
		else
		{
			if(TA_Sleep==0)
			{
				printf("No student is waiting in the hallway.\n\nTA is sleeping.....\n\n" );
				TA_Sleep = 1;
			}
		}
	}
	
}

void *Student_Activity(void *threadID) 
{
    int ID_Of_Student = *(int*)threadID;

	while(1) 
	{
		//If student is waiting in the hallway, then he/she continues to wait
		if (isWaiting(ID_Of_Student) == 1 ) 
		{ 
		    continue; 
		}

		//Now, the student is doing programming assignment.
		int programming_time = (rand() % 5) + 1;
		printf("Student %d is programming for %d seconds.\n",ID_Of_Student, programming_time);
		sleep(programming_time);

		pthread_mutex_lock(&ChairAccess);

        //CRITICAL SECTION BEGINS
		if(Number_Students_Waiting < number_of_chairs) 
		{
			Waiting_Room_Chairs[Next_Seating_Position] = ID_Of_Student;
			Number_Students_Waiting++;

			//Student takes a seat in the hallway
			printf("Student %d takes a seat in the hallway.\nStudents waiting to receive help = %d\n",ID_Of_Student,Number_Students_Waiting);
			Next_Seating_Position = (Next_Seating_Position + 1 ) % number_of_chairs;
            //CRITICAL SECTION ENDS
            
			pthread_mutex_unlock(&ChairAccess);

			//Student needs to wake the TA if the TA is sleeping
			sem_post(&Student_Sem);
			sem_wait(&TA_Sem);

		}
		else 
		{
		    //If Number_Students_Waiting>= number_of_chairs, then this loop will be executed
		    
            //CRITICAL SECTION ENDS
			pthread_mutex_unlock(&ChairAccess);

			//No chairs available. Student will try later.
			printf("Student %d will return at another time for help.\n",ID_Of_Student);
		}
	}

}

int isWaiting(int student_id) 
{
	for(int i=0;i<number_of_chairs;i++) 
	{
		if (Waiting_Room_Chairs[i]==student_id ) 
		{ 
		    return 1; 
		}
	}
	return 0;
}
