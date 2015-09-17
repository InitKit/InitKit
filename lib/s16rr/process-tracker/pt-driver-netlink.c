/* This is a process-tracker driver using Linux's NetLink Proc connector.
 * The Proc connector allows us to be informed of process forks and exits.
 * Thanks to http://netsplit.com/the-proc-connector-and-socket-filters for
 * demystifying this. */

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <linux/connector.h>
#include <linux/netlink.h>
#include <linux/cn_proc.h>

#include "list.h"

#include "s16rr.h"

ListGenForNameType (pid, pid_t);

void * s16mem_alloc (unsigned long);
void s16mem_free (void *);

typedef struct process_tracker_s
{
    int kq;
    int sock;
    pid_list pids;
} process_tracker_t;

static pid_t * pid_new_p (pid_t val)
{
    pid_t * res = s16mem_alloc (sizeof (pid_t));
    *res = val;
    return res;
}

process_tracker_t * pt_new (int kq)
{
    struct sockaddr_nl addr;
    process_tracker_t * pt = s16mem_alloc (sizeof (process_tracker_t));
    struct iovec iov[3];
    char nlmsghdrbuf[NLMSG_LENGTH (0)];
    struct nlmsghdr * nlmsghdr = nlmsghdrbuf;
    struct cn_msg cn_msg;
    enum proc_cn_mcast_op op;

    pt->kq = kq;
    pt->pids = List_new ();

    sock = socket (PF_NETLINK, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                   NETLINK_CONNECTOR);
    pt->sock = sock;

    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid ();
    addr.nl_groups = CN_IDX_PROC;

    bind (sock, (struct sockaddr *)&addr, sizeof addr);

    nlmsghdr->nlmsg_len = NLMSG_LENGTH (sizeof cn_msg + sizeof op);
    nlmsghdr->nlmsg_type = NLMSG_DONE;
    nlmsghdr->nlmsg_flags = 0;
    nlmsghdr->nlmsg_seq = 0;
    nlmsghdr->nlmsg_pid = getpid ();

    iov[0].iov_base = nlmsghdrbuf;
    iov[0].iov_len = NLMSG_LENGTH (0);

    cn_msg.id.idx = CN_IDX_PROC;
    cn_msg.id.val = CN_VAL_PROC;
    cn_msg.seq = 0;
    cn_msg.ack = 0;
    cn_msg.len = sizeof op;

    iov[1].iov_base = &cn_msg;
    iov[1].iov_len = sizeof cn_msg;

    op = PROC_CN_MCAST_LISTEN;

    iov[2].iov_base = &op;
    iov[2].iov_len = sizeof op;

    writev (sock, iov, 3);

    EV_SET (&ke, sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
    i = kevent (pt->kq, &ke, 1, NULL, 0, NULL);

    if (i == -1)
        fprintf (stderr, "Error: failed to watch NetLink socket %d: %s\n", sock,
                 strerror (errno));

    return pt;
}

int pt_watch_pid (process_tracker_t * pt, pid_t pid)
{
    int i;

    pid_list_add (pt->pids, pid_new_p (pid));

    return i == -1 ? 1 : 0;
}

void pt_disregard_pid (process_tracker_t * pt, pid_t pid)
{
    pid_list_iterator it;

    for (it = pid_list_begin (pt->pids); it != NULL;
         pid_list_iterator_next (&it))
    {
        if (*it->val == pid)
        {
            s16mem_free (it->val);
            pid_list_del (pt->pids, it->val);
            break;
        }
    }

    return;
}

pt_info_t * pt_investigate_kevent (process_tracker_t * pt, struct kevent * ke)
{
    pt_info_t * result;
    pt_info_t info;

    struct msghdr msghdr;
    struct sockaddr_nl addr;
    struct iovec iov[1];
    char buf[PAGE_SIZE];
    ssize_t len;

    if (ke->filter != EVFILT_READ || ke->ident != pt->sock)
        return 0;

    msghdr.msg_name = &addr;
    msghdr.msg_namelen = sizeof addr;
    msghdr.msg_iov = iov;
    msghdr.msg_iovlen = 1;
    msghdr.msg_control = NULL;
    msghdr.msg_controllen = 0;
    msghdr.msg_flags = 0;

    iov[0].iov_base = buf;
    iov[0].iov_len = sizeof buf;

    len = recvmsg (sock, &msghdr, 0);

    if (addr.nl_pid != 0)
        return 0;

    for (struct nlmsghdr * nlmsghdr = (struct nlmsghdr *)buf;
         NLMSG_OK (nlmsghdr, len); nlmsghdr = NLMSG_NEXT (nlmsghdr, len))
    {
        struct cn_msg * cn_msg;
        struct proc_event * ev;

        cn_msg = cn_msg = NLMSG_DATA (nlmsghdr);
        if ((cn_msg->id.idx != CN_IDX_PROC) || (cn_msg->id.val != CN_VAL_PROC))
            continue;

        ev = (struct proc_event *)cn_msg->data;

        switch (ev->what)
        {
        case PROC_EVENT_FORK:
            info.event = PT_CHILD;
            info.pid = ev->event_data.fork.child_tgid;
            info.ppid = ev->event_data.fork.parent_tgid;
            info.flags = 0;
            pid_list_add (pt->pids, pid_new_p (info.pid));
            goto reply;
            break;
        case PROC_EVENT_EXIT:
            info.event = PT_EXIT;
            info.pid = ev->event_data.exit.process_tgid;
            info.ppid = 0;
            info.flags = 0;
            pt_disregard_pid (info.pid);
            goto reply;
            break;
        }
    }
    else return 0;

reply:
    result = s16mem_alloc (sizeof (pt_info_t));
    *result = info;

    return result;
}

void pt_destroy (process_tracker_t * pt)
{
    for (pid_list_iterator it = pid_list_begin (pt->pids); it != NULL;
         pid_list_iterator_next (&it))
    {
        pt_disregard_pid (pt, *it->val);
        free (it->val);
    }

    List_destroy (pt->pids);
    close (pt->sock);
    s16mem_free (pt);
}
