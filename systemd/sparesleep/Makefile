all:
	@echo 'Type make install to install systemd config and script'

install:
	install -b sbin/sparesleep.pl /usr/local/sbin
	install -m 0644 -b systemd/sparesleep.service /etc/systemd/system
	systemctl daemon-reload
	systemctl enable sparesleep.service
	systemctl start sparesleep.service
