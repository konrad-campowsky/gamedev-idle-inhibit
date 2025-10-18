#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <systemd/sd-bus.h>


static int acquire_inhibitor_lock(const char *why) {
    sd_bus *bus = NULL;
    sd_bus_message *msg = NULL;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    int fd = -1, r;

    if ((r = sd_bus_open_system(&bus)) < 0) {
        fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
        return -1;
    }

    r = sd_bus_call_method(
        bus,
        "org.freedesktop.login1",
        "/org/freedesktop/login1",
        "org.freedesktop.login1.Manager",
        "Inhibit",
        &error,
        &msg,
        "ssss",                    // 4 strings:
        "idle",                    // what
        "gamepad-monitor",         // who
         why,                      // why
        "block"                    // mode
    );

    if (r < 0) {
        fprintf(stderr, "Failed to acquire inhibitor lock: %s\n", error.message);
        sd_bus_error_free(&error);
        sd_bus_unref(bus);
        return -1;
    }

    if ((r = sd_bus_message_read(msg, "h", &fd)) < 0) {
        fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
    } else {
        fd = dup(fd);
    }

    sd_bus_message_unref(msg);
    sd_bus_unref(bus);

    return fd;
}

int main(int argc, char* argv[]) {
    int fd, sig;
    sigset_t set;
    const char *why = argc > 1 && argv[1][0] ? argv[1] : "Unknown reason";

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);

    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        perror("sigprocmask");
        return 1;
    }

    printf("Inhibiting idle state because of %s\n", why);

    if ((fd = acquire_inhibitor_lock(why)) < 0) {
        return 1;
    }

    for (;;) {
        siginfo_t info;
        if ((sig = sigwaitinfo(&set, &info)) < 0) {
            if (errno == EINTR) 
                continue;
            perror("sigwaitinfo");
            break;
        }

        if (sig == SIGINT || sig == SIGTERM) {
            printf("received signal %d, shutting down...\n", sig);
            break;
        }
    }

    close(fd);

    return 0;
}

