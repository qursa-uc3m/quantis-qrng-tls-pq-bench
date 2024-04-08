# Multi-stage build: First the full builder image:

# First: global build arguments:

# liboqs build type variant; maximum portability of image:
ARG LIBOQS_BUILD_DEFINES="-DOQS_DIST_BUILD=ON"

# base install path
ARG BASEDIR="/opt"

# installation paths
ARG INSTALLDIR=${BASEDIR}/nginx

# defines the QSC signature algorithm used for the certificates:
ARG SIG_ALG="dilithium3"

# defines the list of default groups to be activated in nginx-openssl config:
ARG DEFAULT_GROUPS=x25519:x448:kyber512:p256_kyber512:kyber768:p384_kyber768:kyber1024:p521_kyber1024

# define the nginx version to include
ARG NGINX_VERSION=1.23.3

# Define the degree of parallelism when building the image; leave the number away only if you know what you are doing
ARG MAKE_DEFINES="-j 18"

#! QUANTIS INPUT ARGUMENTS:
ARG QRNG_DEVICE=USB
ARG QRNG_DEBUG=OFF
ARG QUANTIS_QRNG=true
ARG QUANTIS_LIB=YES
ARG DEVICE_NUMBER=0
ARG XOR_RANDOM=ON
ARG MEASURE_RNG=OFF

FROM ubuntu:22.04 as intermediate

# Take in global args
ARG BASEDIR
ARG LIBOQS_BUILD_DEFINES
ARG INSTALLDIR
ARG SIG_ALG
ARG NGINX_VERSION
ARG MAKE_DEFINES
ARG DEFAULT_GROUPS
ARG OSSLDIR=${BASEDIR}/openssl/.openssl

#! required for Quantis
ARG QUANTIS_QRNG
ENV QUANTIS_QRNG=${QUANTIS_QRNG}
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    libtool automake autoconf cmake ninja-build \
    make \
    openssl libssl-dev \
    git wget libpcre3-dev

# get OQS sources
WORKDIR /opt
#! We fix some releases:
RUN git clone --depth 1 --branch 0.8.0 https://github.com/open-quantum-safe/liboqs && \
    git clone --depth 1 --branch 0.5.0 https://github.com/open-quantum-safe/oqs-provider.git && \
    git clone https://github.com/openssl/openssl.git && \
    cd openssl && \
    git checkout 7197abddb891933f52ec84dafb41b685d4a1d122 && \
    cd .. && \
    wget nginx.org/download/nginx-${NGINX_VERSION}.tar.gz && tar -zxvf nginx-${NGINX_VERSION}.tar.gz;

# build liboqs (static only)
WORKDIR /opt/liboqs
RUN mkdir build && cd build && cmake -G"Ninja" ${LIBOQS_BUILD_DEFINES} -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${INSTALLDIR} .. && ninja && ninja install

# build nginx (also building openssl)
WORKDIR /opt/nginx-${NGINX_VERSION}
RUN ./configure --prefix=${INSTALLDIR} \
                --with-debug \
                --with-http_ssl_module --with-openssl=/opt/openssl \
                --with-http_v2_module \
                --without-http_gzip_module && \
    make ${MAKE_DEFINES} && make install;

# create openssl.cnf activating oqsprovider & setting default groups
RUN mkdir -p ${OSSLDIR=}/ssl && cp /opt/openssl/apps/openssl.cnf ${OSSLDIR}/ssl/ && sed -i "s/default = default_sect/default = default_sect\noqsprovider = oqsprovider_sect/g" ${OSSLDIR}/ssl/openssl.cnf && sed -i "s/\[default_sect\]/\[default_sect\]\nactivate = 1\n\[oqsprovider_sect\]\nactivate = 1\n/g" ${OSSLDIR}/ssl/openssl.cnf && sed -i "s/providers = provider_sect/providers = provider_sect\nssl_conf = ssl_sect\n\n\[ssl_sect\]\nsystem_default = system_default_sect\n\n\[system_default_sect\]\nGroups = \$ENV\:\:DEFAULT_GROUPS\n/g" ${OSSLDIR}/ssl/openssl.cnf && sed -i "s/HOME\t\t\t= ./HOME\t\t= .\nDEFAULT_GROUPS\t= ${DEFAULT_GROUPS}/g" ${OSSLDIR}/ssl/openssl.cnf

# build oqsprovider
WORKDIR /opt/oqs-provider

RUN ln -s /opt/nginx/include/oqs ${OSSLDIR}/include && rm -rf build && cmake -DCMAKE_BUILD_TYPE=Debug -DOPENSSL_ROOT_DIR=${OSSLDIR} -DCMAKE_PREFIX_PATH=$INSTALLDIR -S . -B build && cmake --build build && mkdir -p ${OSSLDIR}/lib64/ossl-modules && cp build/lib/oqsprovider.so ${OSSLDIR}/lib64/ossl-modules && rm -rf ${INSTALLDIR}/lib64

WORKDIR ${INSTALLDIR}


ENV PATH ${INSTALLDIR}/sbin:${OSSLDIR}/bin:$PATH
# begin optimizing sizes:
RUN strip ${OSSLDIR}/lib/*.a ${OSSLDIR}/lib64/ossl-modules/oqsprovider.so ${INSTALLDIR}/sbin/* ${INSTALLDIR}/sbin/*
RUN set -x && \
    mkdir cacert && \
    mkdir pki && \
    openssl req -x509 -new -newkey ${SIG_ALG} -keyout CA.key -out cacert/CA.crt -nodes -subj "/CN=oqstest CA" -days 365 -config ${OSSLDIR}/ssl/openssl.cnf && \
    openssl req -new -newkey ${SIG_ALG} -keyout pki/server.key -out server.csr -nodes -subj "/CN=oqs-nginx" -config ${OSSLDIR}/ssl/openssl.cnf && \
    openssl x509 -req -in server.csr -out pki/server.crt -CA cacert/CA.crt -CAkey CA.key -CAcreateserial -days 365

# further optimize sizes:
RUN rm ${OSSLDIR}/bin/*

# second stage:
FROM ubuntu:22.04
# Take in global args
ARG LIBOQS_BUILD_DEFINES
ARG INSTALLDIR
ARG SIG_ALG
ARG BASEDIR
ARG OSSLDIR=${BASEDIR}/openssl/.openssl

#! required for Quantis
ARG QUANTIS_QRNG
ARG QRNG_DEBUG
ARG QRNG_DEVICE
ARG QUANTIS_LIB
ARG DEVICE_NUMBER
ARG XOR_RANDOM
ARG ARG MEASURE_RNG
ENV QUANTIS_QRNG=${QUANTIS_QRNG}
ENV QRNG_DEBUG=${QRNG_DEBUG}
ENV QRNG_DEVICE=${QRNG_DEVICE}
ENV QUANTIS_LIB=${QUANTIS_LIB}
ENV DEVICE_NUMBER=${DEVICE_NUMBER}
ENV XOR_RANDOM=${XOR_RANDOM}
ENV MEASURE_RNG=${MEASURE_RNG}
ENV DEBIAN_FRONTEND noninteractive

LABEL version="2"

#RUN apk add pcre-dev
RUN apt-get update && apt-get install -y libpcre3-dev gnupg gpg-agent

# Only retain the ${*_PATH} contents in the final image
COPY --from=intermediate ${INSTALLDIR} ${INSTALLDIR}
COPY --from=intermediate ${OSSLDIR} ${OSSLDIR}
COPY nginx-conf/ ${INSTALLDIR}/nginx-conf

WORKDIR ${INSTALLDIR}

#! -----QUANTIS LIBRARIES installation-----
#! QRNG dependencies:
# QUANTIS LIBRARIES installation
COPY dependencies/pcie-chip-20.2.4-linux/Libs-Apps /tmp/dependencies/pcie-chip-20.2.4-linux/Libs-Apps
COPY dependencies/pcie-chip-20.2.4-linux/License.txt /tmp/dependencies/pcie-chip-20.2.4-linux/License.txt
COPY dependencies/quantis-qrng-openssl-integration /tmp/dependencies/quantis-qrng-openssl-integration
COPY dependencies/install_quantis_libraries.sh /tmp/dependencies/install_quantis_libraries.sh
COPY dependencies/read_shm /tmp/dependencies/read_shm

RUN if [ "${QUANTIS_QRNG}" = "true" ]; then \
    apt-get install -y --no-install-recommends pciutils usbutils udev software-properties-common wget && \
    # Add your libraries to the Docker image \
    cp -r /tmp/dependencies/pcie-chip-20.2.4-linux/ /opt/quantis && \
    # Copy your installation script into the image \
    cp /tmp/dependencies/install_quantis_libraries.sh /usr/local/bin/install_quantis_libraries.sh && \
    # Set the permissions of the script to be executable \
    chmod +x /usr/local/bin/install_quantis_libraries.sh && \
    # Run your installation script with specified LIB_PATH \
    /usr/local/bin/install_quantis_libraries.sh -t /opt/quantis -p /opt/quantis; \
fi
#! -----QUANTIS LIBRARIES installation-----

#! -----QRNG OPENSSL PROVIDER INSTALLATION-----
RUN if [ "${QUANTIS_QRNG}" = "true" ]; then \
    # This seems to be necessary \
    ranlib /opt/openssl/.openssl/lib/libcrypto.a && \
    cp -r /tmp/dependencies/quantis-qrng-openssl-integration/qrng_openssl_provider/ /opt/qrng_openssl_provider && \
    mkdir /opt/qrng_openssl_provider/build && \
    cd /opt/qrng_openssl_provider/build && \
    cmake -DDEVICE_TYPE=${QRNG_DEVICE} \
        -DQUANTIS_LIB=${QUANTIS_LIB} \
        -DDEVICE_NUMBER=${DEVICE_NUMBER} \
        -DDEBUG=${QRNG_DEBUG} \
        -DOPENSSL_ROOT_DIR=${OSSLDIR} \
        -DXOR_RANDOM=${XOR_RANDOM} \
        -DMEASURE_RNG=${MEASURE_RNG} .. && \
    make && \
    mkdir -p /opt/quantis/providers/ && \
    cp libcustom_qrng_provider.so /opt/quantis/providers/ && \
    ls -l /opt/quantis/providers && sleep 10 && \
    echo "\n[customprovider_sect]\nactivate = 1\nmodule = /opt/quantis/providers/libcustom_qrng_provider.so" >> ${OSSLDIR}/ssl/openssl.cnf && \
    sed -i "/\[provider_sect\]/a customprovider = customprovider_sect" ${OSSLDIR}/ssl/openssl.cnf; \
fi
#! -----QRNG OPENSSL PROVIDER INSTALLATION-----

#! -----QRNG OPENSSL PROVIDER INSTALLATION - MEASURE-----
RUN if [ "${MEASURE_RNG}" = "ON" ]; then \
    # Compile read_shm
    gcc -o /tmp/dependencies/read_shm/read_shm /tmp/dependencies/read_shm/read_shm.c -lrt && \
    mv /tmp/dependencies/read_shm/read_shm /usr/local/bin/; \
fi
#! -----QRNG OPENSSL PROVIDER INSTALLATION - MEASURE-----

# forward request and error logs to docker log collector
RUN ln -sf /dev/stdout ${INSTALLDIR}/logs/access.log && \
    ln -sf /dev/stderr ${INSTALLDIR}/logs/error.log;

# This expose command needs to be in line with what's spec'd in nginx.conf:
EXPOSE 4433

# Ensure nginx just runs
ENV PATH ${INSTALLDIR}/sbin:$PATH

STOPSIGNAL SIGTERM

# Enable a normal user to create new server keys off set CA
RUN groupadd -g 1000 oqs && useradd --uid 1000 -g oqs oqs && chown -R oqs:oqs ${INSTALLDIR}
USER oqs

CMD ["nginx", "-c", "nginx-conf/nginx.conf", "-g", "daemon off;"]