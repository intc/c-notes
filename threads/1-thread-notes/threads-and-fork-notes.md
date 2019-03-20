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





