#!/bin/bash

SRC_SCRIPT='bin/
DST_SCRIPT='/usr/local/sbin/avahi-add-names.sh'
CONFIG='/etc/default/avahi-add-names'

if [ -f $SCRIPT ]
then
	mv -f $SCRIPT $SCRIPT.bk
fi

├── bin
│   └── avahi-add-names.sh
├── conf
│   └── avahi-extra-names
├── do-install.sh
├── Makefile
└── README.md

2 directories, 5 files
