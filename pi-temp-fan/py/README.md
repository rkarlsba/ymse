# vim:ai:tw=100

pi-tempctl.py
==================

In the config file for pi-tempctl.py, pi-tempctl.conf, the following values may
be set under the [default] section.

| Setting               | Meaning                                       | Default |
|-----------------------|-----------------------------------------------|---------|
| temperaure_threashold | Temperature at which to turn on/off fan       | 45      |
| fan_pin               | Pin connected to fan (and driver)             | 0       |
| poll_rate             | Number of seconds to delay between each poll  | 45      |
| temperaure_threashold | Temperature at which to turn on/off fan       | 45      |
| hysteresis_avoidance  | Times to probe threshold before toggling fan3 | 1       |
|-----------------------|-----------------------------------------------|---------|
