#!/bin/bash

selinuxstatus=$( getenforce )
case $selinuxstatus in
	Permissive)
		;;
		ZZ
