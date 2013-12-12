.func longjmp
#include <setjmp.h>
void longjmp( jmp_buf env, int return_value );
.ixfunc2 'Non-local jumps' &func
.synop end
.desc begin
The &func function restores the environment saved by the most recent
call to the
.kw setjmp
function with the corresponding
.kw jmp_buf
argument.
.np
It is generally a bad idea to use &func to jump out of an interrupt
function or a signal handler (unless the signal was generated by the
.kw raise
function).
.desc end
.return begin
After the &func function restores the environment, program
execution continues as if the corresponding call to
.kw setjmp
had just returned the value specified by
.arg return_value
.ct .li .
If the value of
.arg return_value
is 0, the value returned is 1.
.return end
.see begin
.seelist longjmp setjmp
.see end
.exmp begin
#include <stdio.h>
#include <setjmp.h>

jmp_buf env;

rtn()
  {
    printf( "about to longjmp\n" );
    longjmp( env, 14 );
  }
.exmp break
void main()
  {
    int ret_val = 293;

    if( 0 == ( ret_val = setjmp( env ) ) ) {
      printf( "after setjmp %d\n", ret_val );
      rtn();
      printf( "back from rtn %d\n", ret_val );
    } else {
      printf( "back from longjmp %d\n", ret_val );
    }
  }
.exmp output
after setjmp 0
about to longjmp
back from longjmp 14
.exmp end
.class ANSI
.system
