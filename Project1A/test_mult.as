		lw		0		2		mcand	/reg2 == 32766
		lw		0		3		num1	/reg3 == 1
		lw		0		4		mplier	/reg4 == 10383
loop	beq		0		3		done	/reg3 == 0 goto done
		nor		2		2		5		/and step 1
		nor		3		3		6		/and step 2
		nor		5		6		5		/and fin reg5 == mcand & reg3
		add		3		3		3		/shift left reg3
		beq		0		5		mult	/reg5 == 0  goto mult
		add		1		4		1		/reg1 += mplier
		add		4		4		4		/shift left mplier
		beq 	0		0		loop	/goto loop
mult	add		4		4		4		/shift left mplier
		beq		0		0		loop	/goto loop
done	halt
num1	.fill	1
mcand	.fill	32766
mplier	.fill	10383
