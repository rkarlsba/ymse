all:
	@echo 'Try "make install" to install the restic backup scripts'

install:
	install -m 0755 backup-to-restic.sh /usr/local/bin
	install -m 0755 restic-backup-post-exec.sh /usr/local/bin
	install -m 0755 restic-backup-pre-exec.sh /usr/local/bin
	[ -f /etc/default/restic ] || install -m 644 config /etc/default/restic
	[ -f /usr/local/etc/restic-excludes.txt ] || install -m 0644 restic-excludes.txt /usr/local/etc
	[ -f /usr/local/etc/restic-password.txt ] || install -m 0600 restic-password.txt /usr/local/etc
