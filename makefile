all: main.o BTrack.o OnsetDetectionFunction.o kiss_fft.o samplerate.o src_sinc.o src_zoh.o src_linear.o
	g++ -g -Wall -o prog main.o BTrack.o OnsetDetectionFunction.o kiss_fft.o samplerate.o src_sinc.o src_zoh.o src_linear.o -lrt
	
main.o: main.cpp BTrack/BTrack.h
	g++ -g -Wall -c main.cpp
	
BTrack.o: BTrack/BTrack.h BTrack/BTrack.cpp BTrack/OnsetDetectionFunction.h BTrack/CircularBuffer.h BTrack/src/samplerate.h
	g++ -g -Wall -DUSE_KISS_FFT -c BTrack/BTrack.cpp
	
OnsetDetectionFunction.o: BTrack/OnsetDetectionFunction.cpp BTrack/OnsetDetectionFunction.h
	g++ -g -Wall -DUSE_KISS_FFT -c BTrack/OnsetDetectionFunction.cpp
	
kiss_fft.o: BTrack/kiss_fft/kiss_fft.c BTrack/kiss_fft/kiss_fft.h BTrack/kiss_fft/_kiss_fft_guts.h
	g++ -g -Wall -c BTrack/kiss_fft/kiss_fft.c

samplerate.o: BTrack/src/samplerate.h BTrack/src/samplerate.c BTrack/src/common.h
	g++ -g -Wall -c BTrack/src/samplerate.c

src_sinc.o: BTrack/src/src_sinc.c BTrack/src/common.h
	g++ -g -Wall -c BTrack/src/src_sinc.c

src_zoh.o: BTrack/src/src_zoh.c BTrack/src/common.h
	g++ -g -Wall -c BTrack/src/src_zoh.c

src_linear.o: BTrack/src/src_linear.c BTrack/src/common.h
	g++ -g -Wall -c BTrack/src/src_linear.c

clean: 
	rm *.o
	rm prog
