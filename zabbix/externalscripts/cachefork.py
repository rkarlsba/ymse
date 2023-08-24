#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Laget av Rolf.Holtsmark @ Oslo Met . no

# WORKFLOW:
#   hvis litt gammel cache:
#       fork en process som oppdaterer cache + fortsett nedover
#   hvis veldig gammel cache:
#       exit(100)  - mens vi venter til fork processen oppdaterer cachen
#   hvis ikke veldig gammel cache:
#       fil_stdout til stdout
#       fil_stderr til stderr
#       exit med exitcode som scriptet ga.

import os
import sys
import subprocess
import time
import hashlib
import random
import re

debug = False
# debug = True
argremove = 1

py3 = "python3"

if len(sys.argv) > 1 and sys.argv[1] == '0':
    argremove += 1

argsleft = sys.argv[argremove:]
if (len(sys.argv) < 2) or ( len(sys.argv) >= 2 and sys.argv[1] == '-h'):
    if len(sys.argv) > 1 and sys.argv[1] == '-h':
        argremove += 1
    sys.stderr.write(f'using character 0 for background fork process\n')
    sys.stderr.write(f'Usage: {sys.argv[0]} [-h|-s] <command to cache (execute with args)>\n')
    sys.exit(98)

dry_run = False
argsleft = sys.argv[argremove:]
if len(argsleft) >= 1 and (argsleft[0] in ['--dryrun', '--dry-run', '--status', '-s']):
    argremove += 1
    print(f"Status for files... DRY-RUN, no forking.. no executing script... (-s|--status|--dryrun)")
    debug = True
    dry_run = True

cachemaxperiod = 600  # 10 min
cacheperiod = 30  # cache minimum this period

cache_dir = f"/run/user/{os.getuid()}/cachefork_monitoring"  # feilet
cache_dir = f"/tmp/cachefork_monitoring"
# /run/user/1000/


# print(f"argremove: {argremove} {sys.argv}")

if not os.path.isdir(cache_dir):
     os.makedirs(cache_dir)

now = int(time.time())

last_complete = 0
last_start = 0
command = sys.argv[argremove:]

# if debug:
#	print(len(sys.argv), argremove)
#	print(sys.argv)
#	print(" ".join(command))


str_command = " ".join(command).encode()
# print(str(str_command))
# print(str_command[0:30])
basecmd = str(os.path.basename(str_command)[0:30].decode()).replace(' ', "_")
cmdprefix = "".join(re.findall(re.compile(r"[a-zA-Z0-9:_,.-]", re.IGNORECASE), basecmd))

hasher = hashlib.md5()
hasher.update(str_command)
basefile = cmdprefix + "-" + hasher.hexdigest()
f = cache_dir + os.sep + basefile

mother_pidfile = f + '-runcached.pid'
child_pidfile = f + '-child.pid'
child_child_pidfile = f + '-child-child.pid'

fil_stdout = f + '.stdout'
fil_stderr = f + '.stderr'

exitcodefile = f + '.exitcode'
commandfile = f + '.cmd'


if debug:
	print("DEBUG enabled, extra output")
	print("Files used with these parameters:")
	print(f"  pid-files: mother {mother_pidfile} 1st child {child_pidfile} 2nd child: {child_child_pidfile}")
	print(f"  output files: stdout: {fil_stdout} stderr: {fil_stderr} exitcodefile: {exitcodefile}")
	print(f"  commandfile: {commandfile}")

def getdiff_file(file):
    stamp = 0
    if os.path.isfile(file) and os.access(file, os.R_OK):
        try:
            stamp = int(os.path.getmtime(file))
        except Exception as e:
            if debug:
                print(f"got exception {e}")
            stamp = 0
    return now - stamp


# kommando med 0 og script og parametere kjører, sender til output fil
def runit(cmd, exitcodefile, cmdfile):
    print(f"Running {cmd} into {fil_stdout}.tmp.{now}")
    f_stdout = open(f"{fil_stdout}.tmp.{now}", 'w')
    if fil_stdout != fil_stderr:
        f_stderr = open(f"{fil_stderr}.tmp.{now}", 'w')
    else:
        f_stderr = f_stdout
    print(f"cmd: {cmd}")
    # subprocess.Popen(args, bufsize=0, executable=None, stdin=None, stdout=None, stderr=None, preexec_fn=None,
    # close_fds=False, shell=False, cwd=None, env=None, universal_newlines=False, startupinfo=None, creationflags=0)
    # p = subprocess.Popen(cmd, stdout=f_stdout, stderr=f_stderr, shell=True)
    p = subprocess.Popen(" ".join(cmd), stdout=f_stdout, stderr=f_stderr, shell=True)
    p.communicate()
    with open(child_child_pidfile, 'w') as f:
        f.write(str(p.pid))
    f_stdout.close()
    os.rename(f"{fil_stdout}.tmp.{now}", f"{fil_stdout}")
    if fil_stdout != fil_stderr:
        f_stderr.close()
        os.rename(f"{fil_stderr}.tmp.{now}", f"{fil_stderr}")

    exitcode = p.returncode
    with open(cmdfile, 'w') as f:
        f.write(str(cmd))
    with open(exitcodefile, 'w') as f:
        f.write(str(exitcode))


# hvis jeg først er fork, bare kjør kommandoen
if len(sys.argv) > 1 and sys.argv[1] == '0':
    runit(command, exitcodefile, commandfile)


# last_complete defineres av tidsstamp på exitcodefila hvis den finnes.
# exitcodefile lages helt til slutt etter scriptet ble kjørt ferdig.
# last_start defineres av tidsstamp på child_pidfile når den starter.
diff_complete = getdiff_file(exitcodefile)
diff_start = getdiff_file(child_pidfile)

# Hvis jeg er main process:
if len(sys.argv) > 1 and sys.argv[1] != '0':
    # print(f"{diff_complete=} {diff_start=} ")
    if debug:
	    print(f"diff_complete={diff_complete} diff_start={diff_start} ")

    if diff_complete > cacheperiod and diff_start > cacheperiod:
        if debug:
            print("forking subprocess..")
            print([py3, os.path.realpath(__file__), '0', " ".join(command)])
        # start_new_session forker processen. Krever minst python 3.2
        if not dry_run:
            p = subprocess.Popen([py3, os.path.realpath(__file__), '0', " ".join(command)], start_new_session=True,
                             stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
            with open(child_pidfile, 'w') as f:
                f.write(str(p.pid))
            if debug:
                # print(f"{p.pid=} {p.returncode=} {p.stderr=} {p.stdout=}")
                print(f"p.pid={p.pid} p.returncode={p.returncode} p.stderr={p.stderr} p.stdout={p.stdout}")
            # os.getsid(p.pid)

# hvis vi ikke har veldig gammel cache, les ut cache filene
if diff_complete <= cachemaxperiod:
    with open(fil_stdout, 'r') as f:
        sys.stdout.write(f.read())
    with open(fil_stderr, 'r') as f2:
        sys.stderr.write(f2.read())
    if os.path.isfile(exitcodefile):
        with open(exitcodefile, 'r') as fe:
            try:
                sys.exit(int(fe.read()))
            except Exception as e:
                # sys.stderr.write(f"Exception on reading and converting {exitcodefile=} to int() {e}")
                sys.stderr.write(f"Exception on reading and converting exitcodefile={exitcodefile} to int() {e}")
                sys.exit(99)
    else:
        # no exitcodefile
        sys.exit(102)


# if nothing else...
print("WARNING - waiting for cache or something is wrong.")
sys.exit(100)

