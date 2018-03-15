/*
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

//#define LOG_NDEBUG 0
#define LOG_TAG "karaoke-JNI"
#include <utils/Log.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>


#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"


// ----------------------------------------------------------------------------

using namespace android;

// ----------------------------------------------------------------------------

extern int register_com_karaoke_micphone(JNIEnv *env);
extern int register_com_karaoke_rtsoundeffects(JNIEnv *env);
//extern int register_com_karaoke_mediaplayerex(JNIEnv *env);

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (register_com_karaoke_micphone(env) < 0) {
        ALOGE("ERROR: karaoke_micphone native registration failed");
        goto bail;
    }
    if (register_com_karaoke_rtsoundeffects(env) < 0) {
        ALOGE("ERROR: karaoke_rtsoundeffect native registration failed");
        goto bail;
    }
/*     if (register_com_karaoke_mediaplayerex(env) < 0){
        ALOGE("ERROR: karaoke_mediaplayerex native registration failed");
        goto bail;
    } */
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

bail:
    return result;
}

// KTHXBYE
