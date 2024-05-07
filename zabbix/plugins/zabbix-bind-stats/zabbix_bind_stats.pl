#!/usr/bin/perl
# Dmitry Maksimov 18.03.2009 dima_dm@hotmail.com
# Named statistics for Zabbix
#######
# Need!!! perl-suidperl-5.8.5-36.el4_6.3.i386.rpm or later
# Permission on script
# chmod 4755 zabbix_bind_stats.pl
# ls -l zabbix_bind_stats.pl
# -rwsr-xr-x  1 root root 1585 Mar 18 12:06 zabbix_bind_stats.pl
# See detail in Zabbix Linux Server Monitoring.doc 
#### Config ######
my $rndc='/usr/sbin/rndc';
my $retry=5; # if error
my $sleep=5; # in sec
my $data=300; # in byte
my $maxsize=5000000; # in byte $stats file
my $stats='/var/named/chroot/var/named/named.stats';
my $stat_file='/tmp/zabbix_named.stats.txt';
#### End Config ###
my $i;
my $code=1;
my $success;
my $referral;
my $nxrrset;
my $nxdomain;
my $recursion;
my $failure;
delete @ENV{qw(IFS CDPATH ENV BASH_ENV)}; # Make %ENV safer
$ENV{'PATH'}="/usr/local/bin:/bin:/usr/bin";
while ($code>0 && $i<$retry)
 {
 $i++;
# $code=system("$rndc stats >/dev/null  2>&1");
 $code=system("$rndc stats");
 if ($code>0) {sleep($sleep);}
 }
my $position=(stat($stats))[7];
if ($position<$data) 
	{
	$position=0;
	}
	else {$position=$position-$data;}

open(FILE,"$stats");
seek(FILE,$position,0); 
while(<FILE>)
 {
 if (/^success\s+(\d+)/) {$success=$1;}
 if (/^referral\s+(\d+)/) {$referral=$1;}
 if (/^nxrrset\s+(\d+)/) {$nxrrset=$1;}
 if (/^nxdomain\s+(\d+)/) {$nxdomain=$1;}
 if (/^recursion\s+(\d+)/) {$recursion=$1;}
 if (/^failure\s+(\d+)/) {$failure=$1;}
 if (/Statistics Dump ---\s+\((\d+)\)/) {$date=$1;} 
 }
close(FILE);
if ((stat($stats))[7]>$maxsize) 
	{
	open(FILE,">$stats");
	close(FILE);
	}
open(FILE,">$stat_file");
print FILE "date: $date ",scalar(localtime($date)),"\n";
print FILE "success: $success\n";
print FILE "referral: $referral\n";
print FILE "nxrrset: $nxrrset\n";
print FILE "nxdomain: $nxdomain\n";
print FILE "recursion: $recursion\n";
print FILE "failure: $failure\n";
close(FILE);
if ($code>0) 
 {
 print "2\n";
 }else
  {
  print "1\n";
  }
