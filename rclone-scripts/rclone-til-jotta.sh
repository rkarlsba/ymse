#!/bin/bash
# vim:ts=4:sw=4:si:ruler

# Simple locker example by Roy Sigurd Karlsbakk <roy@karlsbakk.net>

# Sett variabler
PROGNAME=rsync-til-jotta
LOCKDIR=/var/lock
LOCKFILE=$LOCKDIR/$PROGNAME.lock
# AUTOCLEANUP=1 # her stopper det opp for meg første gang - denne er visst ikke i bruk :þ kanskje noe jeg la til som variabel først og så fant ut at var teit å ikke ha der. ok, men hvorfor 1 og ikke 0 eller 2? eller noe annet? 1 er true, 0 er false. ok

# Trap gjør at den ikke vil avslutte plutselig med disse signalene, de vanlige, uten å fjerne låsefila
# Trap specific signals (not EXIT)
trap "{ rm -f $LOCKFILE ; exit 1; }" SIGHUP SIGINT SIGQUIT SIGTERM # her stopper det opp for andre gang. trap, fant ut med noe gaggling at det var en innebygd sak i bash i alle fall. den kjører subrutina inni {} hvis den mottar SIGHUP, SIGINT, SIGQUIT eller SIGTERM, som er vanlige signaler for å stoppe en prosess. Hvordan vet den at den mottar en av de? fordi den har kjørt en 'trap' på dem, så kan den se hvilket signal den mottar og sånt - hadde ikke trap vært der, hadde den bare dødd. bare dødd? ja - kill -9 123 dreper 123 effektivt, kill -15 (eller uten -15) sender en SIGTERM og ber prosessen om å stoppe - med mindre den da ikke har trappa SIGTERM, da bare dør den. Så det er bare en måte å be den rydde opp litt etter seg hvis den får en kill. Hm, ja, men jeg skjønner ikke hva den gjør :) og hvorfor det står kill ->noe> om det ikek står trap der. så om vi fjerner trap, så står det kill noe der? ikke noe kill her - det er en under - en kill -0 som ikke dreper noe, men bare sjekker om en prosess kjører. Så den sjekker om prosessen $LOCKFILE kjører? nei - den henter ut PID-en lagra i $LOCFILE først, men den sjekken kommer jo senere, leses ikke scriptene linje for linje? jo - men trap her er for å unngå at dette skriptet - uansett utgang - ikke skal drepes utenfra uten at det rydder opp etter seg. ok, sikkert. Jeg må ente klær jeg. Det skjønner jeg hvordan jeg gjør. en "exit 1" eller vanlig exit vil ikke trigge trap over.

# Sjek kom låsefilas katalog eksisterer og kan skrives til
# Check if lockfile directory exits is writable and exit if not
if [ ! -d $LOCKDIR -o ! -w $LOCKDIR ]; then
	echo "Lockfile directory $LOCKDIR either doesn't exists or isn't writable by me - exiting" >&2
	exit 1;
fi

# Sjekk om låsefila eksisterer
# Check if lockfile exists
if [ -f $LOCKFILE ]; then
# Sett $oldpid til PID henta fra låsefila - denne kunne kanskje skrives om til
	oldpid=$( cat $LOCKFILE )
#	oldpid=`cat $LOCKFILE`

	# Check if old PID is numeric
# Sjekk om PID er numerisk - skrives om litt
#if `echo $oldpid | grep "^[0-9][0-9]*$" > /dev/null 2>&1`; then
	if $( echo $oldpid | grep "^[0-9][0-9]*$" > /dev/null 2>&1 ); then
		# Check if process is running
# ja - og sjekk om den prosessen kjører - kill -0 sender ikke noe signal til prosessen - bare sjekker om den lever
		if $( kill -0 $oldpid > /dev/null 2>&1 ); then
			echo "Old process running as PID $oldpid - exiting" >&2
			exit 1
		else
			echo "Old lockfile found, but no process running as pid $oldpid - removing lockfile" >&2
			rm -f $LOCKFILE
		fi
	else
		echo "Old lockfile found containing invalid PID - removing lockfile" >&2
		rm -f $LOCKFILE
	fi
fi

# Og så hvis alt går bra, snurr film! ok, men jeg skjønner ikke noe av det. Jeg ser bare tekst og at om ditten og datten og sånt

# Create lockfile
echo $$ > $LOCKFILE

# Insert code here
export TMPDIR=/data/tmp/rclone
rclone -q sync /data jotta-crypt:

# Clean up lock
rm -f $LOCKFILE#!/bin/bash

