#!/bin/bash


printf "Compiling and see\n"

rm ./done.exe
$(rm ./*.o  &> /dev/null)
$(rm ./*.a  &> /dev/null)
$(rm ./Go/*.a  &> /dev/null)


cp ./C/osdep/libosdep.a .

gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o utils.o ./C/utils.c
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o stream.o ./C/stream.c
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o vendors.o ./C/vendors.c
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o capture.o ./C/capture.c
gcc -g -w  -O3  -DOLD_SSE_CORE=1 -pthread -D_FILE_OFFSET_BITS=64 -D_REVISION=0  -DCONFIG_LIBNL  -fstack-protector-strong -Wno-unused-but-set-variable -Wno-array-bounds -Iinclude  -c -o devices.o ./C/devices.c
gcc -O2 -c ./C/c_main.c

ar q libgb.a c_main.o utils.o stream.o vendors.o devices.o capture.o

cp libgb.a ./Go/libgb.a
cp libgb.a ./Go/liblgb.a

cp libosdep.a ./Go/libosdep.a
cp libosdep.a ./Go/liblosdep.a

go build -o done.exe ./Go

$(rm ./*.o  &> /dev/null)
$(rm ./*.a  &> /dev/null)
$(rm ./Go/*.a  &> /dev/null)

sudo ./done.exe
