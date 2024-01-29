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
supported ones, check if *file(1)* beleive their content to be *data* This is
synonomous to *something* or *greek* or the analoue to the doctor telling you
you're a schizophrenic, (which doesn't say shit about you or your illness, of
which the doctor doesn't understand either, but that you're completely out of
your mind). However, since *file* uses ZZ

Written in a cold and snowy winter in Oslo, Norway, by [mailto:roy@karlsbakk.net](Roy Sigurd Karlsbakk) in the bloody year of 2024
