        lw      0       1       num1    /reg1 = 1
        lw      0       2       num2    /reg2 = 2
        lw      0       3       num3    /reg3 = 3
        noop
        add     0       1       4       /reg4 = 1
        add     1       2       5       /reg5 = reg1 + reg2 = 3
        noop
        noop
        add     3       4       6       /reg6 = reg3 + reg4 = 4
        noop
        noop
        noop
        add     5       6       7       /reg7 = reg5 +reg6 = 7       
        halt
num1    .fill   1
num2    .fill   2
num3    .fill   3
num4    .fill   4
