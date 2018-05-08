		lw		0		1		zero	/reg1 == 0
		lw		0		2		ten		/reg2 == 10
		lw		0		3		one		/reg3 == 1
loop	beq		1		2		exit	/if reg1 == reg2 goto exit
		add		1		3		1		/reg1 += 1
		beq		0		0		loop	/goto loop
exit	halt
zero	.fill	0
one		.fill	1
ten		.fill	10	
