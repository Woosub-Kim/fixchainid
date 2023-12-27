# fixchainid
fixchainid of PDBs

### install

##### linux
git clone https://github.com/Woosub-Kim/fixchainid.git
cd fixchainid
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=. .
make
make install 
export PATH=$(pwd)/bin/:$PATH


##### macos
git clone https://github.com/Woosub-Kim/fixchainid.git
cd fixchainid
mkdir build
cd build
CC=gcc-13 CXX=g++-13 cmake -DCMAKE_BUILD_TYPE=Release ..
make
make install 
export PATH=$(pwd)/bin/:$PATH

### how to use
OMP_NUM_THREADS=n fixchainid INPUT_DIR OUTPUT_DIR
