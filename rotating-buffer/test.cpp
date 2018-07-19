#include <stdio.h>
#include <stdint.h>

#define BUFFER_LENGTH 3
uint8_t length = 0;

struct ByteWrapper {
    uint8_t x1 : 2;
    uint8_t x2 : 2;
    uint8_t x3 : 2;
    uint8_t x4 : 2;
};

struct Rotating2BitBuffer {
    ByteWrapper buffer[BUFFER_LENGTH];
    int head;
};

Rotating2BitBuffer buf;

uint8_t toByte(ByteWrapper b) {
    uint8_t c = b.x1;
    c = (c << 2) + b.x2;
    c = (c << 2) + b.x3;
    c = (c << 2) + b.x4;
    return c;
}

uint8_t readInByte(ByteWrapper b, char pos) {
    if (pos == 0) {
        return b.x1;
    }
    if (pos == 1) {
        return b.x2;
    }
    if (pos == 2) {
        return b.x3;
    }
    return b.x4;
}

ByteWrapper writeInByte(ByteWrapper b, uint8_t pos, uint8_t data) {
    ByteWrapper c = b;
    if (pos == 0) {
        c.x1 = data;
    }
    if (pos == 1) {
        c.x2 = data;
    }
    if (pos == 2) {
        c.x3 = data;
    }
    if (pos == 3) {
        c.x4 = data;
    }
    return c;
}

uint8_t read(int n) {
    int8_t bitIndex = buf.head - n;
    if (bitIndex < 0) {
        bitIndex = BUFFER_LENGTH * 4 - 2;
    }

    const uint8_t byteIndex = bitIndex / 4;
    const uint8_t bitInByte = bitIndex % 4;
    const ByteWrapper value = buf.buffer[byteIndex];
    return readInByte(value, bitInByte);
}

void write(uint8_t n, uint8_t data) {
    int8_t bitIndex = buf.head - n;
    if (bitIndex < 0) {
        bitIndex = BUFFER_LENGTH * 4 - 2;
    }

    const uint8_t byteIndex = bitIndex / 4;
    const uint8_t bitInByte = bitIndex % 4;
    const ByteWrapper value = buf.buffer[byteIndex];
    printf("Setting %d at byte %d at pos %d\n", data, byteIndex, bitInByte);
    buf.buffer[byteIndex] = writeInByte(value, bitInByte, data);
}

void append(uint8_t data) {
    write(0, data);
    buf.head = (buf.head + 1) % (BUFFER_LENGTH * 4);
}

void printContents() {
    for (int i = 0; i < length; i++) {
        printf("%d: %d\n", i, read(i));
    }
}

int main() {
    /*
    for (int i = 0; i < 4; i++) {
        append(i % 4);
    }
    */

    buf.head = 10;
    write(3, 1);

    length = 12;
    printContents();

    printf("%d", buf.head);

    return 0;
}
