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

#define LOG_TAG "micphone-jni"
#include <utils/Log.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <hardware/micphone.h>

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"

using namespace android;

static mic_hw_device_t* gMicModule = NULL;
static bool gStarted = false;

static jint
com_karaoke_micphone_init(JNIEnv* env,jobject thiz)
{
    ALOGD("com_karaoke_micphone_init");
    hw_module_t const* module;
    if (hw_get_module(MICPHONE_HARDWARE_MODULE_ID, &module) != 0) {
        ALOGE("%s module not found", MICPHONE_HARDWARE_MODULE_ID);
        return -1;
    }
    int err = mic_hw_device_open(module, &gMicModule);
    if(err){
        ALOGE("%s device failed to initialize (%s)\n",
                MICPHONE_HARDWARE_MODULE_ID, strerror(-err));
        return -1;
    }
    gStarted = false;
    return 0;
}

static jint 
com_karaoke_micphone_start(JNIEnv* env,jobject thiz)
{
    ALOGD("com_karaoke_micphone_start()");
    if(gStarted){
        ALOGE("already started");
        return 0;
    }

    if(!gMicModule){
        ALOGE("not initialized");
        return -1;
    }
    if(gMicModule->start(gMicModule) >= 0){
        gStarted = true;
        return 0;
    }
    return -1;
}

static jint
com_karaoke_micphone_stop(JNIEnv* env,jobject thiz)
{
    ALOGD("com_karaoke_micphone_stop()");
    if(!gStarted){
        ALOGE("already paused");
        return 0;
    }
    if(!gMicModule){
        ALOGE("not initialized");
        return -1;
    }
    if(gMicModule->stop(gMicModule) >= 0){
        gStarted = false;
        return 0;
    }
    return -1;
}


static jint
com_karaoke_micphone_pause(JNIEnv* env,jobject thiz)
{
    //same with stop
    ALOGD("com_karaoke_micphone_pause()");
    return com_karaoke_micphone_stop(env,thiz);

}

static jint
com_karaoke_micphone_resume(JNIEnv* env,jobject thiz)
{
    //same with start
    ALOGD("com_karaoke_micphone_resume()");
    return com_karaoke_micphone_start(env,thiz);
}


static jint
com_karaoke_micphone_release(JNIEnv* env,jobject thiz)
{
    ALOGD("com_karaoke_micphone_release()");
    if(gMicModule){
        //before we release,we stop the micphone
        if(gStarted)
            gMicModule->stop(gMicModule);
        mic_hw_device_close(gMicModule);
        gMicModule = NULL;
        gStarted = false;
    }
    //always sucessfull
    return 0;
}

static jint
com_karaoke_micphone_setVolume(JNIEnv* env,jobject thiz,jint volume)
{
    ALOGD("com_karaoke_micphone_setVolume(%d)\n", volume);
    if(!gMicModule){
        ALOGE("not initialized");
        return -1;
    }
    if(volume < 0)
        volume = 0;
    else if(volume > 100)
        volume = 100;

    return gMicModule->set_volume(gMicModule, volume);
}

static jint
com_karaoke_micphone_getVolume(JNIEnv* env,jobject thiz)
{
    ALOGD("com_karaoke_micphone_getVolume()");
    if(!gMicModule){
        ALOGE("not initialized");
        return -1;
    }
    return gMicModule->get_volume(gMicModule);
}


static JNINativeMethod gMethods[] = {
	{"native_init",   "()I", (void *)com_karaoke_micphone_init},
	{"native_start",   "()I", (void *)com_karaoke_micphone_start},
	{"native_pause", "()I", (void *)com_karaoke_micphone_pause},
	{"native_resume",  "()I", (void *)com_karaoke_micphone_resume},
	{"native_stop",  "()I", (void *)com_karaoke_micphone_stop},
	{"native_release", "()I", (void *)com_karaoke_micphone_release},
	{"native_setVolume", "(I)I", (void *)com_karaoke_micphone_setVolume},
	{"native_getVolume", "()I", (void *)com_karaoke_micphone_getVolume},
};


static const char *gClassPathName = "com/karaokeimpl/kMicphone";

int32_t register_com_karaoke_micphone(JNIEnv *env){
    return AndroidRuntime::registerNativeMethods(env,
            gClassPathName, gMethods, NELEM(gMethods));
}

