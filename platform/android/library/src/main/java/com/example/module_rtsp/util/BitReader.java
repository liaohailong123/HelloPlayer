package com.example.module_rtsp.util;

import java.nio.ByteBuffer;

/**
 * Author: liaohailong
 * Date: 2024/12/6
 * Time: 10:58
 * Description:
 **/
public class BitReader {
    private final ByteBuffer buffer;
    private int bitOffset = 0;

    public BitReader(ByteBuffer buffer) {
        this.buffer = buffer;
    }

    public boolean readBit() {
        int byteOffset = bitOffset / 8;
        int bitInByte = 7 - (bitOffset % 8);
        bitOffset++;
        return ((buffer.get(byteOffset) >> bitInByte) & 0x01) == 1;
    }

    public int readUE() {
        int leadingZeroBits = 0;
        while (!readBit() && leadingZeroBits < 32) {
            leadingZeroBits++;
        }
        int codeNum = (1 << leadingZeroBits) - 1;
        for (int i = 0; i < leadingZeroBits; i++) {
            if (readBit()) {
                codeNum += 1 << (leadingZeroBits - i - 1);
            }
        }
        return codeNum;
    }

    public int readSE() {
        int value = readUE();
        if ((value & 0x01) == 0) {
            value = -(value >> 1);
        } else {
            value = (value + 1) >> 1;
        }
        return value;
    }

    public void skipBits(int numBits) {
        bitOffset += numBits;
    }

    public int peekUE() {
        int originalBitOffset = bitOffset;
        int value = readUE();
        bitOffset = originalBitOffset;
        return value;
    }
}
