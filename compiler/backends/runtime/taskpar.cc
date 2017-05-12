#include "taskpar.h"
#include "vnusrtl.h"

//#define TRACE_ON

static task_id_record_header *current_task=NULL;

static ShapeD1<VnusInt> *the_proc_l = NULL;
static ShapeD1<VnusInt> *the_prio_l = NULL;
static int next_task = 0;

static bool match_tir(
    task_id_record_header *tir1,
    task_id_record_header *tir2
){
    long offset = 0;
    assert (sizeof(long) == sizeof(void *));
    offset = ((long)&tir1->tir_size) - ((long)tir1);
#if 0
    fprintf(stderr,"offset:%ld\n",offset);
#endif
    return ((tir1->tir_size == tir2->tir_size) &&
	    (memcmp(&tir1->tir_size, &tir2->tir_size, tir1->tir_size - offset) == 0));
}

static bool src_equals_dest(
    task_id_record_header *src,
    task_id_record_header *dest,
    task_id_record_header *src2,
    task_id_record_header *dest2
){
    return (match_tir(src,src2) && match_tir(dest, dest2));
}

class MsgElt
{
  public:
    MsgElt *prev;
    MsgElt *next;
    void *message;
    int msg_size;
    
    MsgElt(void *theMsg, int size)
     : prev(NULL), next(NULL), message(theMsg), msg_size(size)
    {}
};

class MsgQueue
{
  public:
    MsgElt *head;
    MsgElt *tail;
    
    MsgQueue()
     : head(NULL), tail(NULL)
    {}
    
    void append(void *message, int size)
    {
	MsgElt *newMsg = new MsgElt(message,size);
	newMsg->prev = tail;
	if (tail==NULL)
	{
	    head = tail = newMsg;
	}
	else 
	{
	    tail->next = newMsg;
	    tail = newMsg;
	}
    }

	/*
	  Find a message matching descriptor *tir.
	  */
    void* find(void *tir, int *size)
    {
	task_id_record_header *src = (task_id_record_header*)tir;
	task_id_record_header *dest = current_task;
	MsgElt *walker = head;
	while (walker != NULL)
	{
	    task_id_record_header *src2 = (task_id_record_header*)walker->message;
	    task_id_record_header *dest2 = (task_id_record_header*)(((char *)walker->message) + src->tir_size);
	    if (src_equals_dest(src,dest,src2,dest2))
	    {
		if (walker->prev == NULL)
		    head = walker->next;
		else
		    walker->prev->next = walker->next;
		if (walker->next == NULL)
		    tail = walker->prev;
		else
		    walker->next->prev = walker->prev;
		void *message = walker->message;
		*size = walker->msg_size;
		delete walker;
		return message;
	    }
	    walker = walker->next;
	}
	return NULL;
    }
};

static MsgQueue theQueue;

/*
  Send a message to the task described by descriptor *tir.
  A size message is sent first(see treceive).
  Messages start with a src and destination id record.
  */
void tsend(void *tir, int size, void *buf)
{
    task_id_record_header *src = current_task;
    task_id_record_header *dest = (task_id_record_header*)tir;
    int newsize = src->tir_size+dest->tir_size+size;
    char *newbuf = new char[newsize];
    int proc_id = get_task_processor(tir);
    memcpy(newbuf, src, src->tir_size);
    memcpy(newbuf+src->tir_size, dest, dest->tir_size);
    memcpy(newbuf+src->tir_size+dest->tir_size, buf, size);
#ifdef TRACE_ON
    fprintf(stderr,"tsend:proc_id\n");
#endif
    vnus_send(proc_id, sizeof(newsize), &newsize);
#ifdef TRACE_ON
    fprintf(stderr,"tsend:message, len=%d\n",newsize);
#endif
    vnus_send(proc_id, newsize, newbuf);
    delete [] newbuf;
}

/*
  Copy the data part of the message(part after the header) into buf.
  If everything is ok, the data part should be of size 'size.
  */
static void extract_message(char *message, int msg_size, void *buf, int size)
{
    task_id_record_header *src = (task_id_record_header*)message;
    task_id_record_header *dest = (task_id_record_header*)(message + src->tir_size);
    void *msg_start = message + src->tir_size + dest->tir_size;
#ifdef TRACE_ON
    fprintf(stderr, "size=%d, msg_size=%d\n",size,msg_size);
#endif
    assert(msg_size == size);
    memcpy(buf, msg_start, msg_size);
}

/*
  Receive a message from the task described by descriptor *tir,
  and destined for the task described by current_task.
  If the message is in the queue retrieve it.
  If not, start receiving messages until the right one is received.
  The other ones are appended to the end of the queue.
  This scheme needs a separate size message. We do know the size of the
  message we want to receive, but we don't know the size of the other
  messages that might be received first.
  */
void treceive(void *tir, int size, void *buf)
{
    int recvsize;
    char *message = (char *)theQueue.find(tir,&recvsize);
    if (message != NULL)
    {
	extract_message(message, recvsize, buf, size);
	delete [] message;
	return;
    }
    int proc_id = get_task_processor(tir);
    while(true)
    {
#ifdef TRACE_ON
	fprintf(stderr,"trecv:proc_id\n");
#endif
	vnus_receive(proc_id, sizeof(recvsize), &recvsize);
	message = new char[recvsize];
#ifdef TRACE_ON
	fprintf(stderr,"trecv:message, len=%d\n",recvsize);
#endif
	vnus_receive(proc_id, recvsize, message);
	task_id_record_header *src = (task_id_record_header*)tir;
	task_id_record_header *dest = current_task;
	task_id_record_header *src2 = (task_id_record_header*)message;
	task_id_record_header *dest2 = (task_id_record_header*)(((char *)message)+src2->tir_size);
#ifdef TRACE_ON
	fprintf(stderr,"src->tir_size:%d, src2->tir_size:%d\n",src->tir_size,src2->tir_size);
	fprintf(stderr,"src->task_id:%d, src2->task_id:%d\n",src->task_id,src2->task_id);
	fprintf(stderr,"dest->tir_size:%d, dest2->tir_size:%d\n",dest->tir_size,dest2->tir_size);
	fprintf(stderr,"dest->task_id:%d, dest2->task_id:%d\n",dest->task_id,dest2->task_id);
#if 0
	for (int i=0; i<src->tir_size/4; i++)
	    fprintf(stderr,"%d ",((int *)src)[i]);
	fprintf(stderr,"\n");
	for (int i=0; i<src2->tir_size/4; i++)
	    fprintf(stderr,"%d ",((int *)src2)[i]);
	fprintf(stderr,"\n");
	for (int i=0; i<dest->tir_size/4; i++)
	    fprintf(stderr,"%d ",((int *)dest)[i]);
	fprintf(stderr,"\n");
	for (int i=0; i<dest2->tir_size/4; i++)
	    fprintf(stderr,"%d ",((int *)dest2)[i]);
	fprintf(stderr,"\n");
#endif
#endif
	if (src_equals_dest(src,dest,src2,dest2))
	{
	    extract_message(message, recvsize - src2->tir_size - dest2->tir_size, buf, size);
	    delete [] message;
	    return;
	}
	else
	{
	    theQueue.append(message, recvsize - src2->tir_size - dest2->tir_size);
	}
    }
}


/*
  The following stuff is needed for the scheduler.
  */
typedef void (*procPtr)(void*, void*);

class RegisterElt
{
  public:
    RegisterElt *next;
    int proc_id;
    procPtr proc;

    RegisterElt(int id, procPtr f)
     : next(NULL), proc_id(id), proc(f)
	{}
};

class Register
{
  public:
    RegisterElt *head;
    RegisterElt *tail;

    Register()
     : head(NULL), tail(NULL)
    {}

    void append(int id, procPtr f)
    {
	RegisterElt *newElt = new RegisterElt(id, f);
	if (tail==NULL)
	{
	    head = tail = newElt;
	}
	else 
	{
	    tail->next = newElt;
	    tail = newElt;
	}
    }

    procPtr find(int id)
    {
	for (RegisterElt *walker = head; walker != NULL; walker = walker->next)
	{
	    if (walker->proc_id == id)
		return walker->proc;
	}
	return NULL;
    }
};

static Register theRegister;

class TaskQueueElt
{
  public:
    TaskQueueElt *next;
    void *tirPtr;
    void *prPtr;
    int thePriority;
    int theMapping;
    
    TaskQueueElt(void *tir, void *pr, int priority, int mapping)
     : next(NULL), tirPtr(tir), prPtr(pr),
       thePriority(priority), theMapping(mapping)
	{}
};

/*
  Priority queue.
  Higher priority goes first.
  */
class TaskQueue
{
  public:
    TaskQueueElt *head;
    TaskQueueElt *tail;
    TaskQueueElt *next;

    TaskQueue()
     : head(NULL), tail(NULL), next(NULL)
    {}

    void append(void *tir, void *pr, int priority, int mapping)
    {
	TaskQueueElt *newElt = new TaskQueueElt(tir, pr, priority, mapping);
	TaskQueueElt *walk = head;
	TaskQueueElt *prev = NULL;
	while (walk!=NULL && walk->thePriority<=priority){
	    prev = walk;
	    walk = walk->next;
	}
	if (tail==NULL)
	{
	    next = head = tail = newElt;
	}
	else if (walk == NULL)
	{
	    tail->next = newElt;
	    tail = newElt;
	}
	else if (prev == NULL)
	{
	    assert(head == walk);
	    newElt->next = head;
	    next = head = newElt;
	}
	else
	{
	    newElt->next = walk;
	    prev->next = newElt;
	}
    }

    TaskQueueElt* pop()
    {
	TaskQueueElt *ret = next;
	if (next != NULL)
	    next = next->next;
	return ret;
    }
    
    TaskQueueElt* find(void *tir)
    {
	task_id_record_header *match = (task_id_record_header*)tir;
	TaskQueueElt *walk = head;
	while (walk != NULL)
	{
	    task_id_record_header *current = (task_id_record_header*)walk->tirPtr;
	    if (match_tir(match,current))
		return walk;
	    walk = walk->next;
	}
	return NULL;
    }
};

TaskQueue theTaskQueue;

/*
  Return the processor the task is supposed to run on.
  */
int get_task_processor(void *tir)
{
    TaskQueueElt *t = theTaskQueue.find(tir);
    assert (t != NULL);
    return t->theMapping;
}

/*
  Return the priority of the task.
  */
int get_task_priority(void */*tir*/)
{
    assert (false);
    return 0;
}

/*
  Add a task to the queue.
  */
void add_task(void *tir, void *pr)
{
    theTaskQueue.append(tir,pr,(*the_prio_l)(next_task), (*the_proc_l)(next_task));
    next_task++;
}

/*
  Associate a function with a task id.
  */
void register_task(int id, procPtr f)
{
    theRegister.append(id, f);
}

/*
  Run tasks from the queue until en is reached.
  */
void execute_tasks()
{
    TaskQueueElt *next;
    while ((next = theTaskQueue.pop()) != NULL)
    {
	if (isThisProcessor(next->theMapping))
	{
	    task_id_record_header *hdr = (task_id_record_header*)next->tirPtr;
	    current_task = hdr;
	    procPtr theProc = theRegister.find(hdr->task_id);
	    assert (theProc != NULL);
	    theProc(next->tirPtr, next->prPtr);
	}
    }
}

void register_processor_list(ShapeD1<VnusInt> *proc_l)
{
    the_proc_l = proc_l;
}

void register_priority_list(ShapeD1<VnusInt> *prio_l)
{
    the_prio_l = prio_l;
}

