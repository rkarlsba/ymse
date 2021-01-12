#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai

use strict;
use warnings;

my %otp;
my $cmd = 'vcgencmd otp_dump';
open my $fd,"$cmd |" || die "Can't open command '$cmd': $!\n";

my $args = $#ARGV+1;

# 15:00000000
# 16:00280000
# 17:1020000a
while (my $line = <$fd>) {
    chomp($line);
    if ($line =~ /^([0-9]+)\:([0-9a-f]+)/) {
        $otp{$1} = $2;
    }
}

if ($args == 0) {
    foreach my $key (sort keys %otp) {
        # print "$key\n";
        print "$key: $otp{$key}\n";
    }
} elsif ($args == 1) {
    my $bit = sprintf("%02d", $ARGV[0]);

    # https://www.raspberrypi.org/documentation/hardware/raspberrypi/otpbits.md
    if ($bit == 17) {
        printf("    10987654321098765432109876543210\n");
        printf("$bit: % 32b\n", hex($otp{$bit}));

        # Bit 1: sets the oscillator frequency to 19.2MHz
        # Bit 3: enables pull ups on the SDIO pins
        # Bit 19: enables GPIO bootmode
        # Bit 20: sets the bank to check for GPIO bootmode
        # Bit 21: enables booting from SD card
        # Bit 22: sets the bank to boot from
        # Bit 28: enables USB device booting
        # Bit 29: enables USB host booting (ethernet and mass storage)

        printf("\n");

        my $not = "";
        $not = " not" unless (hex($otp{$bit}) & (1<<1));
        printf("Oscillator frequency$not to 19.2MHz\n");

        $not = "";
        $not = " not" unless (hex($otp{$bit}) & (1<<3));
        printf("Pull ups on the SDIO pins$not enabled\n");

        $not = "";
        $not = " not" unless (hex($otp{$bit}) & (1<<19));
        printf("GPIO boot mode is$not enabled\n");

        $not = "";
        $not = " not" unless (hex($otp{$bit}) & (1<<20));
        printf("Bank is$not set to check GPIO bootmode\n");

        $not = "";
        $not = " not" unless (hex($otp{$bit}) & (1<<21));
        printf("Boot from SD card is$not enabled\n");

        $not = "";
        $not = " not" unless (hex($otp{$bit}) & (1<<22));
        printf("Bank to boot from is set to %d\n", (hex($otp{$bit}) & (1<<22)) >> 22);

        $not = "";
        $not = " not" unless (hex($otp{$bit}) & (1<<28));
        printf("Machine is$not USB bootable\n");

        $not = "";
        $not = " not" unless (hex($otp{$bit}) & (1<<29));
        printf("Machine is$not network bootable\n");
    } elsif ($bit == 28) {
        printf("Serial number: %x (%d)\n", hex($otp{$bit}), hex($otp{$bit}));
    } elsif ($bit == 29) {
        printf("Inverse serial number: %x (%d)\n", hex($otp{$bit}), hex($otp{$bit}));
    } else {
        printf("Dunno what to do with bit $bit\n");
        exit(0);
    }
} elsif ($args == 2) {
    for (my $i=0; $i<32; $i++) {
        printf("Bit %02d: %08x\n", $i, 1<<$i);
    }
} else {
    print STDERR "Syntax: $0 [bitnum]\n";
    exit(1);
}
