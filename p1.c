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

#define SAMPLE_FORMAT   ma_format_f32
#define CHANNEL_COUNT   1
#define SAMPLE_RATE     48000

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

mqd_t mqd_1;
mqd_t mqd_log;
ma_event g_stopEvent;

struct p1_msg new_msg;
struct log_msg log_m;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }


    /* printf("Total frames: %d\n", frameCount); */
    uint frames = ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);

    if(mq_send(mqd_1, (const char *)pOutput, sizeof(float) * frames, 1) == -1)
        handle_error("mq_send");

    if(mq_send(mqd_log, (const char *)&log_m, sizeof(struct log_msg), 1) == -1)
        handle_error("mq_send");

    log_m.msg_id++;

    (void)pInput;

    if (frames < frameCount) {
        ma_event_signal(&g_stopEvent);        
    }
}

int main(int argc, char **argv)
{
    mqd_log = mq_open_log_wrapper("/LOG_MSG_QUEUE_1");
    log_m.msg_id = 0;
    log_m.type = 1;
    log_m.sender = 1;

    mqd_1 = mq_open("/MSG_QUEUE_1", O_WRONLY, S_IRUSR | S_IWUSR, NULL);
    if(mqd_1 == (mqd_t) -1)
        handle_error("mq_open");


    printf("%s", argv[1]);
    ma_result result;
    ma_decoder_config decoderConfig;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    if (argc < 2) {
        printf("No input file.\n");
        return -1;
    }

    decoderConfig = ma_decoder_config_init(SAMPLE_FORMAT, CHANNEL_COUNT, SAMPLE_RATE);
    result = ma_decoder_init_file(argv[1], &decoderConfig, &decoder);
    if (result != MA_SUCCESS) {
        return -2;
    }

    printf("\n\nFormat: %d\n", decoder.outputFormat);

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    ma_event_init(&g_stopEvent);

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

    ma_event_wait(&g_stopEvent);

    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

    mq_close(mqd_log);
    mq_close(mqd_1);

    printf("p1\n");
}
