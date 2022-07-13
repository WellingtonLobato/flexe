sudo apt-get install build-essential gcc g++ bison flex perl python python3 qt5-default libqt5opengl5-dev tcl-dev tk-dev libxml2-dev zlib1g-dev default-jre doxygen graphviz libwebkitgtk-1.0 openscenegraph-plugin-osgearth libosgearth-dev openmpi-bin libopenmpi-dev libpcap-dev gnome-color-chooser nemiver
wget https://github.com/omnetpp/omnetpp/releases/download/omnetpp-5.7/omnetpp-5.7-linux-x86_64.tgz
tar zxvf omnetpp-5.7-linux-x86_64.tgz
cd omnetpp-5.7/
./configure
make -release
wget https://github.com/opencv/opencv/archive/4.5.5.zip -O opencv.zip
unzip opencv.zip
wget https://github.com/opencv/opencv_contrib/archive/refs/tags/4.5.5.zip -O opencv_contrib.zip
unzip opencv_contrib.zip
cd opencv-4.5.5/
mkdir -p build && cd build
cmake -DOPENCV_EXTRA_MODULES_PATH=/home/veins-fl/opencv_contrib-4.5.5/modules ..
cmake --build .
make -j4
make install
