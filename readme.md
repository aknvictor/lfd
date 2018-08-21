
VIDIOC_STREAMON: No space left on device

sudo rmmod uvcvideo
sudo modprobe uvcvideo quirks=128

Analyses:

https://colab.research.google.com/drive/1srDLOa4F6_Sjn52rExsntMn5R5HiL4c3#scrollTo=pP39AffvM_mI
https://docs.google.com/spreadsheets/d/184RWZbR8IHDHbN_5rxd81s4f3VhG0pPBpd-gD4edSIk/edit#gid=0


install opencv branch 3.4 for both opencv and opencv_contrib
pkg-config --modversion opencv to check opencv version
install spacenavd using apt-get
install libspnav for spnav.h
append library location in /usr/local/lib/libspnav.so.0 to LD_LIBRARY_PATH
export the variable


cmake BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D OPENCV_EXTRA_MODULES_PATH=/opt/opencv_contrib/modules /opt/opencv/