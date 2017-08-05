# McFTP
Multicore Fast Thermal Prototyping framework

This is the very first version. I am currently working on debugging, writing 
comments and tutorials of this program. So, there may be bugs in the code...

(1) Very simple instruction of installation:

Prerequisites:
OS: POSIX compliant systems
third-party tools:  
    cpupower, to control the running frequency
    lm-sensors, to read temperature sensor results
hardware: The CPU should have built-in temperature sensors

Installation:
# go to the program build directory
a. cd build
# compile the program using cmake
b. cmake ..

run the program:
# first we must change the cpu frquency governor as userspace
a. sudo cpupower -c all frequency-set -g userspace 
# then go to the program directory
a. cd build
# run the program 
b. sudo ./demo XMLFILENAME

The XMLFILENAME should specify the path to the xml file which describe the settings of
the experiment.


    
