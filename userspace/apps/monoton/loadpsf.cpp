#include "loadpsf.h"
#include "liballoc_1_1.h"
// #include <syscalls.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_NO_STDIO
#define STBI_NO_SIMD

#define STB_IMAGE_RESIZE_IMPLEMENTATION

void *memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *c = (const unsigned char *)src;
    for (size_t i = 0; i < n; i++)
        d[i] = c[i];
    return dest;
}

void *memset(void *dest, int data, size_t nbytes)
{
    unsigned char *buf = (unsigned char *)dest;
    for (size_t i = 0; i < nbytes; i++)
        buf[i] = (unsigned char)data;
    return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *dst = (unsigned char *)dest;
    const unsigned char *srcc = (const unsigned char *)src;
    if (dst < srcc)
    {
        for (size_t i = 0; i < n; i++)
            dst[i] = srcc[i];
    }
    else
    {
        for (size_t i = n; i != 0; i--)
            dst[i - 1] = srcc[i - 1];
    }
    return dest;
}

double pow(double x, int y)
{
    double temp;
    if (y == 0)
        return 1;
    temp = pow(x, y / 2);
    if ((y % 2) == 0)
    {
        return temp * temp;
    }
    else
    {
        if (y > 0)
            return x * temp * temp;
        else
            return (temp * temp) / x;
    }
}

#include <stb/stb_image.h>
// #include <stb/stb_image_resize.h>

/**
 * Parse PNG format into pixels. Returns NULL or error, otherwise the returned data looks like
 *   ret[0] = width of the image
 *   ret[1] = height of the image
 *   ret[2..] = 32 bit ARGB pixels (blue channel in the least significant byte, alpha channel in the most)
 */
unsigned int *jpg_parse(unsigned char *ptr, int size)
{
    int i, w, h, f;
    unsigned char *img, *p;
    stbi__context s;
    stbi__result_info ri;
    s.read_from_callbacks = 0;
    s.img_buffer = s.img_buffer_original = ptr;
    s.img_buffer_end = s.img_buffer_original_end = ptr + size;
    ri.bits_per_channel = 8;
    img = p = (unsigned char *)stbi__jpeg_load(&s, (int *)&w, (int *)&h, (int *)&f, 1, &ri);
    unsigned int *data = (unsigned int *)malloc((w * h + 2) * sizeof(unsigned int));
    if (!data)
    {
        free(img);
        return NULL;
    }
    // convert the returned image into frame buffer format
    for (i = 0; i < w * h; i++, p += f)
        switch (f)
        {
        case 1:
            data[2 + i] = 0xFF000000 | (p[0] << 16) | (p[0] << 8) | p[0];
            break;
        case 2:
            data[2 + i] = (p[1] << 24) | (p[0] << 16) | (p[0] << 8) | p[0];
            break;
        case 3:
            data[2 + i] = 0xFF000000 | (p[0] << 16) | (p[1] << 8) | p[2];
            break;
        case 4:
            data[2 + i] = (p[3] << 24) | (p[0] << 16) | (p[1] << 8) | p[2];
            break;
        }
    free(img);
    data[0] = w;
    data[1] = h;
    return data;
}

unsigned int *tga_parse(unsigned char *ptr, int size)
{
    unsigned int *data;
    int i, j, k, x, y, w = (ptr[13] << 8) + ptr[12], h = (ptr[15] << 8) + ptr[14], o = (ptr[11] << 8) + ptr[10];
    int m = ((ptr[1] ? (ptr[7] >> 3) * ptr[5] : 0) + 18);

    if (w < 1 || h < 1)
        return NULL;

    data = (unsigned int *)malloc((w * h + 2) * sizeof(unsigned int));
    if (!data)
        return NULL;

    switch (ptr[2])
    {
    case 1:
        if (ptr[6] != 0 || ptr[4] != 0 || ptr[3] != 0 || (ptr[7] != 24 && ptr[7] != 32))
        {
            free(data);
            return NULL;
        }
        for (y = i = 0; y < h; y++)
        {
            k = ((!o ? h - y - 1 : y) * w);
            for (x = 0; x < w; x++)
            {
                j = ptr[m + k++] * (ptr[7] >> 3) + 18;
                data[2 + i++] = ((ptr[7] == 32 ? ptr[j + 3] : 0xFF) << 24) | (ptr[j + 2] << 16) | (ptr[j + 1] << 8) | ptr[j];
            }
        }
        break;
    case 2:
        if (ptr[5] != 0 || ptr[6] != 0 || ptr[1] != 0 || (ptr[16] != 24 && ptr[16] != 32))
        {
            free(data);
            return NULL;
        }
        for (y = i = 0; y < h; y++)
        {
            j = ((!o ? h - y - 1 : y) * w * (ptr[16] >> 3));
            for (x = 0; x < w; x++)
            {
                data[2 + i++] = ((ptr[16] == 32 ? ptr[j + 3] : 0xFF) << 24) | (ptr[j + 2] << 16) | (ptr[j + 1] << 8) | ptr[j];
                j += ptr[16] >> 3;
            }
        }
        break;
    case 9:
        if (ptr[6] != 0 || ptr[4] != 0 || ptr[3] != 0 || (ptr[7] != 24 && ptr[7] != 32))
        {
            free(data);
            return NULL;
        }
        y = i = 0;
        for (x = 0; x < w * h && m < size;)
        {
            k = ptr[m++];
            if (k > 127)
            {
                k -= 127;
                x += k;
                j = ptr[m++] * (ptr[7] >> 3) + 18;
                while (k--)
                {
                    if (!(i % w))
                    {
                        i = ((!o ? h - y - 1 : y) * w);
                        y++;
                    }
                    data[2 + i++] = ((ptr[7] == 32 ? ptr[j + 3] : 0xFF) << 24) | (ptr[j + 2] << 16) | (ptr[j + 1] << 8) | ptr[j];
                }
            }
            else
            {
                k++;
                x += k;
                while (k--)
                {
                    j = ptr[m++] * (ptr[7] >> 3) + 18;
                    if (!(i % w))
                    {
                        i = ((!o ? h - y - 1 : y) * w);
                        y++;
                    }
                    data[2 + i++] = ((ptr[7] == 32 ? ptr[j + 3] : 0xFF) << 24) | (ptr[j + 2] << 16) | (ptr[j + 1] << 8) | ptr[j];
                }
            }
        }
        break;
    case 10:
        if (ptr[5] != 0 || ptr[6] != 0 || ptr[1] != 0 || (ptr[16] != 24 && ptr[16] != 32))
        {
            free(data);
            return NULL;
        }
        y = i = 0;
        for (x = 0; x < w * h && m < size;)
        {
            k = ptr[m++];
            if (k > 127)
            {
                k -= 127;
                x += k;
                while (k--)
                {
                    if (!(i % w))
                    {
                        i = ((!o ? h - y - 1 : y) * w);
                        y++;
                    }
                    data[2 + i++] = ((ptr[16] == 32 ? ptr[m + 3] : 0xFF) << 24) | (ptr[m + 2] << 16) | (ptr[m + 1] << 8) | ptr[m];
                }
                m += ptr[16] >> 3;
            }
            else
            {
                k++;
                x += k;
                while (k--)
                {
                    if (!(i % w))
                    {
                        i = ((!o ? h - y - 1 : y) * w);
                        y++;
                    }
                    data[2 + i++] = ((ptr[16] == 32 ? ptr[m + 3] : 0xFF) << 24) | (ptr[m + 2] << 16) | (ptr[m + 1] << 8) | ptr[m];
                    m += ptr[16] >> 3;
                }
            }
        }
        break;
    default:
        free(data);
        return NULL;
    }
    data[0] = w;
    data[1] = h;
    return data;
}

extern "C" long syscall_dbg(int arg1, char *arg2);
extern "C" long syscall_FileOpen(char *arg1);
extern "C" long syscall_FileClose(void *arg1);
extern "C" long syscall_FileRead(void *arg1, uint64_t arg2, void *arg3, uint64_t arg4);
extern "C" long syscall_FileSize(void *arg1);
extern "C" long syscall_displayAddress();
extern "C" long syscall_displayWidth();
extern "C" long syscall_displayHeight();
extern "C" long syscall_displayPixelsPerScanLine();
extern "C" long syscall_requestPages(uint64_t arg1);

long unsigned strlen(char s[])
{
    long unsigned i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

void LoadFont(const char *filename)
{
    syscall_dbg(0x3F8, (char *)"[LoadFont] Loading the font... ");
    syscall_dbg(0x3F8, (char *)filename);
    syscall_dbg(0x3F8, (char *)"\n");
    void *stuff = (void *)syscall_FileOpen((char *)filename);
    if (stuff == nullptr)
    {
        syscall_dbg(0x3F8, (char *)"[LoadFont] Error! Could not open file.\n");
        return;
    }
    syscall_dbg(0x3F8, (char *)"[LoadFont] File opened.\n");
    uint64_t stuffsize = syscall_FileSize(stuff);
    void *out = malloc(stuffsize);
    syscall_FileRead(stuff, 0, out, stuffsize);

    PSF2_FONT *PSF2Font = (PSF2_FONT *)malloc(sizeof(PSF2_FONT));
    uint16_t glyph2 = 0;
    PSF2_HEADER *font2 = (PSF2_HEADER *)out;
    if (font2->magic[0] != PSF2_MAGIC0 || font2->magic[1] != PSF2_MAGIC1 || font2->magic[2] != PSF2_MAGIC2 || font2->magic[3] != PSF2_MAGIC3)
    {
        syscall_dbg(0x3F8, (char *)"[LoadFont] Invalid PSF2 file.\n");
        return;
    }

    PSF2Font->Header = font2;
    PSF2Font->GlyphBuffer = (void *)((uint64_t)out + sizeof(PSF2_HEADER));
    syscall_dbg(0x3F8, (char *)"[LoadFont] Font loaded.\n");
    uint64_t address = syscall_displayAddress();
    uint64_t width = syscall_displayWidth();
    uint64_t height = syscall_displayHeight();
    uint64_t ppsl = syscall_displayPixelsPerScanLine();
    for (int VerticalScanline = 0; VerticalScanline < height; VerticalScanline++)
    {
        uint64_t PixelPtrBase = address + ((ppsl * 4) * VerticalScanline);
        for (uint32_t *PixelPtr = (uint32_t *)PixelPtrBase; PixelPtr < (uint32_t *)(PixelPtrBase + (ppsl * 4)); PixelPtr++)
            *PixelPtr = 0xFF000000;
    }
    char *Cchar = (char *)filename;

    uint64_t Xi = 0, Yi = 0;

    for (int i = 0; i < strlen(Cchar); i++)
    {
        char Char = Cchar[i];
        int bytesperline = (PSF2Font->Header->width + 7) / 8;
        uint32_t *PixelPtr = (uint32_t *)address;
        char *FontPtr = (char *)out + PSF2Font->Header->headersize + (Char > 0 && (unsigned char)Char < PSF2Font->Header->length ? Char : 0) * PSF2Font->Header->charsize;
        for (unsigned long Y = Yi; Y < Yi + PSF2Font->Header->height; Y++)
        {
            for (unsigned long X = Xi; X < Xi + PSF2Font->Header->width; X++)
                if ((*FontPtr & (0b10000000 >> (X - Xi))) > 0)
                    *(uint32_t *)(PixelPtr + X + (Y * ppsl)) = 0xFFFFFF;
            FontPtr += bytesperline;
        }
        Xi += PSF2Font->Header->width;
    }
    syscall_FileClose(stuff);
    free(out);
    free(PSF2Font);

    syscall_dbg(0x3F8, (char *)"Hello. i am trying to put an image on your screen.\n");

    void *imgg = (void *)syscall_FileOpen("/system/wallpapers/fennec.tga");
    if (imgg == nullptr)
    {
        syscall_dbg(0x3F8, (char *)"[LoadImage] Could not open file.\n");
        return;
    }
    syscall_dbg(0x3F8, (char *)"[LoadImage] Image opened.\n");

    uint64_t sizeeeee = syscall_FileSize(imgg);

    void *imgdata = malloc(sizeeeee);
    syscall_dbg(0x3F8, (char *)"[LoadImage] IMAGE GOT SIZE.\n");
    syscall_FileRead(imgg, 0, imgdata, sizeeeee);
    syscall_dbg(0x3F8, (char *)"[LoadImage] Image Readed\n");
    // unsigned int *image = jpg_parse((uint8_t *)imgdata, sizeeeee);
    unsigned int *image = tga_parse((uint8_t *)imgdata, sizeeeee);
    syscall_dbg(0x3F8, (char *)"[LoadImage] Image Parsed\n");
    if (image == nullptr)
        syscall_dbg(0x3F8, (char *)"failed to parse image file!\n");

    uint32_t x = Xi, y = 0;
    for (size_t i = 0; i < image[0] * image[1]; i++)
    {
        if (x <= width || y <= height)
            ((uint32_t *)address)[x + (y * ppsl)] = image[i];
        y++;
        if (y == image[1])
        {
            x++;
            y = 0;
        }
    }
    syscall_FileClose(imgg);
    free(image);
}
