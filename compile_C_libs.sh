#!/bin/bash




printf "Cleaning files\n"
$(rm ./*.o &> /dev/null)
$(rm main.exe &> /dev/null) 

function compile_osdeps {
    printf "Compiling OSDEPS ..."
	cd osdep
    $(rm ./*.o &> /dev/null)

	gcc -g -W -Wall -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude   -c -o radiotap.o radiotap/radiotap.c
	gcc -g -W -Wall -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -fPIC -I..    -c -o osdep.o osdep.c
	gcc -g -W -Wall -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -fPIC -I..    -c -o network.o network.c
	gcc -g -W -Wall -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -fPIC -I..    -c -o file.o file.c
	gcc -g -W -Wall -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -fPIC -I..    -c -o linux.o linux.c
	gcc -g -W -Wall -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -fPIC -I..    -c -o linux_tap.o linux_tap.c
	gcc -g -W -Wall -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -fPIC -I..    -c -o common.o common.c
	ar cru libosdep.a  osdep.o network.o file.o linux.o linux_tap.o radiotap.o common.o
	ranlib libosdep.a
	cd ..
    printf "done\n"
}

function compile_libs {
    printf "Compiling LIBS   ..."
    cd libs
    $(rm ./*.o &> /dev/null)
    gcc -g -W -Wall -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude   -c -o common.o common.c
    gcc -g -W -Wall -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude   -c -o uniqueiv.o uniqueiv.c
    cd ..
    printf "done\n"
}

#compile_osdeps
#compile_libs

gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o main.o main.c 
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o devices.o devices.c 
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o file.o file.c 
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o vendors.o vendors.c 
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o capture.o capture.c 
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o utils.o utils.c 
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o stream.o stream.c 
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o http_server.o http_server.c 
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o http_client.o http_client.c 
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o http_request.o http_request.c 


gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  main.o file.o vendors.o http_server.o http_client.o http_request.o capture.o devices.o utils.o stream.o -o main.exe -Losdep  -losdep -lnl  -lssl -lpthread

# $(rm ./*.o &> /dev/null)
printf "OK\n"

