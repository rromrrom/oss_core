#ifndef __GLOBALS_H__
#define __GLOBALS_H__ 1

#include "OSS/Net/oss_carp.h"

#ifdef DEFINE_GLOBALS
# define GLOBAL0(A) A
# define GLOBAL(A, B) A = B
#else
# define GLOBAL0(A) extern A
# define GLOBAL(A, B) extern A
#endif

GLOBAL0(char *interface);
GLOBAL0(struct in_addr srcip);
GLOBAL0(unsigned char vhid);
GLOBAL0(char *pass);
GLOBAL0(struct in_addr vaddr);
GLOBAL(unsigned char advbase, DEFAULT_ADVBASE);
GLOBAL(unsigned int dead_ratio, DEFAULT_DEAD_RATIO);
GLOBAL0(unsigned char advskew);
GLOBAL0(char *upscript);
GLOBAL0(char *downscript);
GLOBAL0(char *garpscript);
GLOBAL0(signed char preempt);
GLOBAL0(signed char neutral);
GLOBAL0(signed char shutdown_at_exit);
GLOBAL0(unsigned char hwaddr[6]);
GLOBAL0(signed char no_syslog);
GLOBAL0(signed char daemonize);
GLOBAL0(signed char ignoreifstate);
GLOBAL0(signed char no_mcast);
GLOBAL(int syslog_facility, DEFAULT_FACILITY);
GLOBAL0(char *vaddr_arg);
GLOBAL0(char *xparam);
GLOBAL(unsigned char inaddr_carp_group[4], { 224 _COMA_ 0 _COMA_ 0 _COMA_ 18 });
GLOBAL0(sig_atomic_t received_signal);

typedef void (*on_state_change_t)(int state);
GLOBAL(on_state_change_t on_state_change, NULL);

typedef void (*on_gratuitous_arp_t)();
GLOBAL(on_gratuitous_arp_t on_gratuitous_arp, NULL);

typedef void (*on_log_callback_t)(int level, const char* line);
GLOBAL(on_log_callback_t on_log_callback, NULL);

typedef void (*on_received_signal_t)(int sig);
GLOBAL(on_received_signal_t on_received_signal, NULL);
#endif
