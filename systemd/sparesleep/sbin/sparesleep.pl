#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

# eksempel på /proc/mdstat - åpne opp med zo og lukk med zc {{{
=pod
Personalities : [raid1] [linear] [multipath] [raid0] [raid6] [raid5] [raid4] [raid10]
md1 : active raid6 sdc[9] sdn[10] sdi[13] sdj[14] sda[7] sdf[11] sde[15] sdd[8] sdb[16]
      15627063296 blocks super 1.2 level 6, 512k chunk, algorithm 2 [10/8] [_UUUUUUU_U]
      [=====>...............]  recovery = 25.2% (493554884/1953382912) finish=384.4min speed=63284K/sec
      bitmap: 15/15 pages [60KB], 65536KB chunk

md3 : active raid1 sdg1[3] sdm1[2]
      116176768 blocks super 1.2 [2/2] [UU]
=cut
# }}}

my $debug = 0;
my $mdstat_fn = '/proc/mdstat';

if ($debug gt 0) {
    $mdstat_fn = '../mdstat-test.txt';
}

open my $mdstat,"cat $mdstat_fn|" || die "kan ikke lese $mdstat_fn";
while (my $line = <$mdstat>) {
    chomp($line);
    next unless ($line =~ /^md[0-9]/);
    my @elements = split(/ /, $line);
    my $md = $elements[0];
    my @disks;
    my @spares;
    for (my $i = 4 ; ; $i++) {
        if ($elements[$i]) {
            my $d = $elements[$i];
            $d =~ s/\[\d+\]//;
            if ($d =~ /\(S\)/) {
                $d =~ s/\(S\)//;
                push @spares,$d;
            } else {
                push @disks,$d;
            }
        } else {
            last;
        }
    }
    print "$md:\n" if ($debug gt 1);
    print "Disks:\t" . join(' ', @disks) . "\n" if ($debug gt 1);
    print "Spares:\t" . join(' ', @spares) . "\n" if ($debug gt 1);
    foreach my $spare (@spares) {
        my $cmd = "hdparm -B noe? /dev/$spare";
        print "$cmd\n";
        # system($cmd); 
        #
        # Fjern # foran system($cmd); og legg til # foran print($cmd); der, så kan du bruke den i prod!
        #
        # Masse prat - åpne opp med zo og lukk med zc {{{
        # Linjen over "system($cmd);  avkommenters når du ser at $cmd virker fra kommandolinja i test? - ja - når du har testa $cmd, som skrives
        # ut med print der, så kan du automere den med system(). Skrevet her som 'system($cmd);' stemmer dette? jaja - det er bare vanlig å
        # angi systemkall uten parametre hvis du skal nevne funksjon. Ok, men sånt fører også til forvirring om man ikke vet sånt, derfor, inn med teskje-dokumentasjon, så alle kan forstå
        # ja - men system($cmd) kjører akkurat *den* kommandoen, mens system() kjører hva du vil ;)
        # joda, men her er det jo den kommandoen med det systemkallet det er snakk om? jeg trur du forstår. system() uten parametre gjør ingenting og fører bare til feil.
        # ja, jeg skjønner, men poenget mitt er at dokumentasjon som tar ting litt med teskje er mye enklereå  skjønne enn at man må vite en masse ting man kanskje ikke vet, og vips er forvirringen i full sving. Dårlig dokumentasjon er kanskje ett av de største problemene vi har innen IT.
        # Real programmers never comment their code - the code is obvious!
        # For den ene personen kanskje ja :)
        # I et halvt års tid - eller to - og så ehh - hva var det jeg gjorde her…
        # Jepp, og det illustrerer jo nok en gang poenget mitt :)
        # Tja - her er det jo implisitt at parametret i system() er $cmd, så det er ikke noe poeng å nevne det.
        # Nei, ikke om man vet det fra før, men om man ikke vet sånt, er det ikke så dumt å ha det med.
        # Men det står jo i linja over!
        # For en som ikke vet så mye om dette, kan det oppfattes som to forskjellige ting.
        # Vel - nå veit du :)
        # ja, og derfor tar jeg det med, for da er det så mye enklere for meg å huske at ah, ja, sånn var det ja, i stedet for. eh, hæ, hva skjer her?!
        # Det er ikke jeg som har funnet på den måten å dokumentere ting på - det er veldig, veldig vanlig, siden når man snakker om et systemkall, er det kallet
        # og ikke argumentene som er poenget. Så kommer man gjerne til argumentene seinere. Prøv å trykk K på system. Ja, jeg klarer ikke å lese det der nå. Men jeg skjønner at det ikke er du som har funnet på å dokumentere slik, jeg bare sier at måten dokumentasjon funker på innen IT, ofte er vanskelig å forstå.
        # Du blir vant til det ;) Sjekker du dokumentasjonen på "rm", så beskriver den jo "rm" og så først seinere kommer parameterne.
        # jo, men der står det i allefall greit i en liste tenker jeg. Selv om man blir vant til dårlig dokumentasjon, betyr det jo ikke at det er en bra ting osv osv :)
        # Da blar du ned til eksempler - det er ofte det første jeg gjør ;)
        # Ja, men ofte kan eksemplene være dårlige også, men ikke alltid, men alt ofr ofte er det enten ikke eksempler, eller de er vanskelig å forstå
        # Dette er ikke noe bare jeg mener osv heller, uten at det blir mer sant eller feil osv av den grunn, men jeg står ved det poenget. Det er for mye dårlig dokumentasjon ogd årlige eksempler, men ikke alltid. Det er ikke bare slik.
        # joda - har du prøvd å lese dokumentasjonen til openssl?
        # ikke prøv engang ;) Ikke prøvd, men igjen, der ser du :)  jada - jeg veit! Men det betyr ikke at det jeg skreiv over var fullstendig uforståelig - det
        # var bare en notasjon du ikke var vant til. Nå veit du. Jepp :) enig i det også <3 :) men fint at dette funker. For det så veldig nyttig ut :)
        # Skriptet gjør jo mye mer enn å trekke ut spares, det trekker jo ut disklista også, ferdig vaska, så det kan jo brukes til andre ting også hvis du vil
        # Jepp :)
        # Men uansett - dette ser jo ut til å virke
        # Linjen "print "$cmd\n"; <-- kommenters ut i prod
        # eh - "kommenteres ut" blir brukt på begge her. Det jeg mener er at i test, har du print "$cmd"; mens i prod har du system($cmd);
        # Ah
        # }}}
    }
}
