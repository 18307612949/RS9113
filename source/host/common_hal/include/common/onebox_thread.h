/**
 * @file onebox_thread.h
 * @author 
 * @version 1.0
 *
 * @section LICENSE
 *
 * This software embodies materials and concepts that are confidential to Redpine
 * Signals and is made available solely pursuant to the terms of a written license
 * agreement with Redpine Signals
 *
 * @section DESCRIPTION
 *
 * This file contians the function prototypes/ datastructures related to threads
 * 
 */

#ifndef __ONEBOX_THREAD_H__
#define __ONEBOX_THREAD_H__

#define ONEBOX_THREAD_NAME_LEN  15
#define EVENT_WAIT_FOREVER     (0x00)

#define onebox_signal_pending()       signal_pending(current) 

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)
typedef  struct work_struct         ONEBOX_WORK_QUEUE;
#define  onebox_init_work_queue(a,b,c)  INIT_WORK(a,b)
#define  onebox_schedule_work(a)        schedule_work(a)
#else
typedef  struct tq_struct           ONEBOX_WORK_QUEUE;
#define  onebox_init_work_queue(a,b,c)  INIT_TQUEUE(a,b,c)
#define  onebox_schedule_work(a)        schedule_task(a)
#endif

#define onebox_schedule()        schedule()

typedef void (*thread_function)(void *);

typedef struct _onebox_event 
{  
	atomic_t        eventCondition;         
	wait_queue_head_t   eventQueue;            
}ONEBOX_EVENT, *PONEBOX_EVENT;

typedef struct semaphore ONEBOX_SEMAPHORE,*PONEBOX_SEMAPHORE;

typedef struct _onebox_thread_handle_t 
{
	uint8   name[ONEBOX_THREAD_NAME_LEN + 1]; /* Thread Name */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39)
	struct task_struct *thread_id;
#else
	int   thread_id; /* OS Specific ThreadId */
#endif
	int   kill_thread; /* Flag to indicate Thread Kill */
	thread_function function_ptr; /* Thread Function */
	void  *context; /* Argument to thread function */
	struct completion completion; /* Thread Compeletion event */
	ONEBOX_WORK_QUEUE task_queue;
	ONEBOX_SEMAPHORE sync_thread;
	struct mutex thread_lock;
	atomic_t thread_done;
	struct completion thread_complete;
	ONEBOX_EVENT thread_event;

}onebox_thread_handle_t;

int rsi_start_thread(onebox_thread_handle_t *handle); 
#endif
