#!/bin/bash

mdadm --grow --size=10 /dev/md/raidtest:md-fsckup
