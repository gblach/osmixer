#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <Elementary.h>

struct cmdl_t {
    int help:1;
    char *mixer;
} cmdl = {
    0,
    "/dev/mixer",
};

Evas_Object *win, *slider;

void volume_init() {
    win = elm_win_util_standard_add("osmixer", "On Screen Mixer");
    elm_win_autodel_set(win, EINA_TRUE);
    elm_win_borderless_set(win, EINA_TRUE);

    slider = elm_slider_add(win);
    elm_slider_span_size_set(slider, 200);
    elm_slider_min_max_set(slider, 0, 100);
    elm_slider_unit_format_set(slider, "%2.0f%%");
    elm_win_resize_object_add(win, slider);
    evas_object_show(slider);
}

int volume_read(const int mixer) {
    int vol;
    ioctl(mixer, SOUND_MIXER_READ_VOLUME, &vol);
    return vol;
}

void volume_display(int vol, int toggle) {
    ecore_thread_main_loop_begin();

    if(toggle) {
        if(! evas_object_visible_get(win)) {
            int screen_height, win_height;
            int x_margin = 25, y_margin = 20;

            elm_win_screen_size_get(win, NULL, NULL, NULL, &screen_height);
            evas_object_geometry_get(win, NULL, NULL, NULL, &win_height);
            evas_object_move(win, x_margin, screen_height - win_height - y_margin);
            evas_object_show(win);
        }

        elm_slider_value_set(slider, vol / 257);
    } else {
        evas_object_hide(win);
    }

    ecore_thread_main_loop_end();
}

void *volume_loop() {
    int mixer, vol_prev = 0, vol, counter = 0;

    mixer = open(cmdl.mixer, O_RDONLY);

    while(1) {
        vol = volume_read(mixer);

        if(vol_prev != vol) {
            vol_prev = vol;
            counter = 8;
        }

        if(counter > 0) counter--;
        volume_display(vol, counter);

        usleep(100000);
    }

    close(mixer);
    return NULL;
}


void terminate(int sig) {
    elm_exit();
    exit(0);
}

EAPI_MAIN int elm_main(int ac, char **av) {
    pthread_t thr_win;

    while(--ac && av++) {
        if(!strcmp(*av, "-h")) cmdl.help = 1;
        if(!strcmp(*av, "-m")) cmdl.mixer = *(av+1);
    }

    if(cmdl.help) {
        puts("On Screen Mixer - displays audio volume while it's changed");
        puts("./osmixer [-h] [-m /dev/mixer]");
        puts("  -h  display this help message");
        puts("  -m  select mixer device");
        return 0;
    }

    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);

    pthread_create(&thr_win, NULL, (*volume_loop), NULL);

    volume_init();
    elm_run();
    elm_shutdown();

    return 0;
}

ELM_MAIN()
