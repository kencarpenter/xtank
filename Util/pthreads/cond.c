/*
** Xtank
**
** Copyright 2000 by Kurt J. Lidl
**
** $Id$
*/

/*
** A sample program to test pthread condition variables and mutexes
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>	/* for ETIMEDOUT */
#include <pthread.h>

static int ret_a;

void thread_a(void);
pthread_t self, thd_a;

typedef struct CROCK {
	pthread_mutex_t	mutex;	/* protection object */
	pthread_cond_t	cond;	/* signaling object */
	int		value;	/* access protected by mutex */
} crock_t;

/* static initialization of mutex, condition variable */
crock_t data = {
	PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0,
};

/* if TIMEOUT > SLEEPVAL, then signal will work */
/* if TIMEOUT =< SLEEPVAL, then timeout will occur */

#define SLEEPVAL 2
#define TIMEOUT SLEEPVAL+1

int
main(int argc, char *argv[])
{
	int status;
	struct timespec tout;
	pthread_attr_t suspended;

	self = pthread_self();

	status = pthread_attr_init(&suspended);
	if (status) {
		perror("pthread_attr_init");
		exit(1);
	}

	status = pthread_attr_setsuspendstate_np(&suspended,PTHREAD_CREATE_SUSPENDED);
	if (status) {
		perror("pthread_attr_setsuspendstate_np");
		exit(1);
	}

	status = pthread_create(&thd_a, &suspended, (void *)thread_a, (void *)0);
	if (status) {
		perror("pthread_create thread_a");
		exit(1);
	}

	/* run anybody else who wants to run, probably nobody */
	(void) sched_yield();

	printf("after first call to sched_yield\n");

	tout.tv_sec = time (NULL) + TIMEOUT;
	tout.tv_nsec = 0;

	/* wake up the other thread */
	printf("before call to pthread_resume_np(thd_a)\n");
	pthread_resume_np(thd_a);
	printf("after call to pthread_resume_np(thd_a)\n");

	/* will block until lock succeeds */
	status = pthread_mutex_lock(&data.mutex);
	if (status) {
		perror("main: pthread_mutex_lock");
		exit(1);
	}

	while (data.value == 0) {
		status = pthread_cond_timedwait(&data.cond, &data.mutex, &tout);
		if (status == ETIMEDOUT) {
			printf("Condition wait timed out.\n");
			break;
		} else if (status) {
			perror("main: pthread_mutex_lock");
			exit(1);
		}
	}

	if (data.value != 0) {
		printf("Condition was signaled!\n");
	}
	status = pthread_mutex_unlock(&data.mutex);
	if (status) {
		perror("main: pthread_mutex_unlock");
		exit(1);
	}

	printf("main calls pthread_join()\n");
	status = pthread_join(thd_a, NULL);
	if (status) {
		perror("pthread_join");
		exit(1);
	}
	printf("main finishes pthread_join()\n");

	exit (0);
}

void
thread_a(void)
{
	int status;

	printf("thread_a started\n");

	sleep(SLEEPVAL);
	status = pthread_mutex_lock(&data.mutex);
	if (status) {
		perror("pthread_mutex_lock");
		exit(1);
	}
	data.value = 1;
	status = pthread_cond_signal(&data.cond);
	if (status) {
		perror("pthread_cond_signal");
		exit(1);
	}
	status = pthread_mutex_unlock(&data.mutex);
	if (status) {
		perror("pthread_mutex_unlock");
		exit(1);
	}

	printf("thread_a finished\n");
	ret_a = 1;
	pthread_exit (&ret_a);
}
