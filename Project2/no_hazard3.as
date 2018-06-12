        lw      0       1       num     /reg1 = 234
        nor     0       0       2       /reg2 = -1
        noop
        noop
        noop
        add     2       2       2       /reg2 = -2
        noop
        noop
        noop
        nor     2       2       2       /reg2 = 1
        noop
        noop
        noop
        add     1       2       3       /reg3 = num + 1 = 235
        noop
        noop
        noop
        sw      0       3       0       /data[0] = reg3 = 235
        halt
num     .fill   234
