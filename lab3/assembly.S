#include "i8042.h"

.global _assembly
.text
_assembly:
		xor %eax,%eax
CICLE:
		inb $STATUS_PORT  //reads the value at that port and copies it into the al register
		test $OUT_BUF_FULL, %al
		jz CICLE
		inb $KBD_OUT_BUF
		ret

