## Setup new environment
- install opencv branch 3.4 for both opencv and - opencv_contrib
- pkg-config --modversion opencv to check opencv version
- install spacenavd using apt-get
- install libspnav for spnav.h (https://github.com/FreeSpacenav/libspnav)
- append library location in /usr/local/lib/libspnav.so.0 to LD_LIBRARY_PATH
- export the variable


## Research Analyses:

https://colab.research.google.com/drive/1srDLOa4F6_Sjn52rExsntMn5R5HiL4c3#scrollTo=pP39AffvM_mI
https://docs.google.com/spreadsheets/d/184RWZbR8IHDHbN_5rxd81s4f3VhG0pPBpd-gD4edSIk/edit#gid=0


## Setup opencv

cmake -D CMAKE_BUILD_TYPE=RELEASE \
	-D CMAKE_INSTALL_PREFIX=/usr/local \
	-D BUILD_WITH_DEBUG_INFO=OFF \
	-D BUILD_DOCS=OFF \
	-D BUILD_EXAMPLES=OFF \
	-D BUILD_TESTS=OFF \
	-D BUILD_opencv_ts=OFF \
	-D BUILD_PERF_TESTS=OFF \
	-D INSTALL_C_EXAMPLES=ON \
	-D INSTALL_PYTHON_EXAMPLES=ON \
	-D OPENCV_EXTRA_MODULES_PATH=/opt/opencv_contrib/modules \
	-D ENABLE_NEON=ON \
	-D WITH_LIBV4L=ON \
        /opt/opencv/

http://pklab.net/?id=392&lang=EN

## Troubleshooting

- VIDIOC_STREAMON: No space left on device \
	fix:\
	sudo rmmod uvcvideo\
	sudo modprobe uvcvideo quirks=128
