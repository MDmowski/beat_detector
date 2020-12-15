#define MINIAUDIO_IMPLEMENTATION
#include "./miniaudio.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <signal.h>

#include <pthread.h> 
#include <mqueue.h> 
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

#include "messages.h"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

mqd_t mqd_1;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    struct log_msg log;
    struct p1_msg new_msg;

    printf("Total frames: %d\n", frameCount);
    uint frames = ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);

    if(mq_send(mqd_1, (const char *)&new_msg, sizeof(struct p1_msg), 1) == -1)
        handle_error("mq_send");

    (void)pInput;
}

int main(int argc, char **argv)
{
    mqd_t mqd_log = mq_open_log_wrapper("/LOG_MSG_QUEUE_1");

    mqd_1 = mq_open("/MSG_QUEUE_1", O_WRONLY, S_IRUSR | S_IWUSR, NULL);
    if(mqd_1 == (mqd_t) -1)
        handle_error("mq_open");

    printf("%s", argv[1]);
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    if (argc < 2) {
        printf("No input file.\n");
        return -1;
    }

    result = ma_decoder_init_file(argv[1], NULL, &decoder);
    if (result != MA_SUCCESS) {
        return -2;
    }

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&decoder);
        return -3;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -4;
    }

    printf("Press Enter to quit...");
    getchar();

    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

    mq_close(mqd_log);
    mq_close(mqd_1);

    printf("p1\n");
}
