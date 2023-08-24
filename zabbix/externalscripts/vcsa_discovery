#!/usr/bin/perl
$first = 1;
print "{\n";
print "\t\"data\":[\n\n";
$vcsa = "<158.36.68.81>";
$OID = ".1.3.6.1.2.1.25.4.2.1.4";
for (`snmpwalk -c public -v 2c  $vcsa $OID`)
    {
#	($service) = m/((?<=\/).*?((?=\.)|(?=\")))/;
        ($service) = m/([^\/]+(?!=\"))$/;
        ($vmware) = m/("\/.*?vmware.*?")/;
	if (defined $vmware)
	    {
		print "\t,\n" if not $first;
		$first = 0;
		print "\t{\n";
		print "\t\t\"{#VMWARESVC}\":\"$service\,\n";
		print "\t\t\"{#PARAMETERS}\":$vmware\n";
		print "\t}\n";
	    }
    }
print "\n\t]\n";
