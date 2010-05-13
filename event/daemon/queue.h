
#ifndef _QUEUE_H
#define _QUEUE_H

#include <pthread.h>		/* for mutexes */
#include <stdint.h>		/* for uint32_t */
#include "req.h"		/* for req_info */
#include "sparse.h"

#include "ClearSilver.h"

#define QUEUE_SIZE_INFO		100
#define QUEUE_SIZE_WARNING	10000
#define MAX_QUEUE_ENTRY		1048576
//#define MAX_QUEUE_ENTRY	2097152

struct queue {
	pthread_mutex_t lock;
	pthread_cond_t cond;

	size_t size;
	struct queue_entry *top, *bottom;
};

struct queue_entry {
	uint32_t operation;
	struct req_info *req;

	unsigned char *ename;
	size_t esize;
	HDF *hdfrcv;
	HDF *hdfsnd;

	struct queue_entry *prev;
	/* A pointer to the next element on the list is actually not
	 * necessary, because it's not needed for put and get.
	 */
};


struct queue *queue_create();
void queue_free(struct queue *q);

struct queue_entry *queue_entry_create();
void queue_entry_free(struct queue_entry *e);

size_t queue_entry_size(struct queue_entry *e);

void queue_lock(struct queue *q)
	__acquires(q->lock);
void queue_unlock(struct queue *q)
	__releases(q->lock);
void queue_signal(struct queue *q);
int queue_timedwait(struct queue *q, struct timespec *ts)
	__with_lock_acquired(q->lock);

void queue_put(struct queue *q, struct queue_entry *e)
	__with_lock_acquired(q->lock);
void queue_cas(struct queue *q, struct queue_entry *e)
	__with_lock_acquired(q->lock);
struct queue_entry *queue_get(struct queue *q)
	__with_lock_acquired(q->lock);
int queue_isempty(struct queue *q)
	__with_lock_acquired(q->lock);


#define REQ_GET_PARAM_U32(hdf, key, ret)		\
    do {										\
		if (!hdf_get_value(hdf, key, NULL)) {	\
            return REP_ERR_BADPARAM;			\
		}										\
		ret = hdf_get_int_value(hdf, key, 0);	\
    } while (0)

#define REQ_GET_PARAM_ULONG(hdf, key, ret)						\
    do {														\
		if (!hdf_get_value(hdf, key, NULL)) {					\
            return REP_ERR_BADPARAM;							\
		}														\
		ret = strtoul(hdf_get_value(hdf, key, NULL), NULL, 10);	\
    } while (0)

#define REQ_GET_PARAM_STR(hdf, key, ret)		\
    do {										\
		ret = hdf_get_value(hdf, key, NULL);	\
		if (!ret) {								\
            return REP_ERR_BADPARAM;			\
		}										\
    } while (0)


#define REQ_FETCH_PARAM_U32(hdf, key, ret)			\
    do {											\
		if (hdf_get_value(hdf, key, NULL)) {		\
			ret = hdf_get_int_value(hdf, key, 0);	\
		}											\
    } while (0)

#define REQ_FETCH_PARAM_ULONG(hdf, key, ret)						\
    do {															\
		if (hdf_get_value(hdf, key, NULL)) {						\
			ret = strtoul(hdf_get_value(hdf, key, NULL), NULL, 10); \
		}															\
    } while (0)

#define REQ_FETCH_PARAM_STR(hdf, key, ret)		\
    do {										\
		ret = hdf_get_value(hdf, key, NULL);	\
    } while (0)

#endif

