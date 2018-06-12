        lw      0       1       num     /reg1 = 234
        nor     0       0       2       /reg2 = -1
        add     2       2       2       /reg2 = -2 DATA_HAZARD occur
        nor     2       2       2       /reg2 = 1 DATA_HAZARD occur
        add     1       2       3       /reg3 = num + 1 = 235
        sw      0       3       0       /data[0] = reg3 = 235 DATA HAZARD occur
        halt
num     .fill   234
