#!/usr/bin/python3

import random

blocksize=512
devsize=8*1024**3;

random.seed()
print("dd if=/dev/urandom of=/dev/sds bs="+str(blocksize)+" seek="+str(random.randrange(devsize-blocksize)))
