        lw      0       1       num     /reg1 = 234
        lw      0       2       one     /reg2 = 1
        noop
        noop
        beq     0       0       branch  /goto branch CONTROL HAZARD occur
        add     1       2       3       /reg3 = num + 1 not taken
branch  add     0       1       3       /reg3 = num = 234
        halt
num     .fill   234
one     .fill   1
