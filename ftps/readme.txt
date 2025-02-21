
make bianyi 

arm-linux-gnueabihf-g++ sample.cpp -c ftplibpp -lftp++


gcc test.c -o test -L /usr/local/openssl/lib -lssl -lcrypto 

arm-linux-gnueabihf-gcc test.c -o test -L /opt/toolchain/7.5.0/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/lib -lssl -lcrypto

./test put power-inspection/phone-file/test.jpg ./test.jpg 10.106.249.31 30021 ftpUser Password@1

./test get ./image.jpg power-inspection/phone-file/image.jpg 10.106.249.31 30021 ftpUser Password@1

valgrind --tool=memcheck --log-file=log.txt --leak-check=yes  ./test

ftpuser dp12345678
