all:
	@echo 'Type make install to install systemd config and script'

install:
	install -b sbin/enable-scterc-on-all-drives.sh /usr/local/sbin
	install -m 0644 -b systemd/scterc.service /etc/systemd/system
	systemctl daemon-reload
	systemctl enable scterc.service
	systemctl start scterc.service
