#include <libavutil/avutil.h>
#include <libavutil/attributes.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
#include <libswresample/swresample.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
#include <libswresample/swresample.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "pcm.h"
#define MAX_AUDIO_FRAME_SIZE 192000

int sample_is_playable(unsigned int card, unsigned int device, unsigned int channels,unsigned int rate, 
								unsigned int bits, unsigned int period_size,unsigned int period_count);

int main(int argc, const char *argv[])
{
	const char * input_file = argv[1];
	const char * output_file = argv[2];
	
	AVFormatContext * fmt_ctx 	= avformat_alloc_context();
    AVCodecContext  * codec_ctx = NULL;
    AVCodec			* codec		= NULL;
    AVPacket 		* pkt 		= av_packet_alloc();
    AVFrame 		* frame 	= av_frame_alloc();
	SwrContext 		* swr_ctx	= NULL;
	AVCodecParameters *codec_param = NULL;

	struct pcm_config config;
	struct pcm * p_pcm;
	
	uint8_t 		* audio_buf = (uint8_t *)av_mallocz(MAX_AUDIO_FRAME_SIZE * 2);  
	enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	uint64_t out_channel_layout = 0;
    int out_sample_rate = 0, val = 0;
    int out_channels = 0;
	int ret = -1, snd_index = -1; 
	int size = 0;
	char device_name[128] = {0};
	int num_read = 0;
#if 1	
	if(argc != 3){
		printf("usage : ./ffmpeg_alsa  input_file output_file \r\n");
		return -1;
	}
	
	FILE * file = fopen(output_file,"wb+");
	if(file == NULL){
		printf("open output file error\r\n");
		return -1;
	}
	
	ret = avformat_open_input(&fmt_ctx, input_file, NULL, NULL);  
	if(ret < 0){
        printf("Cannot open input file.\t %d \t %s\n", ret, av_err2str(ret));
        return -1;
    }
    
    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if(ret < 0){
        printf("Cannot find any stream in file.\t %d \t %s\n", ret, av_err2str(ret));
        return -1;
    }

    av_dump_format(fmt_ctx, 0, input_file, 0);
	
    snd_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, snd_index, -1, NULL, 0);
	printf(" snd_index = %d\r\n", snd_index);
	
	if(snd_index != -1)
	{
		codec_param = fmt_ctx->streams[snd_index]->codecpar;
        codec = avcodec_find_decoder(codec_param->codec_id);
        if(codec == NULL){
        	printf("dont find decoder\r\n");
            return -1;
        }
        printf("avcodec_find_decoder done\r\n");
        codec_ctx = avcodec_alloc_context3(codec);
        if(avcodec_parameters_to_context(codec_ctx, codec_param) < 0){
            printf("Cannot alloc codec context.\r\n");
            return -1;
        }
        printf("avcodec_parameters_to_context done\r\n");
        codec_ctx->pkt_timebase = fmt_ctx->streams[snd_index]->time_base;  
        
        if(avcodec_open2(codec_ctx, codec, NULL) < 0){
        	printf("codec open fail\r\n"); 
            return -1;
        }
        printf("avcodec_open2 done\r\n");

	}else{
		printf("snd_index = %d\r\n", snd_index);
		return -1;
	}
	
	//out_channel_layout = 2;//codec_ctx->channel_layout;
	out_channel_layout = av_get_default_channel_layout(codec_ctx->channels);
	out_sample_rate = 48000;//codec_ctx->sample_rate;
	out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	printf("out channels = %d\r\n", out_channels);
	
	SwrContext *resampler = swr_alloc();
    swr_ctx = swr_alloc_set_opts(resampler, out_channel_layout, out_sample_fmt, out_sample_rate,
                                codec_ctx->channel_layout, codec_ctx->sample_fmt, codec_ctx->sample_rate, 0, NULL);
    swr_init(resampler);

	printf("output channel_layout = %ld \t sample_rate = %d \r\n",  out_channel_layout, out_sample_rate);
	printf("input channel_layout = %ld \t sample_rate = %d \r\n",  codec_ctx->channel_layout, codec_ctx->sample_rate);
	
	snprintf(device_name, sizeof(device_name), "/dev/snd/pcmC%uD%u%c", 0, 1, 'p');
	
	printf("device name = %s \r\n", device_name);
	
	while(av_read_frame(fmt_ctx, pkt) >= 0 )
    {
        if(pkt->stream_index == snd_index)
        {
            if(avcodec_send_packet(codec_ctx, pkt) >= 0)
            {
                while(avcodec_receive_frame(codec_ctx, frame) >= 0)
                {
                    if(av_sample_fmt_is_planar(codec_ctx->sample_fmt))
                    {
                    
                    	int numBytes = av_get_bytes_per_sample(out_sample_fmt);
               			// 修改采样率参数后，需要重新获取采样点的样本个数
                		int dstNbSamples = av_rescale_rnd(frame->nb_samples, out_sample_rate,
                                              codec_ctx->sample_rate, AV_ROUND_ZERO);
                    
                        int len = swr_convert(resampler, &audio_buf,//MAX_AUDIO_FRAME_SIZE*2,
                        				dstNbSamples, (const uint8_t**)frame->data, frame->nb_samples);
                        				
                        if(len<=0)    continue;

                        int dst_bufsize = av_samples_get_buffer_size(0, out_channels, len, out_sample_fmt, 1);
						fwrite(audio_buf, 1, dst_bufsize, file);
					}
                }
            }
        }
        av_packet_unref(pkt);
    }
    fclose(file);    
#endif	

#if 1
	printf("decode done\r\n");
	
	file = fopen(output_file,"rb");
	if(file == NULL){
		printf("open output file error\r\n");
		return -1;
	}
	config.channels = out_channels;
    config.rate = out_sample_rate;  // 24000,44100 error
    config.period_size = 1024;
    config.period_count = 4;
 	config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;
    printf("config.channels %d \t config.rate %d\r\n", config.channels, config.rate);
//printf("12333\r\n");
#if 0	
	if (!sample_is_playable(0, 1, config.channels, config.rate, 16, config.period_size, config.period_count)){
		printf("sample is unplayable \r\n");
        return -1;
    }
#endif
	//printf("1222\r\n");
	p_pcm = pcm_open(0, 1, PCM_OUT, &config);
	if(!p_pcm || !pcm_is_ready(p_pcm)){
		printf("pcm_open error  %s\r\n", pcm_get_error(p_pcm));
		return -1;
	}
	//printf("1111\r\n");
	size = pcm_frames_to_bytes(p_pcm, pcm_get_buffer_size(p_pcm));
	printf("pcm_frames_to_bytes size = %d\r\n", size);
    char *buffer = (char *)malloc(size);
    if (!buffer) {
        printf("Unable to allocate %d bytes\n", size);
        free(buffer);
        pcm_close(p_pcm);
        return -1;
    }
	
    printf("Playing sample: %u ch, %u hz\n", config.channels, config.rate);
	
    do {
        num_read = fread(buffer, 1, size, file);
        //printf("fread num_read = %d\r\n", num_read);
        if (num_read > 0) {
            if (pcm_write(p_pcm, buffer, num_read)) {
                printf("Error playing sample\n");
                break;
            }
        }
    } while (num_read > 0);
	
	free(buffer);
    pcm_close(p_pcm);
    fclose(file);
#endif
	av_free(audio_buf);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_close(codec_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_free_context(fmt_ctx);  
	return 0;
}

#if 0
int check_param(struct pcm_params *params, unsigned int param, unsigned int value,
                 char *param_name, char *param_unit)
{
    unsigned int min;
    unsigned int max;
    int is_within_bounds = 1;

    min = pcm_params_get_min(params, param);
    if (value < min) {
        fprintf(stderr, "%s is %u%s, device only supports >= %u%s\n", param_name, value,
                param_unit, min, param_unit);
        is_within_bounds = 0;
    }

    max = pcm_params_get_max(params, param);
    if (value > max) {
        fprintf(stderr, "%s is %u%s, device only supports <= %u%s\n", param_name, value,
                param_unit, max, param_unit);
        is_within_bounds = 0;
    }

    return is_within_bounds;
}

int sample_is_playable(unsigned int card, unsigned int device, unsigned int channels,
                        unsigned int rate, unsigned int bits, unsigned int period_size,
                        unsigned int period_count)
{
    struct pcm_params *params;
    int can_play;

    params = pcm_params_get(card, device, PCM_OUT);
    if (params == NULL) {
        fprintf(stderr, "Unable to open PCM device %u.\n", device);
        return 0;
    }

    can_play = check_param(params, PCM_PARAM_RATE, rate, "Sample rate", "Hz");
    can_play &= check_param(params, PCM_PARAM_CHANNELS, channels, "Sample", " channels");
    can_play &= check_param(params, PCM_PARAM_SAMPLE_BITS, bits, "Bitrate", " bits");
    can_play &= check_param(params, PCM_PARAM_PERIOD_SIZE, period_size, "Period size", "Hz");
    can_play &= check_param(params, PCM_PARAM_PERIODS, period_count, "Period count", "Hz");

    pcm_params_free(params);

    return can_play;
}
#endif

