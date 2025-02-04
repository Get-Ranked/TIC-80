// MIT License

// Copyright (c) 2021 Vadim Grigoruk @nesbox // grigoruk@gmail.com

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <tic80_types.h>

typedef struct
{
    u8* data;
    u32 size;
} png_buffer;

typedef union
{
    struct
    {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };

    u8 data[4];

    u32 value;
} png_rgba;

typedef struct
{
    s32 width;
    s32 height;

    union
    {
        png_rgba* pixels;
        u32* values;
        u8* data;
    };
} png_img;

png_buffer png_create(s32 size);

png_img png_read(png_buffer buf);
png_buffer png_write(png_img src);

png_buffer png_encode(png_buffer cover, png_buffer cart);
png_buffer png_decode(png_buffer cover);
