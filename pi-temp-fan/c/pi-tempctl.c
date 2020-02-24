/*
 * vim:ts=4:sw=4:sts=4:et:ai
 */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>

#define MS (1000)

const int fan_pin = 8;
const int poll_rate = 500*MS;
const char *temperature_file = "/sys/class/thermal/thermal_zone0/temp";
const float temperature_threashold = 55.0;
const int state_change_delay = 1500*MS;

void sighandler(int signum) {
    printf("Caught signal %d, coming out...\n", signum);
    digitalWrite(fan_pin, 0);
    exit(signum);
}

float get_temp() {
    int fd,bread;
    char buf[16];
    int mc;
    float c;

    fd = open(temperature_file, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening file '%s': %s\n", temperature_file, strerror(errno));
        exit(1);
    }
    bread = read(fd, buf, sizeof buf);
    if (bread < 0) {
        fprintf(stderr, "Error reading file '%s': %s\n", temperature_file, strerror(errno));
        exit(2);
    }
    close(fd);
    mc = strtol(buf, NULL, 10);
    c = (float)mc/1000;
    return c;
}

void fan_control(int state) {
    digitalWrite(fan_pin, state);
}

int main(void) {
    float temp;
    int state = 0;

    /* setup() */
    wiringPiSetup();
    pinMode (fan_pin, OUTPUT);
    signal(SIGINT, sighandler);
    signal(SIGHUP, sighandler);

    /* loop() */
    for (;;) {
        temp = get_temp();
        if (state == 0 && temp > temperature_threashold) {
            state=1;
            fan_control(state);
            printf("Turning on fan, as temp = %f (> %f)\n", temp, temperature_threashold);
        } else if (state == 1 && temp <= temperature_threashold) {
            state=0;
            fan_control(state);
            printf("Turning off fan, as temp = %f (< %f)\n", temp, temperature_threashold);
        } else {
            printf("Running along with state = %d and temp at %f (threashold %f)\n", state, temp, temperature_threashold);
        }
        usleep(poll_rate);
    }
    return 0;
}

