# Posix Threads

* https://en.wikipedia.org/wiki/POSIX_Threads
* http://www.csc.villanova.edu/~mdamian/threads/posixthreads.html
* https://www.ibm.com/developerworks/library/l-posix2/
* https://www.ibm.com/developerworks/views/linux/libraryview.jsp?topic_by=All+topics+and+related+products&sort_order=asc&lcl_sort_order=desc&search_by=posix+threads&type_by=Articles&show_abstract=true&start_no=1&sort_by=Date&end_no=100&show_all=false
* ok: https://cs.gmu.edu/~rcarver/ModernMultithreading/LectureNotes/Chapter3Notes.pdf

* Mutexes
 * http://www.csc.villanova.edu/~mdamian/threads/posixmutex.html
 * https://github.com/angrave/SystemProgramming/wiki/Synchronization,-Part-3:-Working-with-Mutexes-And-Semaphores
 * ok: https://docs.oracle.com/cd/E19455-01/806-5257/6je9h032p/index.html

* Semaphores
 * http://www.csc.villanova.edu/~mdamian/threads/posixsem.html
 * https://www.softprayog.in/programming/semaphore-basics (meh)
 * https://www.softprayog.in/programming/posix-semaphores
 * https://jlmedina123.wordpress.com/2013/05/03/pthreads-with-mutex-and-semaphores/ (meh)
 * ok: https://docs.oracle.com/cd/E19455-01/806-5257/6je9h032s/index.html

## man pages

* `# man pthreads`
 * Contains also a list of function which are NOT thread safe

## Creating and joining

* `pthread_create()`
 * Creates a new thread

* `pthread_join(pthread_t thread, void **retval)`
 * Joins child thread when it has exitted (blocks the calling thread)
 * This is not required in case child calls `pthread_detach` before it's exit.
 * Joining allows receiving allocated data from the thread which has exitted (2nd arg).

* `pthread_detach(pthread_t thread)`
 * Marks "thread" as detached (-> "thread" can not be joined)
 * Detaching thread which has been already detached results undefined behaviour.
 * It's possible to create new thread direclty with detached state (`pthread_attr_setdetachstate`)

## Inside a thread

* `pthread_detach()`
 * See notes in the section above.

* `pthread_exit()`

## Mutual exclusion - Mutexes (locks)

A mutex will allow calling thread to lock the critical section for it's own use.

When the information has been modofied, the calling thread unlocks the mutex so that other threads can access the information.

* `pthread_mutex_t` - variable type for a mutex

* `pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr)`
 * Initialize the mutex
 * #1 arg: pointer to mutex
 * #2 arg: mutex attributes
 * returns 0 on success, error number in case of an error (positive?)

* `pthread_mutex_destroy(pthread_mutex_t *mutex)`
 * Destroy the mutex
 * #1 arf: pointer to the mutex
 * returns 0 on success, error number in case of an error (positive?)

Attempting to unlock a mutex in a thread other than the lock owner is undefined behaviour in POSIX (check).

### mutex in POSIX Programmer's Manual

* `# man pthread_mutex_init`
* `# man pthread_mutex_lock`
* `# man pthread_mutex_unlock`
* `# man pthread_mutex_consistent`
* `# man pthread_mutex_distroy`
* `# man pthread_mutex_timedlock`
* `# man pthread_mutexattr_getro-bust`
* `# man pthread_mutexattr_setrobust`
* `# man pthread_mutexattr_getrobust`
* `# man pthread_mutexattr_init`

## Semaphores

### Binary semaphore

Binary semaphore (a semaphore with count = 1) can be used as a lock (like mutex). For two subsequent calls to P(), the second call will block. This differs form the case of using locks. If the lock owning thread calls lock again it's *always* not blocked (see `man pthread_mutex_lock`).

### semaphore in POSIX Programmer's Manual

* `# man semaphore.h"`

# Threads (POSIX) and fork(2)

## Links

* http://www.linuxprogrammingblog.com/threads-and-fork-think-twice-before-using-them
* https://stackoverflow.com/questions/6078712/is-it-safe-to-fork-from-within-a-thread

* https://mail.gnome.org/archives/gtk-devel-list/2015-March/msg00038.html
 * https://mail.gnome.org/archives/gtk-devel-list/2015-March/msg00046.html
  * https://mail.gnome.org/archives/gtk-devel-list/2015-March/msg00047.html


## In short

* Risks with standard library functions
 * Some functions will use locking
* Only suggested scenario is to call an external process directly after fork via execve().
 * Working with file descriptors requires some extra atention
  * dup3(2), socket(2), open(2)
   * O_CLOEXEC
  * https://udrepper.livejournal.com/20407.html

NOTE: O_CLOEXEC is useful in "regular" fork scenarious as well in order to contril the fd inheritance
for porcess created by exec.


