/*
 * vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
 *
 * iwspeed - small test to fetch iwspeed in Hz
 *
 * Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net> 2014-02
 *
 * Released under the BSD-3-Clause License
 * See https://opensource.org/license/bsd-3-clause/ for details
 */
#include <iwlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[]) {
    int s;
    struct iwreq wrq;
    if (iw_get_ext(skfd, ifname, SIOCGIWRATE, &wrq) >= 0) {
        printf("yey\n");
    }
//   17400 ioctl(3, SIOCGIWRATE, 0x7e949f58) = 0
//   17400 ioctl(3, SIOCGIWPOWER, 0x7e949f58) = 0

}
