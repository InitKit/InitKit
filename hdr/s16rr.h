#ifndef __S16_RR_H_
#define __S16_RR_H_

#include <sys/wait.h>

#ifdef __cplusplus
extern "C" {
#endif

int subreap_acquire ();
int subreap_relinquish ();
int subreap_status ();
pid_t process_get_ppid (pid_t pid);

#ifdef __cplusplus
}
#endif

#endif
