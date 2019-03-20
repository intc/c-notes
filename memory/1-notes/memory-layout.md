# Memory layout for C programs

## Links

* https://www.cs.bgu.ac.il/~espl171/wiki.files/ch07lev1sec6.html (good)

## Memory structure (from high to low)

### STACK (grows down)

* Automatic variables inside fucntions
* Information which is saved when a function is called

Each time a function is called, the return address and environment related values (machine registers) are saved to stack. Newly called function allocates memory from the stack for it's automatic and temporary variables (this also allows recursion). Each time a recursive function calls its self, a new stack frame is used (so variables between different instances of same function will interfere with each other).

### HEAP (grows up)

Place for dyanmic memory allocation ( malloc() etc ).

### Uninitialized data (bss - block started by symbol)

* Initialized to zero by exec

Data is initialized by kernel to NULL pointers or arithemitc 0 before program starts.

example: int ia[100]; /* intorduced outside any function ia will store in uninitialized data segment. */

### Data segment

* Read from program file by exec

Initialized Data Segment. Contains variables initialized in the program.

example: int c = 12;  /* if intorduced outside any function c will be stored in data segment with it's inital value */

### Text segment

* Read from program file by exex

The machine instructions which CPU executes. Can be shareable (single copy for several instance of the same program?).

Usually read only.

## Notes

There probablt will be additional sections, which depends on the used binary format (like ELF or a.out) and debugging options etc.
