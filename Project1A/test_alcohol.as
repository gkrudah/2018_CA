		noop	/enough age to drink alcohol?
		lw		0		1		age		/reg1 == 17
		lw		0		2		valid	/reg2 == 20
		lw		0		3		year	/reg3 == 1
test	beq		1		2		drink	/if reg1 == reg2 goto drink
		beq		0		0		Aging	/else goto Aging
drink	halt	/you can drink alcohol!
Aging	add		1		3		1		/reg1 += 1
		beq		0		0		test	/a year later try again!
age		.fill	17
valid	.fill	20
year	.fill	1
