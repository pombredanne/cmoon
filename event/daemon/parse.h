
#ifndef _PARSE_H
#define _PARSE_H

#include "req.h"

#include "mheads.h"

#ifdef DEBUG_MSG
#define MSG_DUMP(pre, p, psize)						\
	do {											\
		unsigned char zstra[MAX_PACKET_LEN*2+1];	\
		mmisc_hex2str(p, psize, zstra);				\
		mtc_dbg("%s%s", pre, zstra);				\
	} while (0)
#else
#define MSG_DUMP(p, psize)
#endif

int parse_message(struct req_info *req,
		  const unsigned char *buf, size_t len);

#endif

