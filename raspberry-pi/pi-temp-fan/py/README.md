[comment]: vim:ai:tw=100

[comment]: yellowpi: Raspberry Pi 4 Model B Rev 1.1
[comment]: greenpi:  Raspberry Pi 3 Model B Rev 1.2
[comment]: redpi:    Raspberry Pi 3 Model B Rev 1.2
[comment]: bluepi:   Raspberry Pi 2 Model B Rev 1.1

pi-tempctl.py
==================

In the config file for pi-tempctl.py, pi-tempctl.conf, the following values may
be set under the [default] section.

| Setting               | Meaning                                      | Type  | Default |
|-----------------------|----------------------------------------------|-------|--------:|
| temperaure_threashold | Temperature at which to turn on/off fan      | int   | 45      |
| fan_pin               | Pin connected to fan (and driver)            | int   | 0       |
| poll_delay            | Number of seconds to delay between each poll | float | 1       |
| hysteresis_avoidance  | Times to probe threshold before toggling fan | int   | 1       |
| debug_log             | Log to LOG_DEBUG every x probe               | int   | 0       |

Written around 2020 by [Roy Sigurd Karlsbakk](mailto:roy@karlsbakk.net). Licensed under [AGPL3](https://www.gnu.org/licenses/agpl-3.0.html).
