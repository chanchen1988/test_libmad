# ���ñ�����
CC := gcc
 
# �������ɵ�Ŀ���ļ�
TARGET := myprogram
 
# ����Դ�ļ�
SRCS := version.c fixed.c bit.c timer.c stream.c frame.c synth.c decoder.c layer12.c layer3.c huffman.c main.c
 
# ���ñ������
$(TARGET): $(SRCS:.c=.o)
	$(CC) -o $@ $^
 
# ����.o�ļ��Ĺ���
%.o: %.c
	$(CC) -DHAVE_CONFIG_H -I. -I. -I. -DFPM_DEFAULT -Wall -g -O -fforce-addr -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -fexpensive-optimizations -fregmove -fschedule-insns2 -c $< -o $@ 
 
# �������
clean:
	rm -f $(TARGET) $(SRCS:.c=.o)

#all:
#	gcc -DHAVE_CONFIG_H -I. -I. -I. -DFPM_DEFAULT -Wall -g -O -fforce-addr -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -fexpensive-optimizations -fregmove -fschedule-insns2 -c version.c -o version.o
	
	
#.PHONY: all clean
	