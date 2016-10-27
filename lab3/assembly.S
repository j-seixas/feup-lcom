#include "i8042.h"

.globl _assembly
.text
_ass:
xor %eax,%eax
L1:
inb $STATUS_PORT  //reads the value at that port and copies it into the al register
test $OUT_BUF_FULL, %al
jz
