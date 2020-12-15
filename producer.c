/*
Demonstrates how to load a sound file and play it back using the low-level API.

The low-level API uses a callback to deliver audio between the application and miniaudio for playback or recording. When
in playback mode, as in this example, the application sends raw audio data to miniaudio which is then played back through
the default playback device as defined by the operating system.

This example uses the `ma_decoder` API to load a sound and play it back. The decoder is entirely decoupled from the
device and can be used independently of it. This example only plays back a single sound file, but it's possible to play
back multiple files by simple loading multiple decoders and mixing them (do not create multiple devices to do this). See
the simple_mixing example for how best to do this.
*/
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

ma_event g_stopEvent;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }
    printf("Total frames: %d\n", frameCount);
    mqd_t queue = mq_open("/TEST", O_RDWR, S_IRUSR | S_IWUSR, NULL);
    if(queue == -1)
	    printf("error opening: %s\n", strerror(errno));
    uint frames = ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);
    printf("Sending %d frames\n",frames);
    if(mq_send(queue, pOutput, sizeof(float)*frames, 1) == -1)
	 printf("error sending: %s\n", strerror(errno));
    mq_close(queue);
    (void)pInput;
    if (frames < frameCount) {
    	ma_event_signal(&g_stopEvent);        
    }
}

int main(int argc, char** argv)
{
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

    return 0;
}
