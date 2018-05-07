# McFTP
Multicore Fast Thermal Prototyping framework

This is the first version. I am currently working on debugging, writing 
comments and tutorials of this program. So, there may be bugs in the code...

## Very simple instruction of installation:  

### Prerequisites:
OS: POSIX compliant systems  
third-party tools:    
    (1)cpupower, to control the running frequency  
    (2)lm-sensors, to read temperature sensor results  
hardware: The CPU should have built-in temperature sensors  

### Installation:
#### change the CMakeLists.txt in the top directory, if necessary
 by default, the two feature, reading hardware temperature sensors and estimating 
temperature based on performance counters, are both enabled. The user can disable 
and enable them by changing the line 3 and 4 of the CMakeLists.txt in the top 
directory. For example, to disable reading hardware sensors, change line 3 to:
set(HARD_TEMPERATURE_SENSOR_ENABLE "OFF")
#### go to the program build directory
 cd build
#### cmake
 cmake ..
#### compile the program 
 make

### run the program:
#### first we must change the cpu frquency governor as userspace
a. sudo cpupower -c all frequency-set -g userspace 
#### then go to the program directory
b. cd build
#### run the program 
c. sudo ./mcftp XMLFILENAME  
The XMLFILENAME should specify the path to the xml file which describes the settings of
the experiment.


    
