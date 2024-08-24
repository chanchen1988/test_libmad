# 设置编译器
CC := gcc
 
# 设置生成的目标文件
TARGET := myprogram
 
# 设置源文件
SRCS := version.c fixed.c bit.c timer.c stream.c frame.c synth.c decoder.c layer12.c layer3.c huffman.c main.c
 
# 设置编译规则
$(TARGET): $(SRCS:.c=.o)
	$(CC) -o $@ $^
 
# 设置.o文件的规则
%.o: %.c
	$(CC) -DHAVE_CONFIG_H -I. -I. -I. -DFPM_DEFAULT -Wall -g -O -fforce-addr -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -fexpensive-optimizations -fregmove -fschedule-insns2 -c $< -o $@ 
 
# 清理规则
clean:
	rm -f $(TARGET) $(SRCS:.c=.o)

#all:
#	gcc -DHAVE_CONFIG_H -I. -I. -I. -DFPM_DEFAULT -Wall -g -O -fforce-addr -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -fexpensive-optimizations -fregmove -fschedule-insns2 -c version.c -o version.o
	
	
#.PHONY: all clean
	