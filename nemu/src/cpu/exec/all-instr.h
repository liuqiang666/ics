#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);

make_EHelper(nemu_trap);

//control.c
make_EHelper(call);		
make_EHelper(ret);		
make_EHelper(jcc);		

//data-mov.c
make_EHelper(push);	
make_EHelper(pop);		
make_EHelper(lea);		
make_EHelper(movzx);		
make_EHelper(leave);		

//arith.c
make_EHelper(sub);
make_EHelper(add);
make_EHelper(adc);
make_EHelper(cmp);

//logic.c
make_EHelper(xor);		
make_EHelper(and);	
make_EHelper(or);	
make_EHelper(setcc);	
make_EHelper(test);	
make_EHelper(sar);	
make_EHelper(shl);	

//special.c
make_EHelper(nop);		

