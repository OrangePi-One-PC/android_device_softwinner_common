/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "audio_utils"
#define LOG_NDEBUG 0

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <cutils/log.h>

#include <tinyalsa/asoundlib.h>

#include <autil/audio_utils.h>
#define _DEBUG_

#ifdef _DEBUG_
#define _ALOGV(...)    ALOGI(__VA_ARGS__)
#define _ALOGI(...)    ALOGI(__VA_ARGS__)
#define _ALOGW(...)    ALOGW(__VA_ARGS__)
#define _ALOGT(...)    ALOGI(__VA_ARGS__)
#define _ALOGE(...)    ALOGE(__VA_ARGS__)
#else
#define _ALOGV(...)
#define _ALOGI(...)
#define _ALOGW(...)
#define _ALOGT(...)    ALOGI(__VA_ARGS__)
#define _ALOGE(...)    ALOGE(__VA_ARGS__)
#endif


void get_audio_card_dev(const char *audio_device_name, int stream, int *_card, int *_device)
{
    _ALOGV("%s() %s", __func__, audio_device_name);
    int card, dev, err;
    char *pcm_id;
    char *ret;
    struct mixer *mixer;
    snd_pcm_info_t *pcminfo;
    pcm_info_alloca(&pcminfo);

    card = -1;
    if(mixer_card_next(&card) < 0 || card < 0) {
        _ALOGE("%s() no soundcards found......", __func__);
        goto next_card;
    }
	_ALOGV("%s() %d", __func__, card);

    while (card >= 0) {
		_ALOGE("%s() card =%d", __func__,card);
        if(!(mixer = mixer_open(card))) {
            _ALOGE("%s() mixer_open err", __func__);
            mixer_close(mixer);
            goto next_card;
        }

        dev = -1;
		_ALOGV("%s() %p", __func__, mixer);
        while (1) {
            if(mixer_ctl_pcm_next_device(mixer, &dev) < 0) {
                _ALOGE("%s() get pcm next device err", __func__);
            }
            if (dev < 0) break;

            pcm_info_set_device(pcminfo, dev);
            //pcm_info_set_subdevice(pcminfo, 0);
            pcm_info_set_stream(pcminfo, stream);

            if ((err = mixer_ctl_pcm_info(mixer, pcminfo)) < 0) {
                if (err != -ENOENT) {
                    _ALOGE("%s() control digital audio info (%i): %s", __func__, card, strerror(err));
                }
                continue;
            }

            pcm_id = pcm_info_get_id(pcminfo);
            ALOGV("%s() PCM_ID = (%s)", __func__, (pcm_id) ? pcm_id : NULL);
            if (pcm_id) {
                if(!(ret = strstr(pcm_id, audio_device_name))) {
                    //_ALOGE("%s() %s does not have the substring %s", __func__, pcm_id, audio_device_name);
                } else {
                    *_card = card;
                    *_device = dev;
                    mixer_close(mixer);
                    _ALOGI("%s() (card, dev) = (%d, %d)", __func__, card, dev);
                    return;
                }
            }
        }
        mixer_close(mixer);

next_card:
        if (mixer_card_next(&card) < 0) {
            _ALOGE("%s() get next card err!", __func__);
            break;
        }
    }

    *_card = -1;
    *_device = -1;
}

void get_audio_card_num(const char *audio_device_name, int *_card)
{
    _ALOGV("%s() %s", __func__, audio_device_name);
    int err;
	int i = 0,nLen = 0;
	FILE *fd = NULL;
	for(i = 0;i < CARD_NUM_MAX;i++){
		char *buf;
		char card_id[sizeof(SND_SYS_SOUND_PATH) + 10];
        sprintf(card_id, SND_SYS_SOUND_PATH, i);
		_ALOGV("%s() %s", __func__, card_id);
		fd = fopen(card_id, "r");
		if(!fd){
            _ALOGV("%s() get card %d id fail.", __func__,i);
			continue;
		}
		//fseek(fd, 0, SEEK_END);
		//nLen = ftell(fd);
		nLen = 10;
		//rewind(fd);
		buf = (char*)malloc(sizeof(char)*nLen);
		if(!buf){
            _ALOGE("%s() alloc memory fail.", __func__);
			fclose(fd);
			return;
        }
		nLen = fread(buf, sizeof(char), nLen, fd);
		//_ALOGE("%s() buf = %s", __func__,buf);
		if(!(err = strncmp(buf,audio_device_name,sizeof(audio_device_name)))){
            *_card = i;
			fclose(fd);
			free(buf);
			return;
        }
		fclose(fd);
		free(buf);
    }
}

void pcm_params_get_max_min(int card, int dev, enum pcm_stream stream, enum pcm_param param,
        unsigned int *rmax, unsigned int *rmin) {
    int min = 0, max = 0;
    struct pcm_params *params;

    params = pcm_params_get(card, dev, stream == PCM_STREAM_PLAYBACK ? PCM_OUT : PCM_IN);
    if (params == NULL) {
        _ALOGE("Device does not exist.");
    }

    min = pcm_params_get_min(params, param);
    max = pcm_params_get_max(params, param);
    *rmin = min;
    *rmax = max;

    pcm_params_free(params);
}

void pcm_params_get_rate_max_min(int card, int dev, enum pcm_stream stream,
        unsigned int *rmax, unsigned int *rmin) {
    pcm_params_get_max_min(card, dev, stream,
            PCM_PARAM_RATE, rmax, rmin);
}

void pcm_params_get_channel_max_min(int card, int dev, enum pcm_stream stream,
        unsigned int *rmax, unsigned int *rmin) {
    pcm_params_get_max_min(card, dev, stream,
            PCM_PARAM_CHANNELS, rmax, rmin);
}

void pcm_params_get_format_max_min(int card, int dev, enum pcm_stream stream,
        unsigned int *rmax, unsigned int *rmin) {
    pcm_params_get_max_min(card, dev, stream,
            PCM_PARAM_SAMPLE_BITS, rmax, rmin);
}

/* period size */
void pcm_params_get_ps_max_min(int card, int dev, enum pcm_stream stream,
        unsigned int *rmax, unsigned int *rmin) {
    pcm_params_get_max_min(card, dev, stream,
            PCM_PARAM_PERIOD_SIZE, rmax, rmin);
}

/* period count */
void pcm_params_get_pc_max_min(int card, int dev, enum pcm_stream stream,
        unsigned int *rmax, unsigned int *rmin) {
    pcm_params_get_max_min(card, dev, stream,
            PCM_PARAM_PERIODS, rmax, rmin);
}
