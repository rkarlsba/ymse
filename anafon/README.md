[//]: # vim:tw=80

# BT-greier - En liten oversikt

I fila *pi-oversikt.txt* er det en oversikt over diverse pi-er og hva
bluetoothctl sier om dem. Nye flagg, som Handsfree Audio Gateway leler Audio
Source dukker opp etter hvert som de aktiveres. *bluealsa* aktiverer noen av
dem.

systemctl status bluetooth.service gir feilmeldinger

Jan 02 10:33:50 whitepi bluetoothd[510]: Sap driver initialization failed.
Jan 02 10:33:50 whitepi bluetoothd[510]: sap-server: Operation not permitted (1)
Jan 02 10:33:50 whitepi bluetoothd[510]: Failed to set privacy: Rejected (0x0b)

https://raspberrypi.stackexchange.com/questions/40839/sap-error-on-bluetooth-service-status
sier man bare kan skru av SAP, SIM Access Profile i /etc/systemd/system/bluetooth.target.wants/bluetooth.service

ExecStart=/usr/lib/bluetooth/bluetoothd --noplugin=sap

og…

Jan 02 10:40:15 whitepi bluetoothd[511]: Failed to set privacy: Rejected (0x0b)

https://unix.stackexchange.com/questions/382031/bluez-error-setting-privacy-on-raspbian

bluetoothctl-skript

```
sudo bluetoothctl <<EOF
power on
discoverable on
pairable on
agent NoInputNoOutput
default-agent 
EOF
```

eller

```
sudo bluetoothctl <<EOF
agent off
agent NoInputNoOutput
discoverable on
pairable on
default-agent 
EOF
```


------------------------------------------------------------------------------------------------
root@anafon:~# hciconfig -a
hci0:	Type: Primary  Bus: UART
	BD Address: B8:27:EB:29:FE:1C  ACL MTU: 1021:8  SCO MTU: 64:1
	UP RUNNING PSCAN ISCAN
	RX bytes:1532 acl:0 sco:0 events:98 errors:0
	TX bytes:3587 acl:0 sco:0 commands:98 errors:0
	Features: 0xbf 0xfe 0xcf 0xfe 0xdb 0xff 0x7b 0x87
	Packet type: DM1 DM3 DM5 DH1 DH3 DH5 HV1 HV2 HV3
	Link policy: RSWITCH SNIFF
	Link mode: SLAVE ACCEPT
	Name: 'anafon'
	Class: 0x680000
	Service Classes: Capturing, Audio, Telephony
	Device Class: Miscellaneous,
	HCI Version: 4.1 (0x7)  Revision: 0x1fc
	LMP Version: 4.1 (0x7)  Subversion: 0x2209
	Manufacturer: Broadcom Corporation (15)
------------------------------------------------------------------------------------------------
hci0:	Type: Primary  Bus: UART
	BD Address: B8:27:EB:29:FE:1C  ACL MTU: 1021:8  SCO MTU: 64:1
	UP RUNNING PSCAN
	RX bytes:1532 acl:0 sco:0 events:98 errors:0
	TX bytes:4276 acl:0 sco:0 commands:98 errors:0
	Features: 0xbf 0xfe 0xcf 0xfe 0xdb 0xff 0x7b 0x87
	Packet type: DM1 DM3 DM5 DH1 DH3 DH5 HV1 HV2 HV3
	Link policy: RSWITCH SNIFF
	Link mode: SLAVE ACCEPT
	Name: 'anafon'
	Class: 0x680000
	Service Classes: Capturing, Audio, Telephony
	Device Class: Miscellaneous,
	HCI Version: 4.1 (0x7)  Revision: 0x1fc
	LMP Version: 4.1 (0x7)  Subversion: 0x2209
	Manufacturer: Broadcom Corporation (15)

---------
funka ikke - prøver med noe annet

pip3 install bluedot

