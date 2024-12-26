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
#include <unistd.h>
#include <pthread.h>

#include "pcm.h"
#define MAX_AUDIO_FRAME_SIZE 40960

uint8_t *buf1 = NULL;
uint8_t *buf2 = NULL;
volatile int buf1_en = 0, buf2_en = 0;
volatile int buf1_size =0, buf2_size = 0;
volatile int buf1_pos = 0, buf2_pos = 0;
volatile int buf1_len = 0, buf2_len = 0;
volatile int out_channels = 0, stop = 0, convert_over = 0;
int dst_bufsize = 0;
static void *pcm_play(void *arg);

int main(int argc, const char *argv[])
{
	const char * input_file = argv[1];
	
	pthread_t pid;
	
	AVFormatContext * fmt_ctx 	= avformat_alloc_context();
    AVCodecContext  * codec_ctx = NULL;
    AVCodec			* codec		= NULL;
    AVPacket 		* pkt 		= av_packet_alloc();
    AVFrame 		* frame 	= av_frame_alloc();
	SwrContext 		* swr_ctx	= NULL;
	AVCodecParameters *codec_param = NULL;
	
	uint8_t * audio_buf = (uint8_t *)av_mallocz(MAX_AUDIO_FRAME_SIZE);  
	buf1 = (uint8_t *)av_mallocz(MAX_AUDIO_FRAME_SIZE);
	buf2 = (uint8_t *)av_mallocz(MAX_AUDIO_FRAME_SIZE);

	enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	uint64_t out_channel_layout = 0;
    int out_sample_rate = 0, val = 0;
    
	int ret = -1, snd_index = -1; 
	int size = 0;
	char device_name[128] = {0};
	int num_read = 0;
	
	if(argc != 2){
		printf("usage : ./ffmpeg_alsa  input_file \r\n");
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
#if 1 	
	if (pthread_create(&pid, NULL, pcm_play, NULL)){ //创建初始化和识别进程
        printf("%s fail!\n", __func__);
		return -1;
	}
#endif
	buf1_en = 0; buf2_en = 1;
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
                    	//int numBytes = av_get_bytes_per_sample(out_sample_fmt);
               			// 修改采样率参数后，需要重新获取采样点的样本个数
                		int dstNbSamples = av_rescale_rnd(frame->nb_samples, out_sample_rate,
                                              codec_ctx->sample_rate, AV_ROUND_ZERO);
                    
                        int len = swr_convert(resampler, &audio_buf,//MAX_AUDIO_FRAME_SIZE*2,
                        				dstNbSamples, (const uint8_t**)frame->data, frame->nb_samples);           				
                        if(len<=0)    continue;

                        dst_bufsize = av_samples_get_buffer_size(0, out_channels, len, out_sample_fmt, 1);
						//printf("dstNbSamples %d  len  %d   dst_bufsize  %d \r\n ", dstNbSamples, len , dst_bufsize);
#if 0
						int i= 0;
						for(i = 0; i < dst_bufsize; i ++){
							printf("%02x ", audio_buf[i]);
							if(i % 32 == 31){
								printf("\r\n");
							}
						}
						printf("\r\n");
#endif				
						
#if 1						
						if(buf1_en == 0 && buf2_en == 1)     //read buf2
						{
							if(buf1_pos + dst_bufsize  > MAX_AUDIO_FRAME_SIZE)  //not enough to write 
							{	
								while(buf2_en == 1);    // wait buf2 read done
								buf1_en = 1;			//start read buf1
								buf1_len = buf1_pos;
								buf1_pos = 0; 
								memcpy(buf2, audio_buf, dst_bufsize); // write to buf2, reset pos
								buf2_pos = dst_bufsize; 
							}
							else{
								memcpy(buf1 + buf1_pos, audio_buf, dst_bufsize);
								buf1_pos += dst_bufsize;
							}
						}
						else if(buf2_en == 0 && buf1_en == 1)  //read buf1
						{
							if(buf2_pos + dst_bufsize  > MAX_AUDIO_FRAME_SIZE)  //not enough to write 
							{	
								while(buf1_en == 1);
								buf2_en = 1; 
								buf2_len = buf2_pos;
								buf2_pos = 0;				
								memcpy(buf1, audio_buf, dst_bufsize); // write to buf1, reset pos
								buf1_pos = dst_bufsize; 
							}
							else{
								memcpy(buf2 + buf2_pos, audio_buf,dst_bufsize);
								buf2_pos += dst_bufsize;
							}
						}
#endif
					}
                }
            }
        }
        av_packet_unref(pkt);
    }
    sleep(1);
	convert_over = 1;

    printf("decode done\r\n");
    
    while(stop == 0);   
    
	av_free(audio_buf);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_close(codec_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_free_context(fmt_ctx);  
    printf("all closed\r\n");
	return 0;
}

static void *pcm_play(void *arg)
{
	struct pcm_config config;
	struct pcm * p_pcm;
	
	printf("pcm play thread \r\n");
	config.channels = out_channels;
    config.rate = 48000;  // 24000,44100 error
    config.period_size = 1024;
    config.period_count = 4;
 	config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;
    
    printf("config.channels %d \t config.rate %d\r\n", config.channels, config.rate);

	p_pcm = pcm_open(0, 1, PCM_OUT, &config);
	if(!p_pcm || !pcm_is_ready(p_pcm)){
		printf("pcm_open error  %s\r\n", pcm_get_error(p_pcm));
		exit(0);
	}
	
	printf("pcm write start\r\n");
	do{
		if(buf2_en == 1){
			pcm_write(p_pcm, buf2, buf2_len);
           	buf2_en = 0;
		}
		else if(buf1_en == 1){
			pcm_write(p_pcm, buf1, buf1_len);
           	buf1_en = 0;
		}
	}
	while(convert_over == 0);
	
	printf("done\r\n");
	
    pcm_close(p_pcm);
    stop = 1;
}


