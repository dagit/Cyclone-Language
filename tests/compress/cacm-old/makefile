CFLAGS = -O

all:			adaptive_encode adaptive_decode

adaptive_encode:	encode.o adaptive_model.o bit_output.o \
			  arithmetic_encode.o
			cc encode.o adaptive_model.o bit_output.o \
			  arithmetic_encode.o -o adaptive_encode

adaptive_decode:	decode.o adaptive_model.o bit_input.o \
			  arithmetic_decode.o
			cc decode.o adaptive_model.o bit_input.o \
			  arithmetic_decode.o -o adaptive_decode

encode.o:		encode.c model.h
decode.o:		decode.c model.h

adaptive_model.o:	adaptive_model.c model.h

arithmetic_encode.o:	arithmetic_encode.c arithmetic_coding.h
arithmetic_decode.o:	arithmetic_decode.c arithmetic_coding.h

bit_input.o:		bit_input.c
bit_output.o:		bit_output.c
