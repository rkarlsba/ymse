#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Simple benchmarking program ripped from https://unix.stackexchange.com/questions/28756/what-is-the-most-high-performance-linux-filesystem-for-storing-a-lot-of-small-fi/28792 # 
# Rewritten slightly to support macos and to run under python3 by Roy Sigurd Karlsbakk <roy@karlsbakk.net> in September 2021.

filecount = 3_00_000
filesize = 1024

import random, time
from os import system
from sys import platform

if platform == "linux" or platform == "linux2":
    # Linux
    flush = "sudo su -c 'sync ; echo 3 > /proc/sys/vm/drop_caches'"
elif platform == "darwin":
    # OS X
    flush = "purge"
elif platform == "win32":
    # Windows...
    print("I DON'T LIKE WINDOWS!!!!!!")
    exit(1)

# Most OSes should take the rest, now that windows is out of the way
randfile = open("/dev/urandom", "r")

print("\ncreate test folder:")
starttime = time.time()
system("rm -rf test && mkdir test")
print(time.time() - starttime)
system(flush)

print("\ncreate files:")
starttime = time.time()
for i in range(filecount):
    rand = randfile.read(int(filesize * 0.5 + filesize * random.random()))
    outfile = open("test/" + unicode(i), "w")
    outfile.write(rand)
print(time.time() - starttime)
system(flush)

print("\nrewrite files:")
starttime = time.time()
for i in range(int(filecount / 10)):
    rand = randfile.read(int(filesize * 0.5 + filesize * random.random()))
    outfile = open("test/" + unicode(int(random.random() * filecount)), "w")
    outfile.write(rand)
print(time.time() - starttime)
system(flush)

print("\nread linear:")
starttime = time.time()
for i in range(int(filecount / 10)):
    infile = open("test/" + unicode(i), "r")
    outfile.write(infile.read());
print(time.time() - starttime)
system(flush)

print("\nread random:")
starttime = time.time()
outfile = open("/dev/null", "w")
for i in range(int(filecount / 10)):
    infile = open("test/" + unicode(int(random.random() * filecount)), "r")
    outfile.write(infile.read());
print(time.time() - starttime)
system(flush)

print("\ndelete all files:")
starttime = time.time()
system("rm -rf test")
print(time.time() - starttime)
system(flush)

