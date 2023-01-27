# mkvm.sh

Simple script to clone a predefined VM template to a new VM

Syntax:
```bash
mkvm.sh my-new-vm
```

The script adds the defined domain part to the vm and clones the template to
this new one. Only one vdisk is supported and used in this operation. You can
obviously type this out manually with virt-clone, but I'm lazy ;)

[Roy Sigurd Karlsbakk](roy@karlsbakk.net)
