/*
================================================================================
EX NO: 9  23Z511 – SYSTEM SOFTWARE LABORATORY
GDB DEBUGGING DEMONSTRATION
================================================================================
AIM:
  To use GDB debugging techniques to trace and inspect the execution of a 
  compiled C program.

GDB DEBUGGING COMMANDS & TRACE (from Lab Manual Pages 19-21):
  1. Compile with debug symbols:
       gcc -g sum.c -o sum
  2. Launch GDB:
       gdb ./sum
  3. View source listing:
       (gdb) list
  4. Set breakpoint at main():
       (gdb) break main
  5. Run program:
       (gdb) run
  6. Step line-by-line:
       (gdb) next
  7. Print variable values:
       (gdb) print a          -> $1 = 10
       (gdb) print b          -> $2 = 20
       (gdb) print sum        -> $3 = 30
  8. Inspect all local variables:
       (gdb) info locals      -> a = 10, b = 20, sum = 30
  9. Continue program execution:
       (gdb) continue         -> Sum = 30
  10. Inspect and manage breakpoints:
       (gdb) info breakpoints
       (gdb) delete 1
  11. Exit GDB:
       (gdb) quit
================================================================================
*/

#include <stdio.h>

int main(void)
{
    int a = 10;
    int b = 20;
    int sum;

    sum = a + b;

    printf("Sum = %d\n", sum);

    return 0;
}
