/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEC_LOG_H
#define OEC_LOG_H

#include <config.h>
#include <syslog.h>
#include <stdio.h>

#define dbg_tvar(t)     t   dbg_ ## t

static dbg_tvar(char);
static dbg_tvar(int);
static int dbg_level;

#ifdef OE_SYSLOG

#define dbg_printf(...) (syslog(LOG_DEBUG, __VA_ARGS__))

#ifdef OE_DEBUG
#define OE_DLOG(rc, ...) syslog(LOG_DEBUG, __VA_ARGS__);
#else
#define OE_DLOG(rc, ...) ;
#endif /* OE_DEBUG'ing */

#ifdef OE_TRACE
#define OE_TLOG(rc, ...) syslog(LOG_DEBUG, __VA_ARGS__);
#else
#define OE_TLOG(rc, ...) ;
#endif /* OE_TRACE'ing */

#define OE_ILOG(rc, ...) syslog(LOG_INFO, __VA_ARGS__);

#define OE_ERR(rc, ...) syslog(LOG_ERR, __VA_ARGS__);

#else

#define dbg_printf (printf)

#ifdef OE_DEBUG
#define OE_DLOG(rc, ...) fprintf(stdout, __VA_ARGS__);
#else
#define OE_DLOG(rc, ...) ;
#endif /* OE_DEBUG'ing */

#ifdef OE_TRACE
#define OE_TLOG(rc, ...) fprintf(stdout, __VA_ARGS__);
#else
#define OE_TLOG(rc, ...) ;
#endif /* OE_TRACE'ing */

#define OE_ILOG(rc, ...) fprintf(stdout, __VA_ARGS__);
#define OE_ERR(rc, ...) fprintf(stderr, __VA_ARGS__);

#endif /* OE_SYSLOG */

/////////////////////
/// TRACE SUPPORT ///
/////////////////////
#define DBG_CALL_VOID(func,param)                 \
    (dbg_printf("%20s(%4i)%*s >" #func            \
                "()\n", __FILE__, __LINE__,       \
                dbg_level*2,""),                  \
     dbg_level++,                                 \
     (func) param,                                \
     dbg_level--,                                 \
     dbg_printf("%20s(%4i)%*s <" #func            \
                "()\n",__FILE__,__LINE__,         \
                dbg_level*2,""),                  \
     (void) 0                                     \
     )

#define DBG_CALL(fmt,ret,func,param)              \
    (dbg_printf("%20s(%4i)%*s >" #func            \
                "()\n", __FILE__, __LINE__,       \
                dbg_level*2,""),                  \
     dbg_level++,                                 \
     dbg_ ## ret = (func) param,                  \
     dbg_level--,                                 \
     dbg_printf("%20s(%4i)%*s <" #func            \
                "() --> " fmt "\n",__FILE__,      \
                __LINE__, dbg_level*2,"",         \
                dbg_ ## ret),                     \
     dbg_ ## ret                                  \
     )

#ifdef OE_TRACE

#define OEK_put(k,c) \
        DBG_CALL_VOID(OEK_put,(k,c))

#define OEK_get(k,c) \
        DBG_CALL_VOID(OEK_get,(k,c))

#define OEK_sta(k,c) \
        DBG_CALL_VOID(OEK_sta,(k,c))

#define OEK_upt(k,c) \
        DBG_CALL_VOID(OEK_upt,(k,c))

#define OEK_upl(k,c) \
        DBG_CALL_VOID(OEK_upl,(k,c))

#ifdef NOTHING
#define OeStore_put(a,b,c) \
        DBG_CALL("%i", int, OeStore_put,(a,b,c))
#define OeStore_get(a,b,c,d,e,f,g,h,i,j,k) \
        DBG_CALL("%i", int, OeStore_get,(a,b,c,d,e,f,g,h,i,j,k))

#define Oed_Dispatcher_start(d) \
        DBG_CALL("%i", int, Oed_Dispatcher_start,(d))

#define Oed_Dispatcher_stop(d) \
        DBG_CALL("%i", int, Oed_Dispatcher_start,(d))

#define Oed_Dispatcher_process(a, b, c) \
        DBG_CALL_VOID(Oed_Dispatcher_process,(a,b,c))

#define Oed_Dispatcher_exec(a, b, c) \
        DBG_CALL_VOID(Oed_Dispatcher_exec,(a,b,c))
#endif /* NOTHING */

#endif /* OE_TRACE'ing */

#endif /* OEC_LOG_H */

#ifdef __cplusplus
}
#endif 

