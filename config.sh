 #!/bin/bash         

#Configure the linking between the original source and the project folders

echo $PWD
cd .. 
echo $PWD
sudo rm -r iotivity-constrained/apps
sudo rm  iotivity-constrained/port/linux/Makefile
cd iotivity-constrained
sudo ln -s ../iotivity-constrained-project/apps
cd port/linux
sudo ln -s ../../../iotivity-constrained-project/linux/Makefile
