#include "../../../inc/slib/core/thread.h"
#include "../../../inc/slib/core/system.h"

#if defined(SLIB_PLATFORM_IS_ANDROID)
#	include "../../../inc/slib/core/platform_android.h"
#endif

SLIB_NAMESPACE_BEGIN
Thread::Thread()
{
	m_eventWake = Event::create(sl_true);
	m_eventExit = Event::create(sl_false);

	m_flagRunning = sl_false;
	m_flagRequestStop = sl_false;

	m_handle = sl_null;
	m_priority = Thread::priorityNormal;
}

Thread::~Thread()
{
}

Thread::Priority Thread::getPriority()
{
	return m_priority;
}

void Thread::setPriority(Priority priority)
{
	m_priority = priority;
	_nativeSetPriority();
}

Ref<Thread> Thread::getCurrent()
{
	return Thread::_nativeGetCurrentThread();
}

sl_bool Thread::isCurrentThread()
{
	return getCurrent() == this;
}

sl_bool Thread::start(sl_uint32 stackSize)
{
	if (!m_flagRunning) {
		m_flagRunning = sl_true;
		m_flagRequestStop = sl_false;
		m_eventExit->reset();
		m_eventWake->reset();
		_nativeStart(stackSize);
		if (m_handle) {
			if (m_priority != priorityNormal) {
				_nativeSetPriority();
			}
			return sl_true;
		} else {
			m_flagRunning = sl_false;
		}
	}
	return sl_false;
}

void Thread::finish()
{
	if (m_flagRunning) {
		m_flagRequestStop = sl_true;
		wake();
	}
}

sl_bool Thread::join(sl_int32 timeout)
{
	if (m_flagRunning) {
		if (!m_eventExit->wait(timeout)) {
			return sl_false;
		}
	}
	return sl_true;
}

sl_bool Thread::finishAndWait(sl_int32 timeout)
{
	if (isCurrentThread()) {
		if (m_flagRunning) {
			m_flagRequestStop = sl_true;
		}
		return sl_false;
	}
	if (timeout >= 0) {
		if (m_flagRunning) {
			m_flagRequestStop = sl_true;
            while (1) {
                wake();
                sl_int32 t = timeout;
                if (t > 100) {
                    t = 100;
                }
                if (m_eventExit->wait(t)) {
                    return sl_true;
                }
                if (timeout <= 100) {
                    break;
                }
                timeout -= 100;
            }
        }
	} else {
		while (m_flagRunning) {
			m_flagRequestStop = sl_true;
            wake();
			System::sleep(1);
		}
	}
	return sl_false;
}

void Thread::wake()
{
	Ref<Event> ev = m_eventWaiting;
	if (ev.isNotNull()) {
		ev->set();
	}
}

sl_bool Thread::wait(sl_int32 timeout)
{
	if (m_flagRequestStop) {
		return sl_false;
	} else {
		sl_bool ret = m_eventWake->wait(timeout);
		return ret;
	}
}

void Thread::setWaitingEvent(Event* ev)
{
	m_eventWaiting = ev;
}

void Thread::clearWaitingEvent()
{
	m_eventWaiting.setNull();
}

Variant Thread::getProperty(const String& name)
{
	return m_properties.getValue(name, Variant::null());
}

void Thread::setProperty(const String& name, const Variant& value)
{
	m_properties.put(name, value);
}

void Thread::clearProperty(const String& name)
{
	m_properties.remove(name);
}

void Thread::_run()
{
#if defined(SLIB_PLATFORM_USE_JNI)
	Jni::attachThread();
#endif

	Thread::_nativeSetCurrentThread(this);
	if (m_runnable.isNotNull()) {
		m_runnable->run();
	}
	m_runnable.setNull();

#if defined(SLIB_PLATFORM_USE_JNI)
	Jni::detachThread();
#endif

	_nativeClose();
	m_handle = sl_null;
	m_flagRunning = sl_false;
	m_eventExit->set();
}

Ref<Thread> Thread::create(const Ref<Runnable>& runnable)
{
	Ref<Runnable> r = runnable;
	if (r.isNull()) {
		return Ref<Thread>::null();
	}
	Ref<Thread> ret = new Thread();
	if (ret.isNotNull()) {
		ret->m_runnable = r;
	}
	return ret;
}

Ref<Thread> Thread::start(const Ref<Runnable>& runnable, sl_uint32 stackSize)
{
	Ref<Thread> ret = create(runnable);
	if (ret.isNotNull()) {
		if (ret->start(stackSize)) {
			return ret;
		}
	}
	return Ref<Thread>::null();
}

sl_bool Thread::sleep(sl_uint32 ms)
{
	Ref<Thread> thread = getCurrent();
	if (thread.isNotNull()) {
		return thread->wait(ms);
	} else {
		System::sleep(ms);
		return sl_true;
	}
}

sl_bool Thread::isStoppingCurrent()
{
	Ref<Thread> thread = getCurrent();
	if (thread.isNotNull()) {
		return thread->isStopping();
	}
	return sl_false;
}

sl_bool Thread::isNotStoppingCurrent()
{
	Ref<Thread> thread = getCurrent();
	if (thread.isNotNull()) {
		return thread->isNotStopping();
	}
	return sl_false;
}

sl_uint64 Thread::getCurrentThreadUniqueId()
{
	static sl_int32 uid = 10000;
	sl_uint64 n = _nativeGetCurrentThreadUniqueId();
	if (n > 0) {
		return n;
	}
	n = Base::interlockedIncrement32(&uid);
	_nativeSetCurrentThreadUniqueId(n);
	return n;
}
SLIB_NAMESPACE_END