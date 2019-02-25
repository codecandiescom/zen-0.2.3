/**
 * Various functions to deal with threading. These are mostly wrappers
 * for the pthread functions, but also has thread types to separate 
 * threads which are used for different things.
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

/*
 * Copyright (C) 1999, Tomas Berndtsson <tomas@nocrew.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif /* HAVE_PTHREAD_H */

#include "threads.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/* Commands going from the interface to the main program. */
static pthread_mutex_t thread_command_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t thread_command_cond = PTHREAD_COND_INITIALIZER;
static enum thread_command thread_cmd = THREAD_COMMAND_UNKNOWN;
static void *thread_cmd_argument = NULL;

/* Responses going from the main program to the interface. 
 * This is usually sent due to a request command from the interface, but
 * may be sent at other times as well.
 */
static pthread_mutex_t thread_response_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t thread_response_cond = PTHREAD_COND_INITIALIZER;
static enum thread_response thread_res = THREAD_COMMAND_UNKNOWN;
static void *thread_res_argument = NULL;

static struct thread_info *threads = NULL;

/**
 * Start a new thread and add it to our linked list of threads.
 *
 * @param type The type of the new thread.
 * @param start_function A pointer to the function which starts the 
 * @param start_function new thread.
 * @param arg An argument to the start_function.
 *
 * @return non-zero value if an error occurred.
 */
int thread_start(enum thread_type type, thread_function *start_function, 
		 void *arg)
{
  int ret;
  struct thread_info *threadp;

  threadp = (struct thread_info *)malloc(sizeof(struct thread_info));
  if(threadp == NULL)
    return 1;

  threadp->type = type;
  ret = pthread_create(&threadp->thread, NULL, start_function, arg);
  if(ret != 0) {
    free(threadp);
  } else {
    threadp->next = threads;
    threads = threadp;
  }

  return ret;
} 

/**
 * Kill all threads which we have on the linked list, and remove
 * them from the very same.
 *
 * @return non-zero value if an error occurred.
 */
int thread_kill_all(void)
{
  struct thread_info *threadp, *tmp_thread;

  threadp = threads;
  while(threadp) {
    pthread_cancel(threadp->thread);
    tmp_thread = threadp;
    threadp = threadp->next;
    free(tmp_thread);
  }

  threads = NULL;

  return 0;
}

/**
 * Kill all threads of a specific type, and remove them from the
 * linked list.
 *
 * @param The type of the threads we want to kill.
 *
 * @return non-zero value if an error occurred.
 */
int thread_kill(enum thread_type type)
{
  struct thread_info *threadp, *previous, *tmp_thread;

  threadp = threads;
  previous = NULL;
  while(threadp) {
    if(threadp->type == type) {
      pthread_cancel(threadp->thread);
      if(previous == NULL)
	threads = threadp->next;
      else
	previous->next = threadp->next;
      tmp_thread = threadp;
      threadp = threadp->next;
      free(tmp_thread);
    } else {
      previous = threadp;
      threadp = threadp->next;
    }
  }

  return 0;  
}

/**
 * Wait for all threads of a specific type, and remove them from the
 * linked list.
 *
 * @param The type of the threads we want to wait for.
 *
 * @return non-zero value if an error occurred.
 */
int thread_wait(enum thread_type type)
{
  struct thread_info *threadp, *previous, *tmp_thread;

  threadp = threads;
  previous = NULL;
  while(threadp) {
    if(threadp->type == type) {
      pthread_join(threadp->thread, NULL);
      if(previous == NULL)
	threads = threadp->next;
      else
	previous->next = threadp->next;
      tmp_thread = threadp;
      threadp = threadp->next;
      free(tmp_thread);
    } else {
      previous = threadp;
      threadp = threadp->next;
    }
  }

  return 0;  
}

/**
 *
 */
enum thread_command thread_wait_command(void **argument)
{
  enum thread_command command;

  pthread_mutex_lock(&thread_command_mutex);
  pthread_cond_wait(&thread_command_cond, &thread_command_mutex);

  command = thread_cmd;
  *argument = thread_cmd_argument;
  thread_cmd = THREAD_COMMAND_UNKNOWN;
  
  pthread_mutex_unlock(&thread_command_mutex);

  return command;
}

/**
 *
 */
int thread_give_command(enum thread_command command, void *argument)
{
  pthread_mutex_lock(&thread_command_mutex);

  thread_cmd = command;
  thread_cmd_argument = argument;
  
  pthread_cond_signal(&thread_command_cond);
  pthread_mutex_unlock(&thread_command_mutex);

  return 0;
}

/**
 *
 */
enum thread_response thread_wait_response(void **argument)
{
  enum thread_response response;

  pthread_mutex_lock(&thread_response_mutex);
  pthread_cond_wait(&thread_response_cond, &thread_response_mutex);

  response = thread_res;
  *argument = thread_res_argument;
  thread_res = THREAD_RESPONSE_UNKNOWN;
  
  pthread_mutex_unlock(&thread_response_mutex);

  return response;
}

/**
 *
 */
int thread_check_response(enum thread_response expected_response, 
			  void **argument)
{
  int ret = 0;

  pthread_mutex_lock(&thread_response_mutex);

  if(thread_res == expected_response) {
    *argument = thread_res_argument;
    thread_res = THREAD_RESPONSE_UNKNOWN;
    ret = 1;
  }
  
  pthread_mutex_unlock(&thread_response_mutex);

  return ret;
}

/**
 *
 */
int thread_give_response(enum thread_response response, void *argument)
{
  pthread_mutex_lock(&thread_response_mutex);

  thread_res = response;
  thread_res_argument = argument;
  
  pthread_cond_signal(&thread_response_cond);
  pthread_mutex_unlock(&thread_response_mutex);

  return 0;
}

