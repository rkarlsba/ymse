#!/bin/sh
# Søk etter log4j i alle jar-filer. Forutsetter at locate-databasen er 
# oppdatert. På grunn av problematikk med mellomrom i katalog og filnavn 
# legges listen over jar-filer i en temporær fil.

locate -r "\.jar$" >/tmp/jars-$$
# find / -name '*.jar' -print >/tmp/jars-$$ # Alternativ om locate mangler

REGEX="log4j\.jar|log4j-[0-9].*\.jar$|log4j-core.*\.jar$"
while read line; do
  HIT=`unzip -l "$line" 2>/dev/null | egrep -i "$REGEX"`
  if [ -n "$HIT" ]; then
    echo -e "$line\n$HIT\n"
    HIT=""
  fi
done < /tmp/jars-$$

rm /tmp/jars-$$
