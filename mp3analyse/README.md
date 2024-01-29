<!---
vim:ts=4:sw=4:sts=4:et:ai:tw=80
--->
# mp3analyse.pl

## Currently - hackish
Make a list of files and their apparent contents with

```bash
find /data/empetre/ -type f -exec file {} \; > /data/empetre/filetypes.txt &`
```

Now, read the list of files and check their extension and if it's in a set of
supported ones, check if ''file'' 

Written in a cold and snowy winter in Oslo, Norway, by [mailto:roy@karlsbakk.net](Roy Sigurd Karlsbakk) in the bloody year of 2024
