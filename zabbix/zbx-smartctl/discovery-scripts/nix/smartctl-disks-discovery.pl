#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# Changelog {{{
#
# Version 1.6
# Changed by Roy Sigurd Karlsbakk <roy@karlsbakk.net> 2024
#
# - Added check to see if we're actually running as root
#
# - Added addressing disks by /dev/disk/by-id to avoid errors hanging on old
#   device names (typical Linux issue). This only takes into account device
#   names starting with ata-. If others are needed, it shouldn't be hard, but
#   I don't need them.
#
# The code could be more beautiful and currently only supports drivenames
# starting with ata-, but it works™
#
# }}}
# /dev/disk/by-id {{{
#
# ata-INTEL_SSDSC2CT120A3_BTMP302002XW120BGN-part1@	 --> /dev/sdf1
# ata-INTEL_SSDSC2CT120A3_BTMP302002XW120BGN-part2@	 --> /dev/sdf2
# ata-INTEL_SSDSC2CT120A3_BTMP302002XW120BGN@	 --> /dev/sdf
# ata-INTEL_SSDSC2CT120A3_BTMP30210007120BGN-part1@	 --> /dev/sdb1
# ata-INTEL_SSDSC2CT120A3_BTMP30210007120BGN-part2@	 --> /dev/sdb2
# ata-INTEL_SSDSC2CT120A3_BTMP30210007120BGN@	 --> /dev/sdb
# ata-ST16000NM001G-2KK103_WL20KC89@	 --> /dev/sdc
# ata-ST16000NM001G-2KK103_WL20KPMP@	 --> /dev/sdd
# ata-ST16000NM001G-2KK103_ZL2PQKRQ@	 --> /dev/sde
# ata-ST16000NM001G-2KK103_ZL2PS47V@	 --> /dev/sda
# ata-Samsung_SSD_850_EVO_500GB_S21JNXBGC22428V-part1@	 --> /dev/sdg1
# ata-Samsung_SSD_850_EVO_500GB_S21JNXBGC22428V-part2@	 --> /dev/sdg2
# ata-Samsung_SSD_850_EVO_500GB_S21JNXBGC22428V@	 --> /dev/sdg
#
# }}}

use warnings;
use strict;
use Getopt::Long;
use Data::Dumper;
use File::Basename;

my $VERSION = 1.6;

#add path if needed into $smartctl_cmd
my $smartctl_cmd = "/usr/sbin/smartctl";
die "Unable to find smartctl. Check that smartmontools package is installed.\n" unless (-x $smartctl_cmd);
my $sg_scan_cmd = "/usr/bin/sg_scan";
my $nvme_cmd = "/usr/sbin/nvme";
my @input_disks;
my @global_serials;
my @smart_disks;

# Option flags
my $opt_help = 0;
my $opt_debug = 0;
my $opt_by_id = 0;
my $opt_by_path = 0;
my $opt_by_uuid = 0;

my $helpstr = <<EOL;
Usage: $0 [ -i | -p | -u | -d | -h ]

    -h | --help     This help
    -d | --debug    Enable debugging (multiple -d asks for even more)

    -i | --by-id    Get device names from /dev/disk/by-id
    -p | --by-path  Get device names from /dev/disk/by-path (not implemented)
    -u | --by-uuid  Get device names from /dev/disk/by-uuid (not implemented)
EOL

sub help {
    my $err = shift;
    if (defined($err)) {
        print STDERR "ERROR: $err\n$helpstr\n";
        exit 1;
    } else {
        print "$helpstr\n";
        exit 0;
    }
}

# Must be run as root
if ($< ne 0) {
    print STDERR "This must be run as root!\n";
    exit(1);
}

# Get and parse options
Getopt::Long::Configure('bundling');
GetOptions(
    "h" =>  \$opt_help,     "help"  => \$opt_help,
    "d+" => \$opt_debug,    "debug+" => \$opt_debug,
    "i" =>  \$opt_by_id,    "by-id" => \$opt_by_id,
    "p" =>  \$opt_by_path,  "by-path" => \$opt_by_path,
    "u" =>  \$opt_by_uuid,  "by-uuid" => \$opt_by_uuid,
);

&help if ($opt_help);

# Sanity check
if ($opt_by_path or $opt_by_uuid) {
    &help('Option not supported');
}


# by providing additional positional arguments you can add disks that are hardly discovered otherwise.
# for example to force discovery of the following disks provide '/dev/sda_-d_sat+megaraid,00 /dev/sda_-d_sat+megaraid,01' as arguments
if (@ARGV > 0) {
    foreach my $disk_line (@ARGV) {
        $disk_line =~ s/_/ /g;
        my ($disk_name) = $disk_line =~ /(\/(.+?))(?:$|\s)/;
        my ($disk_args) = $disk_line =~ /(-d [A-Za-z0-9,\+]+)/;
        if (!defined($disk_args)) {
           $disk_args = '';
        }


        if ( $disk_name and defined($disk_args) ) {
            push @input_disks,
                {
                    disk_name => $disk_name,
                    disk_args => $disk_args
                };
        }
    }
}


if ( $^O eq 'darwin' ) {    # if MAC OSX (limited support, consider to use smartctl --scan-open)
    while ( glob('/dev/disk*') ) {
        if ( $_ =~ /\/(disk+[0-9])$/ ) { 
            push @input_disks,
              {
                disk_name => $1,
                disk_args => ''
              };
        }
    }
} else {
    foreach my $line (@{[
        `$smartctl_cmd --scan-open`,
        `$smartctl_cmd --scan-open -dnvme`
        ]}) {

        #my $testline = "# /dev/sdc -d usbjmicron # /dev/sdc [USB JMicron], ATA device open" ;
        #for ($testline) {
        #splitting lines like  "/dev/sda -d scsi # /dev/sda, SCSI device"
        #"/dev/sda [SAT] -d sat [ATA] (opened)" # in debian 6 and smartctl 5.4
        #"/dev/sda -d sat # /dev/sda [SAT], ATA device" # in debian 8 and smartctl 6.4
        #"/dev/bus/0 -d megaraid,01" for megaraid
        #"# /dev/sdc -d usbjmicron # /dev/sdc [USB JMicron], ATA device open "

        my ($disk_name) = $line =~ /(\/(.+?))(?:$|\s)/;
        my ($disk_args) = $line =~ /(-d [A-Za-z0-9,\+]+)/;

        if ( $disk_name and $disk_args ) {
            push @input_disks,
              {
                disk_name => $disk_name,
                disk_args => $disk_args
              };
        }

    }

    if (-x $sg_scan_cmd){
        foreach my $line (`$sg_scan_cmd -i`) {
            ## sg_scan -i
            # https://github.com/v-zhuravlev/zbx-smartctl/pull/29
            #/dev/sg0: scsi0 channel=0 id=0 lun=0
            #    ATA       TOSHIBA MG03ACA1  FL1D [rmb=0 cmdq=1 pqual=0 pdev=0x0] 
            #/dev/sg1: scsi0 channel=1 id=0 lun=0
            #    Dell      Virtual Disk      1028 [rmb=0 cmdq=1 pqual=0 pdev=0x0] 
            #/dev/sg2: scsi0 channel=0 id=1 lun=0
            #    ATA       TOSHIBA MG03ACA1  FL1D [rmb=0 cmdq=1 pqual=0 pdev=0x0]
            if ($line =~ /(\/(.+?)):/){
                    my ($disk_name) = $1;
                    my ($disk_args) = "";

                    push @input_disks,
                        {
                            disk_name => $disk_name,
                            disk_args => $disk_args
                        };
            }

        }
    }

    if (-x $nvme_cmd){
        foreach my $line (`$nvme_cmd list`) {
            ## sg_scan -i
            # Node             SN                   Model                                    Namespace Usage                      Format           FW Rev
            # ---------------- -------------------- ---------------------------------------- --------- -------------------------- ---------------- --------
            # /dev/nvme0n1     S3W8NX0M10ZZZZ       SAMSUNG MZVLB512HAJQ-00000               1          18.87  GB / 512.11  GB    512   B +  0 B   EXA7301Q
            # /dev/nvme1n1     S3W8NX0M15ZZZZ       SAMSUNG MZVLB512HAJQ-00000               1         511.77  GB / 512.11  GB    512   B +  0 B   EXA7301Q
            if ($line =~ /(\/(?:.+?))\s/){
                    my ($disk_name) = $1;
                    my ($disk_args) = "";

                    push @input_disks,
                        {
                            disk_name => $disk_name,
                            disk_args => $disk_args
                        };
            }

        }
    }
}

foreach my $disk (@input_disks) {

    my @output_arr;
    #initialize disk defaults:
    $disk->{disk_model}='';
    $disk->{disk_sn}='';
    $disk->{subdisk}=0;
    $disk->{disk_type}=2; # other

    if ( @output_arr = get_smart_disks($disk) ) {
        push @smart_disks, @output_arr;
    }

}

json_discovery( \@smart_disks );

# ata-ST16000NM001G-2KK103_WL20KPMP@	 --> /dev/sdd
sub get_diskname_by_id {
    my $diskname = shift;
    my $devpath = '/dev/disk/by-id';
    my $basediskname = basename($diskname);
    my $reldiskname = "../../$basediskname";
    if (opendir(my $dh, $devpath)) {
        while (my $devlinkfile = readdir($dh)) {
            next unless ($devlinkfile =~ /^(ata-.*)/);
            next if ($devlinkfile =~ /-part[0-9]/);
            my $linktarget = readlink("$devpath/$devlinkfile");
            if ($linktarget eq $reldiskname) {
                $diskname = "$devpath/$devlinkfile";
            }
        }
        closedir $dh;
    } else { 
        warn "Can't open directory $devpath: $!";
    }
    readdir
    return $diskname;
}

sub get_smart_disks {
    my $disk = shift;
    my @disks;

    $disk->{smart_enabled} = 0;

    chomp( $disk->{disk_name} );
    chomp( $disk->{disk_args} );

    if ($opt_by_id) {
        $disk->{disk_name} = get_diskname_by_id($disk->{disk_name});
    }
    
    $disk->{disk_cmd} = $disk->{disk_name};
    if (length($disk->{disk_args}) > 0){
        $disk->{disk_cmd}.=q{ }.$disk->{disk_args};
        if ( $disk->{subdisk} == 1 and $disk->{disk_args} =~ /-d\s+[^,\s]+,(\S+)/) {
            $disk->{disk_name} .= " ($1)";
        }
    }

    #my $testline = "open failed: Two devices connected, try '-d usbjmicron,[01]'";
    #my $testline = "open device: /dev/sdc [USB JMicron] failed: Two devices connected, try '-d usbjmicron,[01]'";
    #if ($disk->{subdisk} == 1) {
    #$testline = "/dev/sdb -d usbjmicron,$disk->{disk_args} # /dev/sdb [USB JMicron], ATA device";
    #}
    my @smartctl_output = `$smartctl_cmd -i $disk->{disk_cmd} 2>&1`;
    foreach my $line (@smartctl_output) {
        #foreach my $line ($testline) {
        #print $line;
        if ( $line =~ /^(?:SMART.+?: +|Device supports SMART and is +)(.+)$/ ) {

            if ( $1 =~ /Enabled/ ) {
                $disk->{smart_enabled} = 1;
            }
            #if SMART is disabled then try to enable it (also offline tests etc)
            elsif ( $1 =~ /Disabled/ ) {
                foreach (`smartctl -s on -o on -S on $disk->{disk_cmd}`)
                {
                    if (/SMART Enabled/) { $disk->{smart_enabled} = 1; }
                }
            }
        }
    }
    my $vendor = '';
    my $product = '';
    foreach my $line (@smartctl_output) {
        # SAS: filter out non-disk devices (enclosure, cd/dvd)
        if ( $line =~ /^Device type: +(.+)$/ ) {
                if ( $1 ne "disk" ) {
                    return;
                }
        }
        # Areca: filter out empty slots
        if ( $line =~ /^Read Device Identity failed: empty IDENTIFY data/ ) {
            return;
        }
    
    
        
        if ( $line =~ /^serial number: +(.+)$/i ) {
                $disk->{disk_sn} = $1;
        }
        if ( $line =~ /^Device Model: +(.+)$/i ) {
                $disk->{disk_model} = $1;
        }

        #for NVMe disks and some ATA: Model Number:
        if ( $line =~ /^Model Number: +(.+)$/i ) {
                $disk->{disk_model} = $1;
        }
        #for SAS disks: Model = Vendor + Product
        elsif ( $line =~ /^Vendor: +(.+)$/ ) {
                $vendor = $1;
                $disk->{disk_model} = $vendor;
        }
        elsif ( $line =~ /^Product: +(.+)$/ ) {
                $product = $1;
                $disk->{disk_model} .= q{ }.$product;
        }
        
        if ( $line =~ /Rotation Rate: (.+)/i ) {

            if ( $1 =~ /Solid State Device/i ) {
                $disk->{disk_type} = 1;
            }
            elsif( $1 =~ /rpm/ ) {
                $disk->{disk_type} = 0;
            }
        }

        if ( $line =~ /Permission denied/ ) {

            warn $line;

        }
        elsif ( $disk->{subdisk} == 0 and $line =~ /failed: [A-zA-Z]+ devices connected, try '(-d [a-zA-Z0-9]+,)\[([0-9]+)\]'/) {
            #check for usbjmicron: "open failed: Two devices connected, try '-d usbjmicron,[01]'"
            # or "open device: /dev/sdc [USB JMicron] failed: Two devices connected, try '-d usbjmicron,[01]'"
            #not $disk->{subdisk} works as a guard against endless recursion

            foreach ( split //, $2 ) {    #splitting [01]

                push @disks,
                  get_smart_disks(
                    {
                        disk_name => $disk->{disk_name},
                        disk_args => $1 . $_,
                        subdisk   => 1
                    }
                  );

            }
            return @disks;

        }
        
    }
    
    # Areca SATA RAID
    if ( $disk->{subdisk} == 0 and $vendor eq "Areca" and $product eq "RAID controller" ) {
        for (my $i = 1; $i <= 16; $i++) {
            push @disks,
                get_smart_disks(
                    {
                        disk_name => $disk->{disk_name},
                        disk_args => "-d areca,$i",
                        disk_model => '',
                        disk_sn => '',
                        subdisk   => 1
                    }
                );
        }
        return @disks;
    }

    if ( $disk->{disk_name} =~ /nvme/ or $disk->{disk_args} =~ /nvme/) {
            $disk->{disk_type} = 1; # /dev/nvme is always SSD
            $disk->{smart_enabled} = 1;
    }
    # if disk_type is still unknown after parsing then rerun with extended -a:
    if ( $disk->{disk_type} == 2) {
            foreach my $extended_line (`$smartctl_cmd -a $disk->{disk_cmd} 2>&1`){

                #search for Spin_Up_Time or Spin_Retry_Count
                if ($extended_line  =~ /Spin_/){
                    $disk->{disk_type} = 0;
                    last;
                }
                #search for SSD in uppercase
                elsif ($extended_line  =~ / SSD /){
                    $disk->{disk_type} = 1;
                    last;
                }
                #search for NVME
                elsif ($extended_line  =~ /NVMe/){
                    $disk->{disk_type} = 1;
                    last;
                }
                #search for 177 Wear Leveling Count(present only on SSD):
                elsif ($extended_line  =~ /177 Wear_Leveling/){
                    $disk->{disk_type} = 1;
                    last;
                }
                #search for 231 SSD_Life_Left (present only on SSD)
                elsif ($extended_line  =~ /231 SSD_Life_Left/){
                    $disk->{disk_type} = 1;
                    last;
                }
                #search for 233 Media_Wearout_Indicator (present only on SSD)
                elsif ($extended_line  =~ /233 Media_Wearout_/){
                    $disk->{disk_type} = 1;
                    last;
                }
            }
    }
    
    # push to global serials list
    if ($disk->{smart_enabled} == 1){
        #print "Serial number is ".$disk->{disk_sn}."\n";
        if ( grep /$disk->{disk_sn}/, @global_serials ) {
            # print "disk already exists skipping\n";
            return;
        }
        push @global_serials, $disk->{disk_sn};
    }

    push @disks, $disk;
    return @disks;

}



sub escape_json_string {
    my $string = shift;
    $string =~  s/(\\|")/\\$1/g;
    return $string;
}

sub json_discovery {
    my $disks = shift;

    my $first = 1;
    print "{\n";
    print "\t\"data\":[\n";

    foreach my $disk ( @{$disks} ) {

        print ",\n" if not $first;
        $first = 0;
        print "\t\t{\n";
        print "\t\t\t\"{#DISKMODEL}\":\"".escape_json_string($disk->{disk_model})."\",\n";
        print "\t\t\t\"{#DISKSN}\":\"".escape_json_string($disk->{disk_sn})."\",\n";
        print "\t\t\t\"{#DISKNAME}\":\"".escape_json_string($disk->{disk_name})."\",\n";
        print "\t\t\t\"{#DISKCMD}\":\"".escape_json_string($disk->{disk_cmd})."\",\n";
        print "\t\t\t\"{#SMART_ENABLED}\":\"".$disk->{smart_enabled}."\",\n";
        print "\t\t\t\"{#DISKTYPE}\":\"".$disk->{disk_type}."\"\n";
        print "\t\t}";

    }
    print "\n\t]\n";
    print "}\n";

}
