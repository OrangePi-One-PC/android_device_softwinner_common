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

using namespace android;

enum MEDIA_CHANNEL{
    LEFT = 0,
    RIGHT = 1,
    CENTER = 2
};

static jint
com_karaoke_switchChannel(JNIEnv* env,jobject thiz,jint channel)
{
    ALOGD("com_karaoke_switchChannel(%d)\n", channel);
    //todo : do something
    //if success return new set channel
    //else return old channel
    return channel;
}

static JNINativeMethod gMethods[] = {
	{"nativeSwitchChannel", "(I)I", (void *)com_karaoke_switchChannel},
};


static const char *gClassPathName = "android/media/MediaPlayer";

int32_t register_com_karaoke_mediaplayerex(JNIEnv *env){
    return AndroidRuntime::registerNativeMethods(env,
            gClassPathName, gMethods, NELEM(gMethods));
}

