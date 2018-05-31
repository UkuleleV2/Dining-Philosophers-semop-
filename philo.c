#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <signal.h>
#include <wait.h>
#define N 5
#define RIGHT_FORK ( left_fork_id + 1 ) % N

int phil_id,sem_id;
int arr[N];
pid_t pid;
int status;
static volatile sig_atomic_t interrupt_flag = 0;
static void set_interrupt_flag(int signo) 
{
	interrupt_flag = 1;
}
void grab_forks(int left_fork_id)
{
	int right_fork_id = RIGHT_FORK;
	struct sembuf semaphore[2] = 
	{
	        {right_fork_id, -1, 0},
	        {left_fork_id, -1, 0}
	};

	if (semop(sem_id, semaphore, 2) == -1)
	{
		printf("[%d], Oh, the dinner is over \n",phil_id);
	}

	printf("[%d]: I am taking: %d and: %d.\n", phil_id,left_fork_id,right_fork_id);
}
void put_away_forks(int left_fork_id)
{
	int right_fork_id = RIGHT_FORK;
	struct sembuf semaphore[2] = 
	{
	        {right_fork_id, 1, 0},
	        {left_fork_id, 1, 0}
	};

	if (semop(sem_id, semaphore, 2) == -1)
	{
		printf("[%d], Oh, the dinner is over \n",phil_id);
	}

	printf("[%d]: I am putting away: %d and: %d.\n", phil_id,left_fork_id,right_fork_id);
}

void eat()
{
	printf("[%d], I am eating\n",phil_id);
	sleep(1);
}	
void think()
{
	printf("[%d], I am thinking\n",phil_id);
	sleep(2);
}


void philosopher(int i)
{
	while (1)
        {

		think();
		sleep(1);
		grab_forks(i);
		sleep(0);
		eat();
		put_away_forks(i);
		if(interrupt_flag) 
		{
			while ((pid = wait(&status)) != -1) {}
		        kill(getpid(), SIGTERM);
		}
        }
}



int main()
{
	int status;
	signal(SIGINT, set_interrupt_flag);
	//getting semaphore set ID
	if ((sem_id = semget(IPC_PRIVATE, N, 0644 | IPC_CREAT)) == -1) 
	{
		perror("Cannot get semaphore set id\n");
		exit(1);
	}
	//setting up/
	for (int i = 0; i < N; i++)
	{
		if (semctl(sem_id, i, SETVAL, 1) == -1)
			{
				printf("Cannot set semaphore\n");
				return 1;
			}
	}    
		

	for (int i = 0; i < N; i++)
	{
		if ((pid == fork())== 0)
		{	
			arr[i] = getpid();
			phil_id = i;
			philosopher(phil_id);
		}
		else if (pid < 0)
		{
			printf("Something went wrong");
			interrupt_flag = 1;
			exit(1);
		}

	}
	
	return 0;
}
