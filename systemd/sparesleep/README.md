# sparesleep 

*Put spares into autosleep and other drives into non-sleep*

## License

This is written by [Roy Sigurd Karlsbakk](mailto:roy@karlsbakk.net) and licensed under the
[AGPL](https://www.gnu.org/licenses/agpl-3.0.en.html) license. See the LICENSE.md file for
details.

## Usage

```bash
autosleep [mdX]
```

Make all drives in an md RAID or just the md RAID specified, not sleep during normal operation,
but those drives designated as spares (with (S) at the end in /proc/mdadm) sleep most of the
time.
