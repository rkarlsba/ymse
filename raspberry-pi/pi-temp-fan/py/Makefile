# vim:ts=4:sw=4:sts=4:noet:ai
all:
	@echo Only valid target is 'install'

install:
	install -m 0755 -o root -g root pi-tempctl.py /usr/local/sbin
	install -m 0644 -o root -g root pi-tempctl.conf /usr/local/etc
	install -m 0644 -o root -g root pi-tempctl.service /etc/systemd/system

