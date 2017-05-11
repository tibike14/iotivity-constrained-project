 #!/bin/bash         

# clone the iotivity-constrained repository and the cbor repository 
#Configure the linking between the original source and the project folders

cd .. 
#git clone https://github.com/iotivity/iotivity-constrained.git
#rm -r iotivity-constrained/apps
#rm  iotivity-constrained/port/linux/Makefile

cd iotivity-constrained
#git clone https://github.com/01org/tinycbor.git deps/tinycbor -b v0.4

sudo ln -s ../iotivity-constrained-project/apps
cd port/linux
sudo ln -s ../../../iotivity-constrained-project/linux/Makefile

#set environment variable $IPV4 to 1 for building with IPv4 connectivity
export IPV4=1 
#make DEBUG=1 
make --always-make
