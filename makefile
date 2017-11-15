COMPILER = /usr/local/cuda-8.0/bin/nvcc 
COMPILER1 = g++ -g -Wall
EXECUTABLE = startacq
LIBS += -L /usr/lib/x86_64-linux-gnu/ -lspcm_linux -lpthread -lcpgplot -lm -lfftw3 -lstdc++ \
	-I/usr/local/cuda-8.0/samples/common/inc -lm -lpthread -lcufft -L/usr/local/cuda-8.0/lib64
CFLAGS = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
ALL_CFLAGS = -ccbin g++ -I/usr/local/cuda-8.0/samples/common/inc  -m64 \
	-gencode arch=compute_35,code=\"sm_35,compute_35\"
OBJECTS = \
    acq_start.o ./common/ostools/spcm_ostools_linux.o startacq.o gpudedisp.o sem1.o file.o \
    filterbank_header.o strings_equal.o print_version.o send_stuff.o swap_bytes.o\
    mjdnow.o fftthread.o dedisperse_data.o dmdelay.o dmshift.o nrselect.o dedisperse_header.o\
    mixthread.o stopVar.o ignored_channels.o plotFold.o band.o initMutex.o initThreads.o\
    joinThreads.o vlbi.o date.o rdfHeader.o read_polyco.o crc-16.o freeBuffers.o \
    read_par.o gpufft.o getchirp.o allocateBuffers.o allocateSharedMemory.o \
    freeSharedMemory.o ips.o read_ips.o timenow.o \
    
	 
OBJECTS1 = \
    acq.o ./common/ostools/spcm_ostools_linux.o allocateSharedMemory.o freeSharedMemory.o \
    file.o mjdnow.o sem1.o stopVar.o read_par.o allocateBuffers.o getchirp.o timenow.o read_ips.o
         
all: acq acq_start acq_stop

$(EXECUTABLE):	$(OBJECTS)
	$(COMPILER) $(ALL_CFLAGS) $(CFLAGS) -o $(EXECUTABLE) $(LIBS) $(OBJECTS)

%.o:%.cpp
	$(COMPILER) $(ALL_CFLAGS) $(CFLAGS) -o $*.o -c $*.cpp $(LIBS)

%.o:%.c
	$(COMPILER) $(ALL_CFLAGS) $(CFLAGS) -o $*.o -c $*.c $(LIBS)

%.o:%.cu
	$(COMPILER) $(ALL_CFLAGS) $(CFLAGS) -o $*.o -c $*.cu $(LIBS)

acq_start: $(OBJECTS)
	$(COMPILER) $(ALL_CFLAGS) $(CFLAGS) -o acq_start $(LIBS) $(OBJECTS)

acq_stop: acq_stop.o 
	$(COMPILER) $(CFLAGS) -o acq_stop acq_stop.c sem1.c 

cleanObj:
	rm -rf ./*~
	
acq:$(OBJECTS1)
	$(COMPILER) $(ALL_CFLAGS) $(CFLAGS) -o acq $(LIBS) $(OBJECTS1)
    
clean:
	rm -f -r ./*/*.o
	rm -f -r ./*/*/*.o
	rm -f -r ./*~
	rm -rf *.o

