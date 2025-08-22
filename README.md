# gnirt
Print the strings of printable characters in files, but in the way I want => in the worst way possible, without using any library.

Because of inline assembly, only Apple Silicon devices are supported at the moment.

## Removed libraries
- stdio.h: custom string print function using unistd write (`print`)
- string.h: custom implementation of memset() to zero a buffer (`clean_buffer`)
- ctype.h: custom implementation of isprint (`is_printable`)
- stdlib.h: custom implementation of atoi (`atoi`)
- sys/stat.h: input file is read on-the-fly in chunks with read() instead of copying it all in a right size-preallocated buffer
- fcntl.h: replaced open() with system call invocation via inline assembly (`open`)
- unistd.h: replaced read() and write() with native assembly syscall, like I already did with fcntl.h open()

22/08/2025: did it! 0 libraries!

## FAQs

- Is this yet another version of GNU strings? - yes, but I did it
- Why "gnirt"? - because is a reversed substring of "strings" and I liked the way it sounded
- Does it make sense to use this? - no, it's just a challenge to write a functioning program with a sort of sense while trying to reinvent the wheel by implementing commonly used library function
- So it's just a playground for YOU? you creep! - ...