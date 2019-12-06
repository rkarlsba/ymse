#!/bin/bash
mdadm --create --name md-fsckup --level=1 --raid-devices=2 /dev/md/md-fsckup /dev/sd[ab]

mdadm --detail --scan
