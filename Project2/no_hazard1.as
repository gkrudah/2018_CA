        lw      0       1       num1    /reg1 = 1
        lw      0       2       num2    /reg2 = 2
        lw      0       3       num3    /reg3 = 3
        lw      0       4       num4    /reg4 = 4
        sw      0       1       0       /data[0] = reg1 = 1
        sw      0       2       1       /data[1] = reg2 = 2
        sw      0       3       2       /data[2] = reg3 = 3
        sw      0       4       3       /data[3] = reg4 = 4
        halt
num1    .fill   1
num2    .fill   2
num3    .fill   3
num4    .fill   4
