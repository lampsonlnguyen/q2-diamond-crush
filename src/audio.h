/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef DIAMOND_AUDIO_H
#define DIAMOND_AUDIO_H
/* Q2 Pod passes the PCM device and optional software volume in argv.
   Events: 1 invalid, 2 match, 3 combo, 4 select. */
int audio_start(const char *app_path, const char *device, const char *volume);
void audio_play(int event);
void audio_stop(void);
#endif
