#ifndef APP_H_
#define APP_H_

#include <sys/poll.h>

#include <functional>

#include "../js/js.h"
#include "../restarters/restarter.h"
#include "../scheduler/scheduler.h"
#include "evloop.h"

#define log_trace(...)
#define log_dbg(...) printf(__VA_ARGS__)
#define log_err(...) fprintf(stderr, __VA_ARGS__)

struct kevent;

class App {

    public:
	typedef Evloop::timerid_t timerid_t;

    protected:
	struct Timer {
		typedef std::function<void(timerid_t, uintptr_t)> callback_t;

		callback_t m_cb; //!< callback to invoke on timer elapse
		uintptr_t m_udata;

		Timer(callback_t cb, uintptr_t udata = 0)
		    : m_cb(cb)
		    , m_udata(udata) {};
	};

	struct FD {
		typedef std::function<void(int)> callback_t;

		int m_fd;
		int m_events;
		callback_t
		    m_cb; //!< callback to invoke on FD readable/writeable

		FD(int fd, int events, callback_t cb)
		    : m_fd(fd)
		    , m_events(events)
		    , m_cb(cb) {};
	};

	std::list<std::unique_ptr<Timer>> m_timers;
	std::list<std::unique_ptr<FD>> m_fds;

	void handle_timer(struct kevent *kev);
	void handle_fd(struct kevent *kev);

    public:
	int m_kq;
	JS m_js;
	Scheduler m_sched;
	/** Object type to restarter. */
	std::unordered_map<std::string, Restarter *> restarters;

	App();

	/** Add a new timer. Returns 0 on failure, otherwise unique ID. */
	timerid_t add_timer(bool recur, int ms, Timer::callback_t cb,
	    uintptr_t udata = 0);
	int del_timer(timerid_t id);

	int add_fd(int fd, int events, FD::callback_t cb);
	int del_fd(int fd);

	int loop();
};

#endif /* APP_H_ */
