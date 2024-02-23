#ifndef SYSTEMD_H
#define SYSTEMD_H

int service_start(const char* service);
int service_stop(const char* service);

int service_status(const char* service);

int server_poweroff(void);
int server_reboot(void);

#endif /* SYSTEMD_H */

