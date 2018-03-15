#define LOG_TAG "micphone-hal"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <hardware/hardware.h>
#include <hardware/micphone.h>

#include <autil/audio_utils.h>

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#ifndef bool
#define bool int32_t
#endif

#define SUNXI_MIC

/*
 * define mixer_ctls for control codec alc3224
 */

#define MIXER_MIC_HDMI_ENABLE   "RIGHT ADC input Mixer MIC2 boost Switch"
#define MIXER_MIC_CVBS_ENABLE   "AIF1 AD0R Mixer ADCR Switch"
#define MIXER_MIC_VOLUME        "ADC volume"

#define MIXER_MIC_HDMI_ENABLE_Duplicate   "AIF1 ADCR Switch Duplicate"
#define MIXER_MIC_CVBS_ENABLE_Duplicate   "DACR ADCR Switch Duplicate"

#define MIXER_MIC_SINGLE_ENABLE   "AIF1OUT0L Mux"

struct mixer_ctls {
    struct mixer_ctl *mic_hdmi_enable;
    struct mixer_ctl *mic_cvbs_enable;
    struct mixer_ctl *mic_volume;
	struct mixer_ctl *out_hdmi_enable;
	struct mixer_ctl *out_spdif_enable;
	struct mixer_ctl *micl_enable;
	
	struct mixer_ctl *mic_hdmi_enable_duplicate;
	struct mixer_ctl *mic_cvbs_enable_duplicate;
	struct mixer_ctl *mic_single_enable;
};

struct mic_context_t{
    mic_hw_device_t device; 
    /* our private states goes below here */
    struct mixer *mixer;
    struct mixer_ctls mixer_ctls;
    bool started;
    pthread_mutex_t lock;
};

static int32_t mic_dump(const struct mic_hw_device* dev)
{
    ALOGD("%s()", __func__);
    struct mic_context_t* ctx = (struct mic_context_t*)dev;
    pthread_mutex_lock(&ctx->lock);
    //TODO
    pthread_mutex_unlock(&ctx->lock);
    return 0;
}

static int32_t mic_start(const struct mic_hw_device* dev)
{
    ALOGD("%s()", __func__);
    struct mic_context_t* ctx = (struct mic_context_t*)dev;
    pthread_mutex_lock(&ctx->lock);
	ALOGD("Micphone,%s()", __func__);

	mixer_ctl_set_value(ctx->mixer_ctls.mic_hdmi_enable, 0, 1);
	
	mixer_ctl_set_value(ctx->mixer_ctls.mic_cvbs_enable, 0, 1);
	usleep(500000);
	mixer_ctl_set_value(ctx->mixer_ctls.mic_hdmi_enable_duplicate, 0, 1);
	mixer_ctl_set_value(ctx->mixer_ctls.mic_cvbs_enable_duplicate, 0, 1);
	mixer_ctl_set_value(ctx->mixer_ctls.mic_single_enable, 0, 1);
    //mixer_ctl_set_value(ctx->mixer_ctls.mic_cvbs_enable, 1, 1);
	//usleep(500000);
    pthread_mutex_unlock(&ctx->lock);
    return 0;
}

static int32_t mic_stop(const struct mic_hw_device* dev)
{
    ALOGD("%s()", __func__);
    struct mic_context_t* ctx = (struct mic_context_t*)dev;
    pthread_mutex_lock(&ctx->lock);
	ALOGD("Micphone,%s()", __func__);
	mixer_ctl_set_value(ctx->mixer_ctls.mic_cvbs_enable_duplicate, 0, 0);
	
	mixer_ctl_set_value(ctx->mixer_ctls.mic_hdmi_enable_duplicate, 0, 0);	
    mixer_ctl_set_value(ctx->mixer_ctls.mic_cvbs_enable, 0, 0);
    mixer_ctl_set_value(ctx->mixer_ctls.mic_hdmi_enable, 0, 0);
	mixer_ctl_set_value(ctx->mixer_ctls.mic_single_enable, 0, 0);
    //mixer_ctl_set_value(ctx->mixer_ctls.mic_cvbs_enable, 1, 0);

    pthread_mutex_unlock(&ctx->lock);
    return 0;
}

static int32_t mic_set_volume(const struct mic_hw_device* dev, int32_t volume)
{
    ALOGD("%s(), volume : %d\n", __func__, volume);
    struct mic_context_t* ctx = (struct mic_context_t*)dev;
    int val;
    
    pthread_mutex_lock(&ctx->lock);
/*
	if (volume > 98) {
    	val = 31;
    }
    else if (volume >= 49){ // 23~30
    	val = volume / 7 + 16;
  	} else {
  		val = volume / 7 * 3;
  	}
    if (val < 0)
      val = 0;

    val = volume / 10 * 3;
*/
    if(volume > 99){
		val = 180;
    }
	if(volume <= 0){
		val = 0;
	}
	val = volume/2 + 130;
    ALOGD("%s(), val : %d\n", __func__, val);
    mixer_ctl_set_value(ctx->mixer_ctls.mic_volume, 0, val);
    mixer_ctl_set_value(ctx->mixer_ctls.mic_volume, 1, val);

    pthread_mutex_unlock(&ctx->lock);
    return 0;
}

static int32_t mic_get_volume(const struct mic_hw_device* dev)
{
    ALOGD("%s()", __func__);
    struct mic_context_t* ctx = (struct mic_context_t*)dev;
    int val;
    pthread_mutex_lock(&ctx->lock);

    //val = mixer_ctl_get_value(ctx->mixer_ctls.mic_volume, 0);
    /*
    if (val > 30) {
    	val = 99;
    } else if (val >= 23) {
    	val = (val - 16) * 7;
    } else {
    	if (val > 18)
    		val = 18;
    	val = val / 3 * 7;
    }
    */

    //val = val / 3 * 10;

    ALOGD("%s(), val : %d\n", __func__, val);
    pthread_mutex_unlock(&ctx->lock);
    return val;
}

static int32_t mic_close(hw_device_t *device)
{
    ALOGD("%s()", __func__);
    struct mic_context_t* ctx= (struct mic_context_t*)device;
    ALOGD("Micphone,%s()", __func__);
    mixer_ctl_set_value(ctx->mixer_ctls.mic_hdmi_enable, 0, 0);
    mixer_ctl_set_value(ctx->mixer_ctls.mic_cvbs_enable, 0, 0);
    mixer_ctl_set_value(ctx->mixer_ctls.mic_cvbs_enable, 0, 0);
    mixer_ctl_set_value(ctx->mixer_ctls.mic_hdmi_enable, 0, 0);
	mixer_ctl_set_value(ctx->mixer_ctls.mic_single_enable, 0, 0);
    //mixer_ctl_set_value(ctx->mixer_ctls.mic_cvbs_enable, 1, 0);

    if(ctx){
        pthread_mutex_destroy(&ctx->lock);
        free(ctx);
    }
    return 0;
}

static int32_t mic_dev_open(const hw_module_t* module, const char* name,
            hw_device_t** device)
{
    ALOGD("%s()", __func__);
    struct mic_context_t *tCtx;
    int32_t ret = 0;
    int32_t card;
    if(strcmp(name, MIC_HARDWARE_INTERFACE) != 0){
        return -EINVAL; 
    }

    tCtx = calloc(1, sizeof(struct mic_context_t));
    if (!tCtx)
        return -ENOMEM;

    tCtx->device.common.tag = HARDWARE_DEVICE_TAG;
    tCtx->device.common.version = MICPHONE_DEVICE_API_VERSION_1_0;
    tCtx->device.common.module = (struct hw_module_t *) module;
    tCtx->device.common.close = mic_close;

    tCtx->device.start = mic_start;
    tCtx->device.stop = mic_stop;
    tCtx->device.set_volume = mic_set_volume;
    tCtx->device.get_volume = mic_get_volume;
    tCtx->device.dump = mic_dump;

    *device = &tCtx->device.common;

    //get_audio_card_num(INFOTMIC_PCM_ID_DEFAULT_RT, &card);
	card = 0;
    ALOGD("%s() card:%d", __func__, card);
    tCtx->mixer = mixer_open(card);
    if (!tCtx->mixer) {
        ret = -EINVAL;
        ALOGE("Unable to open the mixer, aborting.");
        goto done;
    }
	ALOGD("Micphone,%s()", __func__);

    tCtx->mixer_ctls.mic_hdmi_enable = mixer_get_ctl_by_name(tCtx->mixer, MIXER_MIC_HDMI_ENABLE);
    tCtx->mixer_ctls.mic_cvbs_enable = mixer_get_ctl_by_name(tCtx->mixer, MIXER_MIC_CVBS_ENABLE);
    tCtx->mixer_ctls.mic_volume = mixer_get_ctl_by_name(tCtx->mixer, MIXER_MIC_VOLUME);
    
    tCtx->mixer_ctls.mic_hdmi_enable_duplicate = mixer_get_ctl_by_name(tCtx->mixer, MIXER_MIC_HDMI_ENABLE_Duplicate);
    tCtx->mixer_ctls.mic_cvbs_enable_duplicate = mixer_get_ctl_by_name(tCtx->mixer, MIXER_MIC_CVBS_ENABLE_Duplicate);
    tCtx->mixer_ctls.mic_single_enable = mixer_get_ctl_by_name(tCtx->mixer, MIXER_MIC_SINGLE_ENABLE);


	ALOGE("Microphone,tCtx->mixer_ctls.mic_hdmi_enable = %d", tCtx->mixer_ctls.mic_hdmi_enable);

    tCtx->started = false;
    if (pthread_mutex_init(&tCtx->lock, NULL)) {
        ALOGE("failed to create mutex (%d): %m", errno);
        ret = -errno;
        goto done;
    }
	mixer_ctl_set_value(tCtx->mixer_ctls.mic_hdmi_enable, 0, 0);
done:
    if(ret){
        pthread_mutex_destroy(&tCtx->lock);
        free(tCtx);
        *device = NULL;
    }
    return ret;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = mic_dev_open,
};

struct mic_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = MICPHONE_MODULE_API_VERSION_1_0,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = MICPHONE_HARDWARE_MODULE_ID,
        .name = "Micphone interface HW HAL",
        .author = "The GPL",
        .methods = &hal_module_methods,
    },
};
