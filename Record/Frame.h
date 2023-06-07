#ifndef FRAME_H
#define FRAME_H
#include <string>

enum FrameFmt{
    Frame_Fmt_BGRA,
    Frame_Fmt_RGB32,
};
struct Frame
{

public:
    Frame(FrameFmt fmt,int size,int width, int height, int64_t count) {
        this->fmt = fmt;
        this->size = size;
        this->width = width;
        this->height = height;
        this->count = count;
        this->data = new uint8_t[this->size];
    }
    ~Frame() {
        delete[] this->data;
        this->data = nullptr;
    }
    FrameFmt fmt;
    int size;
    int width;
    int height;
    uint8_t* data;
    int64_t  count;
};


#endif // FRAME_H
