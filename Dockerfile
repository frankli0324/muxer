FROM debian:bullseye AS build

RUN apt update && apt install -y wget gnupg && \
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    echo "deb http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-12 main\n\
    deb-src http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-12 main" >> /etc/apt/sources.list && \
    apt update && apt install -y \
    clang-12 libc++-12-dev libc++abi-12-dev cmake make
RUN ln -s /usr/bin/clang++-12 /usr/bin/clang++ && ln -s /usr/bin/clang-12 /usr/bin/clang

RUN mkdir -p /opt/boost
WORKDIR /opt/boost

RUN wget https://dl.bintray.com/boostorg/release/1.75.0/source/boost_1_75_0.tar.gz
RUN tar xzvf boost_1_75_0.tar.gz && cd boost_1_75_0 && \
    ./bootstrap.sh --with-toolset=clang && \
    ./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" && \
    ./b2 headers && ./b2 install

COPY . /opt/muxer
RUN mkdir /opt/muxer/build
WORKDIR /opt/muxer/build
RUN cmake .. && make -j4

FROM debian:bullseye-slim

RUN apt-get update && apt-get install -y wget gnupg && \
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    echo "deb http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-12 main\n\
    deb-src http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-12 main" >> /etc/apt/sources.list && \
    apt-get update && apt-get install -y \
    libc++1-12 libc++abi1-12 && \
    apt-get remove --autoremove -y wget gnupg && \
    apt-get clean && rm -rf /var/lib/apt/lists/*
COPY --from=build /opt/muxer/build/muxer /usr/bin/muxer
CMD [ "muxer" ]
