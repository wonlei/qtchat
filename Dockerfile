FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential cmake \
    qtbase5-dev libqt5sql5-mysql libqt5network5 \
    libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --target WonChatServer -j$(nproc)

EXPOSE 8888

CMD ["./build/Server/WonChatServer"]
