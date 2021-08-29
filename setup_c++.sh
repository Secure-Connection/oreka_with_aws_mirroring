base_path=$(pwd)

# install dependencies
sudo apt-get install autoconf libtool libboost-all-dev libcurl4-openssl-dev libssl-dev libboost-dev libapr1-dev liblog4cxx-dev libpcap-dev libxerces-c-dev libsndfile1-dev libspeex-dev

# Download and build opus and silk libraries
git clone https://github.com/OrecX/dependencies.git
tar -xf dependencies/opus-1.2.1.tar.gz
cd opus-1.2.1
./configure --prefix=/opt/opus/
make CFLAGS="-fPIC -msse4.1"
sudo make install
sudo cp /opt/opus/lib/libopus.a /opt/opus/lib/libopusstatic.a
cd ..
sudo tar -xf dependencies/silk.tgz -C /opt/
cd /opt/silk/SILKCodec/SILK_SDK_SRC_FIX/
sudo make clean lib

# Clone oreka
cd $base_path
git clone https://github.com/Secure-Connection/oreka_with_aws_mirroring.git
cd oreka_with_aws_mirroring

# Build orkbasecxx on the command line:
cd orkbasecxx
autoreconf -i
./configure CXX=g++
make
sudo make install
cd ..

# Build orkaudio on the command line:
cd orkaudio
autoreconf -i
./configure CXX=g++
make
sudo make install