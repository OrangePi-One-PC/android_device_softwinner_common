
#define LOG_TAG "audio_raw"
// #define LOG_NDEBUG 0

#include<fcntl.h>
#include<sys/stat.h>

#include <cutils/log.h>
#include <cutils/properties.h> // for property_get

#include "audio_raw.h"

int read_node_value(const char *path, RAW_INFO_t *raw_info)
{
  int ret = -1;
  int fd = open(path, O_RDONLY);
  if (fd >= 0)
  {
    read(fd, raw_info, sizeof(RAW_INFO_t));
    close(fd);
    ret = 0;
  }
  return ret;
}

int write_node_value(RAW_INFO_t *raw_info)
{
    int ret = -1;
    int fd = open(NODE_AUDIO_RAW, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR
                  | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    //int fd = open(NODE_AUDIO_RAW, O_RDWR | O_CREAT, 777);
    /*
    RAW_INFO_t raw_info;
    memset(&raw_info, '0',sizeof(RAW_INFO_t));
    raw_info.raw_mode = RAW_PCM;
    raw_info.raw_enable = 1;
    //raw_info.sample_rate = 192000;
    raw_info.sample_rate = 44100;
    */

    //chmod for XBMC, or it will need to root device.
    chmod(NODE_AUDIO_RAW, S_IRUSR | S_IWUSR
                  | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd >=0 && raw_info) 
    {
        write(fd, raw_info, sizeof(RAW_INFO_t));
        close(fd);
        ALOGD("write node success!");
        ret = 0;
    } else {
        ALOGD("write node failed!");
    }
    return ret;
}

void detectRaw(void * audio_buf, RAW_INFO_t * raw_info)
{
    //init value if does not exists NODE_AUDIO_RAW
    raw_info->raw_enable = 0;
	raw_info->raw_mode = RAW_PCM;
	raw_info->sample_rate = 44100;
	if (read_node_value(NODE_AUDIO_RAW, raw_info) == 0)
	{
		//raw_info->raw_mode = (RAW_MODE_t)atoi(str_val);
		//raw_info->sample_rate = 192000;
        //raw_info->sample_rate = 48000;
        //raw_info->sample_rate = 44100;
		//ALOGD("read_node_value %s ok, enable: %d, raw_mode=%d, sampleRate=%d", NODE_AUDIO_RAW, raw_info->raw_enable
                      //, raw_info->raw_mode, raw_info->sample_rate);

	}
	else
	{
		//ALOGD("read_node_value %s failed: %s", NODE_AUDIO_RAW, strerror(errno));
        write_node_value(raw_info);
	}
}

