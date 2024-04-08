#!/bin/bash

# Set default values
INSTALL_DIR="/opt/quantis"
LIB_PATH=""

# Process options
while getopts t:p: OPTION; do
    case $OPTION in
        t)
            INSTALL_DIR=$OPTARG
            ;;
        p)
            LIB_PATH=$OPTARG
            ;;
        ?)
            echo "Usage: $0 [-t target] -p library_path"
            exit 1
            ;;
    esac
done

# Check LIB_PATH is set
if [[ -z $LIB_PATH ]]; then
    echo "Library path (-p) must be provided."
    exit 1
fi


# Function to check if command was successful
function check_cmd {
  if [ $? -ne 0 ]; then
    echo "Previous command failed, exiting."
    exit 1
  fi
}

UBUNTU_VERSION=$(grep -oP '(?<=VERSION_ID=").*(?=")' /etc/os-release)

echo "Installing Quantis libraries in Ubuntu ${UBUNTU_VERSION}..."

if [ "$UBUNTU_VERSION" = "20.04" ]; then
  echo "Installing libqt4..."
  add-apt-repository -y ppa:rock-core/qt4
  check_cmd
  apt-get install -y qt4-dev-tools
  check_cmd
elif [ "$UBUNTU_VERSION" = "22.04" ]; then
  echo "Installing libqt4..."
  add-apt-repository ppa:ubuntuhandbook1/ppa
  check_cmd
  apt-get update --allow-insecure-repositories
  check_cmd
  apt-get install -y libqt4-dev
  check_cmd
else
  echo "Ubuntu version not supported."
  exit 1
fi

# Recompiling the Quantis Library
echo "Adding other dependencies..."
apt-get update
check_cmd
apt-get install -y libusb-1.0-0 libusb-1.0-0-dev build-essential cmake pkg-config default-jdk \
    libboost-date-time-dev libboost-program-options-dev libboost-thread-dev libboost-filesystem-dev
check_cmd

# Compilation
cd ${INSTALL_DIR}/Libs-Apps/
rm -r build
mkdir build
check_cmd
cd build
cmake -DCMAKE_CXX_STANDARD=14 -DDISABLE_EASYQUANTIS=1 ..
make
check_cmd

# Append the lines to the ~/.bashrc file
echo "Appending lines to ~/.bashrc file..."
echo '## QUANTIS EXECUTABLES AND LIBRARIES' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/opt/quantis/Libs-Apps/build/Quantis"' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/opt/quantis/Libs-Apps/build/QuantisExtensions"' >> ~/.bashrc

echo "Quantis installation completed..."