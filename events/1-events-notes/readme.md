# Events

* `poll`, `ppoll` - Linux, FreeBSD, etc.
* `epoll` - Linux
* `kqueue` - FreeBSD, NetBSD, OpenBSD, DragonflyBSD, macOS

# Links

## epoll

* https://en.wikipedia.org/wiki/Epoll

* Code examples:
 * https://github.com/WhaleGirl/epoll-Server-client/blob/master/epoll_server.c
  * This looks clean and does _NOT_ implement nonblocking socket (while not checking read return for EWOULDBLOCK)
   * see '$ man 3 read'

## kqueue

* https://en.wikipedia.org/wiki/Kqueue

# Related notes

## Blocking & nonblocking IO

* https://eklitzke.org/blocking-io-nonblocking-io-and-epoll
* `$ man fcntl.h`

## fcntl.h

File control options

`$ man fcntl.h`
