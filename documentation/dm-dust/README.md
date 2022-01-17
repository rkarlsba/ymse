[//]: # vim:tw=80

# dm-dust

Devices used for this test will be **sdg** and **sdh**, 16GB each, without partition
tables.

## Setup

### Connect the (virtual) disks to a dm-dust device each

```bash
# Check block size on disk
blockdev --getbsz /dev/sdg
512

# Check disk size
blockdev --getsz /dev/vdg
33554432
```

Each disk is mapped to a dm-dust device, so use the numbers from above.

```bash
# map the first one
dmsetup create dust0 --table '0 33554432 dust /dev/sdg 0 512'
# Repeating for sdh, since it's just as big
dmsetup create dust1 --table '0 33554432 dust /dev/sdh 0 512'
```

Check the status of the read behavior ("bypass" indicates that all I/O will be
passed through to the underlying device). The following dd commands will
overwrite the head of the dust device and move the nulls back whence they came
(from nowehere):

```
dmsetup status dust0
0 33554432 dust 8:96 bypass verbose
dmsetup status dust1
0 33554432 dust 8:112 bypass verbose

dd if=/dev/mapper/dust0 of=/dev/null bs=512 count=128 iflag=direct
128+0 records in
128+0 records out

dd if=/dev/zero of=/dev/mapper/dust0 bs=512 count=128 oflag=direct
128+0 records in
128+0 records out
```

### Create a RAID-1 (aka mirror) on the dm-dust devices

```bash
mdadm --create /dev/md/dust --level=1 --raid-devices=2 /dev/mapper/dust[01]^C
mdadm: Note: this array has metadata at the start and
    may not be suitable as a boot device.  If you plan to
    store '/boot' on this device please ensure that
    your boot-loader understands md/v1.x metadata, or use
    --metadata=0.90
Continue creating array? y
mdadm: Defaulting to version 1.2 metadata
mdadm: array /dev/md/dust started.
```

Check mdstat

```bash
cat /proc/mdstat
Personalities : [raid1] [raid6] [raid5] [raid4] [linear] [multipath] [raid0] [raid10]
md127 : active raid1 dm-4[1] dm-3[0]
      16759808 blocks super 1.2 [2/2] [UU]
      [=>...................]  resync =  5.9% (1001856/16759808) finish=1.3min speed=200371K/sec
```

### Configure LVM and filsystem

After creating the RAID, create a new LVM volume group (VG) and a logical volume
(LV) and then make a filesystem on top. I'll use xfs here, whatever floats your
boat.

```bash
vgcreate dust /dev/md/dust
lvcreate -n dust -l 100%FREE dust
mkfs -t xfs /dev/dust/dust
mkdir /dust
mount /dev/dust/dust /dust
```

### Creaate "bad blocks"

```bash
sudo dmsetup message dust1 0 addbadblock 60
kernel: device-mapper: dust: badblock added at block 60

dmsetup message dust1 0 addbadblock 67
kernel: device-mapper: dust: badblock added at block 67

dmsetup message dust1 0 addbadblock 72
kernel: device-mapper: dust: badblock added at block 72
```

