#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mad.h"

//#pragma comment(lib, "libmad.lib")

struct mad_stream stream;
struct mad_frame frame;
struct mad_synth synth;

typedef struct FrameHeader {
    unsigned int sync:11 ;
    unsigned int version:2 ;
    unsigned int layer:2 ;
    unsigned int error_protection:1 ;
    unsigned int bitrate_index:4 ;
    unsigned int sampling_frequency:2 ;
    unsigned int padding:1 ;
    unsigned int private:1 ;
    unsigned int mode:2 ;
    unsigned int mode_extension:2 ;
    unsigned int copyright:1 ;
    unsigned int original:1 ;
    unsigned int emphasis:2 ;
}HEADER,*LPHEADER;

HEADER  g_mp3_frame_header = {0};
#if 0
static void print_mp3_frame_header(HEADER *header)
{
    printf("header->sync: 0x%x \n", header->sync);
    printf("header->version : %d \n", header->version);
    printf("header->layer : %d\n", header->layer);
    printf("header->error_protection : %d\n", header->error_protection);
    printf("header->bitrate_index : %d\n", header->bitrate_index);
    printf("header->sampling_frequency : %d\n", header->sampling_frequency);
    printf("header->padding : %d\n", header->padding);
    printf("header->private : %d\n", header->private);
    printf("header->mode : %d\n", header->mode);
    printf("header->mode_extension : %d\n", header->mode_extension);
    printf("header->copyright : %d\n", header->copyright);
    printf("header->original : %d\n", header->original);
    printf("header->emphasis : %d\n", header->emphasis);
    
}

static void parse_mp3_frame_header(void)
{
    g_mp3_frame_header.sync =  mad_bit_read(&(stream.ptr), 11);
    g_mp3_frame_header.version =  mad_bit_read(&(stream.ptr), 2);
    g_mp3_frame_header.layer =  mad_bit_read(&(stream.ptr), 2);
    g_mp3_frame_header.error_protection =  mad_bit_read(&(stream.ptr), 1);
    g_mp3_frame_header.bitrate_index =  mad_bit_read(&(stream.ptr), 4);
    g_mp3_frame_header.sampling_frequency =  mad_bit_read(&(stream.ptr), 2);
    g_mp3_frame_header.padding =  mad_bit_read(&(stream.ptr), 1);
    g_mp3_frame_header.private =  mad_bit_read(&(stream.ptr), 1);
    g_mp3_frame_header.mode =  mad_bit_read(&(stream.ptr), 2);
    g_mp3_frame_header.mode_extension =  mad_bit_read(&(stream.ptr), 2);
    g_mp3_frame_header.copyright =  mad_bit_read(&(stream.ptr), 1);
    g_mp3_frame_header.original =  mad_bit_read(&(stream.ptr), 1);
    g_mp3_frame_header.emphasis =  mad_bit_read(&(stream.ptr), 2);
}
#endif
char g_fdata_buffer[8196] = {0};

/*
转换24bit pcm为24bit pcm
 */
static signed int scale(mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 24));
 
  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;
 
  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 24);
}

static void mp3_decode_to_pcm(void)
{
	int i = 0; /*, j = 0;*/
	int read_len = 0, buffer_left_bytes = 0;
	//int label_header_len = 0;
	//unsigned char label_header[10] = {0};
	unsigned char *buf;
    signed int sample;

#if 1
	FILE *finput = fopen("00009.mp3", "rb");
	FILE *foutput = fopen("output16.pcm", "wb");

	//unsigned char buffer[2048];
	unsigned char buffer[576];
	//signed short output[2][1152];
 
	mad_stream_init(&stream);
	mad_frame_init(&frame);
	mad_synth_init(&synth);

#if 0	
	read_len = fread(label_header, 1, sizeof(label_header), finput);
	if (label_header[0] == 'I' && label_header[1] == 'D' && label_header[2] == '3')
	{
		label_header_len = (label_header[6]&0x7F)*0x200000 + (label_header[7]&0x7F)*0x4000 + (label_header[8]&0x7F)*0x80 + (label_header[9]&0x7F);
	}
	
	fseek(finput, label_header_len, SEEK_CUR);
	//printf("label_header_len: %d \n", label_header_len);
#endif

    buf = (unsigned char*)malloc(1152 * 2 * 3);
    
#if 0
	if ((read_len = fread(g_fdata_buffer, 1, sizeof(g_fdata_buffer), finput)) > 0)
	{
		mad_stream_buffer(&stream, g_fdata_buffer, sizeof(g_fdata_buffer));
        
		while (mad_frame_decode(&frame, &stream) == 0) 
		{
			mad_synth_frame(&synth, &frame);
			printf("synth.pcm.samplerate : %d, synth.pcm.channels : %d, synth.pcm.length : %d \n", synth.pcm.samplerate, synth.pcm.channels, synth.pcm.length);
            
            memset(buf, 0x00, 1152 * 2 * 3);
            for (i = 0; i < synth.pcm.length; i++)
            {
                sample = scale(synth.pcm.samples[0][i]);
                buf[i * 2 * 3 + 0] = sample & 0xff;
                buf[i * 2 * 3 + 1] = (sample >> 8) & 0xff;
                buf[i * 2 * 3 + 2] = (sample >> 16) & 0xff;

                sample = scale(synth.pcm.samples[1][i]);
                buf[i * 2 * 3 + 3] = sample & 0xff;
                buf[i * 2 * 3 + 4] = (sample >> 8) & 0xff;
                buf[i * 2 * 3 + 5] = (sample >> 16) & 0xff;
            }
            fwrite(buf, 1, synth.pcm.length * 2 * 3, foutput);
            //fwrite(synth.pcm.samples[0], 4, synth.pcm.length, foutput);
			//fwrite(synth.pcm.samples[1], 4, synth.pcm.length, foutput);
		}
				
	}
#else
//again:
	if ((read_len = fread(buffer + buffer_left_bytes, 1, sizeof(buffer) - buffer_left_bytes, finput)) > 0) 
	{
		//printf("****** read_len: %d \n", read_len);
		
		mad_stream_buffer(&stream, buffer, sizeof(buffer));

    #if 1
		while (mad_frame_decode(&frame, &stream) == 0) 
		{
			mad_synth_frame(&synth, &frame);
			//printf("synth.pcm.samplerate : %d, synth.pcm.channels : %d, synth.pcm.length : %d \n", synth.pcm.samplerate, synth.pcm.channels, synth.pcm.length);
            for (i = 0; i < synth.pcm.length; i++)
            {
                sample = scale(synth.pcm.samples[0][i]);
                buf[i * 2 * 3 + 0] = sample & 0xff;
                buf[i * 2 * 3 + 1] = (sample >> 8) & 0xff;
                buf[i * 2 * 3 + 2] = (sample >> 16) & 0xff;

                sample = scale(synth.pcm.samples[1][i]);
                buf[i * 2 * 3 + 3] = sample & 0xff;
                buf[i * 2 * 3 + 4] = (sample >> 8) & 0xff;
                buf[i * 2 * 3 + 5] = (sample >> 16) & 0xff;
            }
            fwrite(buf, 1, synth.pcm.length * 2 * 3, foutput);
            //fwrite(synth.pcm.samples[0], 4, synth.pcm.length, foutput);
			//fwrite(synth.pcm.samples[1], 4, synth.pcm.length, foutput);
		}
    #endif
        //fwrite(buffer, 1, sizeof(buffer), foutput);

		buffer_left_bytes = stream.bufend - stream.next_frame;
		//printf("buffer_left_bytes: %d \n", buffer_left_bytes);
		if (buffer_left_bytes > 0)
			memmove(buffer, stream.next_frame, buffer_left_bytes);

        //j ++ ;
        //if (j <= 2)
        //    goto again;
        
		//mad_stream_finish(&stream);
	}
    
#endif 
	mad_synth_finish(&synth);
	mad_frame_finish(&frame);
	mad_stream_finish(&stream);

    if (buf) free(buf);

	fclose(finput);
	fclose(foutput);
	
#endif

}

int main()
{

	mp3_decode_to_pcm();

#if 0
    int i = 0;
	int read_len = 0, buffer_left_bytes = 0;
    int label_header_len = 0;
    unsigned char label_header[10] = {0};
    

    FILE *finput = fopen("sinfile3.mp3", "rb");
    FILE *foutput = fopen("output6.pcm", "wb");

    unsigned char buffer[321];
    //signed short output[2][1152];
 
    mad_stream_init(&stream);
    mad_frame_init(&frame);
    mad_synth_init(&synth);

    
    read_len = fread(label_header, 1, sizeof(label_header), finput);
    if (label_header[0] == 'I' && label_header[1] == 'D' && label_header[2] == '3')
    {
        label_header_len = (label_header[6]&0x7F)*0x200000 + (label_header[7]&0x7F)*0x4000 + (label_header[8]&0x7F)*0x80 + (label_header[9]&0x7F);
    }
	//printf("label_header_len: %d \n", label_header_len);
	//label_header_len += 10;
    fseek(finput, label_header_len, SEEK_CUR);
    printf("label_header_len: %d \n", label_header_len);
	
    #if 1
    while ((read_len = fread(buffer + buffer_left_bytes, 1, sizeof(buffer) - buffer_left_bytes, finput)) > 0) 
    {
        //printf("****** read_len: %d \n", read_len);
        
        mad_stream_buffer(&stream, buffer, sizeof(buffer));

        #if 1
        while (mad_frame_decode(&frame, &stream) == 0) 
        {
            mad_synth_frame(&synth, &frame);
            printf("synth.pcm.samplerate : %d, synth.pcm.channels : %d, synth.pcm.length : %d \n", synth.pcm.samplerate, synth.pcm.channels, synth.pcm.length);
            fwrite(synth.pcm.samples[0], 4, synth.pcm.length, foutput);
            fwrite(synth.pcm.samples[1], 4, synth.pcm.length, foutput);
        }
        #endif

        buffer_left_bytes = stream.bufend - stream.next_frame;
        //printf("buffer_left_bytes: %d \n", buffer_left_bytes);
        if (buffer_left_bytes > 0)
            memmove(buffer, stream.next_frame, buffer_left_bytes);
        
        mad_stream_finish(&stream);
        
    }
	#endif 
    mad_synth_finish(&synth);
    mad_frame_finish(&frame);
    mad_stream_finish(&stream);

    fclose(finput);
    fclose(foutput);
#endif    

	return 0;
}

#if 0
/**
  libmad测试
**/
static int decode(unsigned char const *, unsigned long);
static FILE *fout;
//得到文件的大小
int file_size(char* filename)
{
	int size;	
    FILE *fp=fopen(filename,"r");
    if(!fp) return -1;
    fseek(fp,0L,SEEK_END);
    size=ftell(fp);
    fclose(fp);
    return size;
}

#define MP3_FILE "sinfile3.mp3"
#define PCM_FILE "output17.pcm"
int main()
{
  char *fdm,*fdm2;
  FILE *p;
  char label[10];
  int label_size;
  int fileSize;
  
  fileSize=file_size(MP3_FILE);
  if(_access(PCM_FILE,0)==0)remove(PCM_FILE);//如果存在就删除
  fout=fopen(PCM_FILE,"wb");
  if(fout==NULL){
    printf("create file failed\n");
    return -1;
  }
  p=fopen(MP3_FILE,"rb");
  if(p==NULL){
    printf("open file failed\n");
    return -1;
  }
  if(fread(label,sizeof(char),10,p)>0){
    //计算标签大小
    label_size=((label[6]&0x7F)<<21)|((label[7]&0x7F)<<14)|((label[8]&0x7F)<<7)|((label[9]&0x7F));
    label_size+=10;
    printf("label size=%d\nfileSize=%d\n",label_size,fileSize);
  //分配音频数据帧缓冲区
      fdm2=fdm=malloc(fileSize-label_size);
      if(fdm==NULL){
        printf("malloc failed\n");
      }
      fread(fdm,sizeof(char),label_size-10,p);//去掉标签头数据，剩下的就是音频数据帧
  }else return -1;
 
   printf("read %d bytes\n",fread(fdm,sizeof(char),fileSize-label_size,p));
   printf("decode ok %s\n",decode(fdm,fileSize-label_size)==0?"OK":"failed");
   free(fdm2);
   fclose(p);
   fclose(fout);
  return 0;
}
 
 
struct buffer {
  unsigned char const *start;
  unsigned long length;
};
/**
输入回调函数，一次性写入所有音频数据帧
**/
static
enum mad_flow input(void *data,
		    struct mad_stream *stream)
{
  struct buffer *buffer = data;
 
  if (!buffer->length)
    return MAD_FLOW_STOP;
 
  mad_stream_buffer(stream, buffer->start, buffer->length);
 
  buffer->length = 0;
 
  return MAD_FLOW_CONTINUE;
}

#if 0 
/*
转换24bit pcm为16bit pcm
 */
static signed int scale(mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));
 
  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;
 
  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}
 
/*
输出回调函数，每译码完成一帧音频数据，就调用一次
 */
static
enum mad_flow output(void *data,
		     struct mad_header const *header,
		     struct mad_pcm *pcm)
{
  unsigned int nchannels, nsamples;
  mad_fixed_t const *left_ch, *right_ch;
  unsigned char *buf;
  int index;
 
  nchannels = pcm->channels;
  nsamples  = pcm->length;
  left_ch   = pcm->samples[0];
  right_ch  = pcm->samples[1];
  buf = malloc(nsamples*nchannels*2);
 
  index=0;
  while (nsamples--) {
    signed int sample;
 
    sample = scale(*left_ch++);
    *(buf+2*nchannels*index+0)=(sample >> 0) & 0xff;
    *(buf+2*nchannels*index+1)=(sample >> 8) & 0xff;
 
    if (nchannels == 2) {
      sample = scale(*right_ch++);
      *(buf+2*nchannels*index+2)=(sample >> 0) & 0xff;
      *(buf+2*nchannels*index+3)=(sample >> 8) & 0xff;
    }
    index++;
  }
  fwrite(buf,sizeof(char),pcm->length*nchannels*2,fout);
  free(buf);
  return MAD_FLOW_CONTINUE;
}
#else
static signed int scale(mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 24));
 
  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;
 
  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 24);
}


/*
输出回调函数，每译码完成一帧音频数据，就调用一次
 */
static
enum mad_flow output(void *data,
		     struct mad_header const *header,
		     struct mad_pcm *pcm)
{
  unsigned int nchannels, nsamples;
  mad_fixed_t const *left_ch, *right_ch;
  unsigned char *buf;
  int index;
 
  nchannels = pcm->channels;
  nsamples  = pcm->length;
  left_ch   = pcm->samples[0];
  right_ch  = pcm->samples[1];
  buf = malloc(nsamples*nchannels*3);
 
  index=0;
  while (nsamples--) {
    signed int sample;
 
    sample = scale(*left_ch++);
    *(buf+3*nchannels*index+0)=(sample >> 0) & 0xff;
    *(buf+3*nchannels*index+1)=(sample >> 8) & 0xff;
 	*(buf+3*nchannels*index+2)=(sample >> 16) & 0xff;
	
    if (nchannels == 2) {
      sample = scale(*right_ch++);
      *(buf+3*nchannels*index+3)=(sample >> 0) & 0xff;
      *(buf+3*nchannels*index+4)=(sample >> 8) & 0xff;
	  *(buf+3*nchannels*index+5)=(sample >> 16) & 0xff;
    }
    index++;
  }
  fwrite(buf,sizeof(char),pcm->length*nchannels*3,fout);
  free(buf);
  return MAD_FLOW_CONTINUE;
}
#endif

 
/*
错误回调函数，有错误就调用
 */
static
enum mad_flow error(void *data,
		    struct mad_stream *stream,
		    struct mad_frame *frame)
{
  struct buffer *buffer = data;
 
  fprintf(stderr, "decoding error 0x%04x (%s) at byte offset %u\n",
	  stream->error, mad_stream_errorstr(stream),
	  stream->this_frame - buffer->start);
 
  return MAD_FLOW_CONTINUE;
}
 
/*
译码
 */
static
int decode(unsigned char const *start, unsigned long length)
{
  struct buffer buffer;
  struct mad_decoder decoder;
  int result;
 
  buffer.start  = start;
  buffer.length = length;
  //初始化译码器，设置回调函数
  mad_decoder_init(&decoder, &buffer,
		   input, 0 /* header */, 0 /* filter */, output,
		   error, 0 /* message */);
  //运行译码器
  result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);
 
  mad_decoder_finish(&decoder);
 
  return result;
}
#endif


