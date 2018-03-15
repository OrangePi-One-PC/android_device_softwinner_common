/*
 * Copyright (C) 2008 The Android Open Source Project
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

#define LOG_TAG "rtsoundeffects-jni"
#include <utils/Log.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"

#include <sys/ioctl.h>

#define MIC_EFFECT_MAJOR    179
#define MIC_EFFECT_MAGIC    'd'
#define MIC_EFFECT_IOCMAX   10
#define MIC_EFFECT_NAME     "mic_sound_effect"
#define MIC_EFFECT_SET      _IOW(MIC_EFFECT_MAGIC, 1, unsigned long)
#define MIC_EFFECT_GET      _IOW(MIC_EFFECT_MAGIC, 2, unsigned long)


#define MIC_SOUND_EFFECT_NODE "/dev/mic_sound_effect"
struct mic_effect_data{
    int mode;
    int param;
};

using namespace android;

static jint
com_karaoke_rtsoundeffects_setParam(JNIEnv* env,jobject thiz,jint mode,jint param)
{
    ALOGD("com_karaoke_rtsoundeffects_setParam(%d,%d)\n", mode, param);
    int err = 0;
    int fd = open(MIC_SOUND_EFFECT_NODE,O_RDWR);
    if(fd >= 0){
        mic_effect_data data;
        data.mode = mode;
        data.param = param;
        if(ioctl(fd,MIC_EFFECT_SET,&data)){
            err = -1;
            ALOGD("do MIC_EFFECT_SET fail");
        } 
        close(fd);
    }else {
        ALOGD("com_karaoke_rtsoundeffects_setParam open %s fail.\n",MIC_SOUND_EFFECT_NODE);
    }
    return err;
}

static jint
com_karaoke_rtsoundeffects_getParam(JNIEnv* env,jobject thiz,jint mode)
{
    ALOGD("com_karaoke_rtsoundeffects_getParam(%d)\n", mode);
    int ret = 0;
    int fd = open(MIC_SOUND_EFFECT_NODE,O_RDWR);
    if(fd >= 0){
        mic_effect_data data;
        data.mode = mode;
        if(ioctl(fd,MIC_EFFECT_GET,&data)){
            ret = -1;
            ALOGD("do MIC_EFFECT_GET fail");
        } else {
            ret = data.param;
            ALOGD("com_karaoke_rtsoundeffects_getParam(data.param%d)\n", data.param);
        }
        close(fd);
    }else {
        ALOGD("com_karaoke_rtsoundeffects_getParam open %s fail.\n",MIC_SOUND_EFFECT_NODE);
    }
    return ret;
}

static JNINativeMethod gMethods[] = {
	{"native_setParam",   "(II)I", (void *)com_karaoke_rtsoundeffects_setParam},
	{"native_getParam", "(I)I", (void *)com_karaoke_rtsoundeffects_getParam},
};


static const char *gClassPathName = "com/karaokeimpl/kRTSoundEffects";

int32_t register_com_karaoke_rtsoundeffects(JNIEnv *env){
    return AndroidRuntime::registerNativeMethods(env,
            gClassPathName, gMethods, NELEM(gMethods));
}

