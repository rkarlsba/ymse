[comment]: vim:ai:tw=100

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
