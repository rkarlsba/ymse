[comment]: vim:ai:tw=100

README
================================
These are some attempts to control USB power on raspberry pi.

I have a few 3d printers and I use raspberry pi with octoprint to control them. Sometimes it's
rather annoying to see the printer getting powered from USB from the pi directly, even if the
printer itself has been switched off. I don't know if this happens to all printers, but I know for
sure that for crealities, it sure does. This will result in a printer with a somewhat working
controller, unable to use the highpower parts (like heaters, fans etc). For some printers, like
Ender 3 and CR10S, it'll also result in an extremely annoying 10kHz sound humming in the
background. So if I can disable this altogether and have the printer powered only by its own power,
that'll help me a great lot. I haven't found a good way to do this, though. Below is what I've
tried.

pi-usb-pwr-ctl.sh
--------------------------------
Attempt to disable power lines on USB using sysfs calls. This fails to work on raspberry pi 3 and at
all propability on other pi machines as well.

pi-disable-usbpower.service
--------------------------------
This is an attempt to Disable USB power with [uhubctl](https://github.com/mvp/uhubctl), using a
small systemd script. While this works perfectly well, it not only disables USB power on the ports,
but also disable all use of them as well, meaning this is probably something you don't want in your
startup, but rather a command to run in case you want to shut down that whining printer without
leaving your chair.

[Roy Sigurd Karlsbakk](mailto:roy@karlsbakk.net)
