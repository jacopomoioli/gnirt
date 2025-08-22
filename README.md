# gnirt
An educational clone of GNU strings with no includes.

No real reason to use this, just an educational project to learn by reinventing the wheel and going low level-ish. Also, it's full of bugs.

At the moment it only works on apple silicon based systems, due to inline assembly.

## Removed includes
- stdio.h → Replaced with a custom `print()` using the raw write syscall.
- string.h → Replaced `memset()` with my own `clean_buffer()`.
- ctype.h → Replaced `isprint()` with my own `is_printable()`.
- stdlib.h → Replaced `atoi()` with a hand-rolled one.
- sys/stat.h → No file statting. File is read on-the-fly in chunks.
- fcntl.h → Replaced `open()` with a syscall via inline assembly.
- unistd.h → Replaced `read()` and `write()` with inline syscall wrappers, just like `open()`.

## FAQs
> **Is this yet another version of GNU strings?**<br>
yes, but I did it

> **Why "gnirt"?**<br> 
because is a reversed substring of "strings" and I liked the way it sounded

> **Does it make sense to use this?**<br>
no, it's just a challenge to write a functioning program with a sort of sense while trying to reinvent the wheel by implementing commonly used library function

> **So it's just a playground for YOU? you creep!**<br>
... that's not a question?