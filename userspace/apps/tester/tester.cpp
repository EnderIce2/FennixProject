#include <types.h>
#include <display.h>
#include <printf.h>

// stub code

#define msleep(x) (void)(x)
#define usleep(x) (void)(x)
#define sleep(x) (void)(x)

void SierpinskiTriangle(uint32_t n)
{
    // uint32_t dx = 0;
    // uint32_t dy = 0;
    for (uint32_t y = n - 1; y >= 0; y--)
    {
        for (uint32_t i = 0; i < y; i++)
        {
            printf(" ");
            // CurrentDisplay->SetPixel(dx, dy, 0xFF000000);dx++;
        }
        for (uint32_t x = 0; x + y < n; x++)
        {
            if (x & y)
            {
                printf(" ");
                printf(" ");
                // CurrentDisplay->SetPixel(dx, dy, 0xFF000000);dx++;
                // CurrentDisplay->SetPixel(dx, dy, 0xFF000000);dx++;
            }
            else
            {
                printf("* ");
                // CurrentDisplay->SetPixel(dx + x, dy, 0xFFFFFFFF);dx++;
                // CurrentDisplay->SetPixel(dx + x, dy, 0xFF000000);dx++;
            }
        }
        printf("\n");
        usleep(1);
        // dx = 0;
        // dy++;
    }
}

void DisplayPixelTest()
{
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF000000 + (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF000000 + (x * y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF000000 + x");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF000000 + x);
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF000000 + x");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF000000 + y);
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF000000 << (x + y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF000000 << (x + y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF000000 >> (x + y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF000000 >> (x + y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF000000 << (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF000000 << (x * y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF000000 >> (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF000000 >> (x * y));

    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFFFF0000 + (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFFFF0000 + (x * y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFFFF0000 + x");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFFFF0000 + x);
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFFFF0000 + x");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFFFF0000 + y);
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFFFF0000 << (x + y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFFFF0000 << (x + y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFFFF0000 >> (x + y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFFFF0000 >> (x + y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFFFF0000 << (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFFFF0000 << (x * y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFFFF0000 >> (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFFFF0000 >> (x * y));

    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF00FF00 + (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF00FF00 + (x * y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF00FF00 + x");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF00FF00 + x);
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF00FF00 + x");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF00FF00 + y);
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF00FF00 << (x + y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF00FF00 << (x + y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF00FF00 >> (x + y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF00FF00 >> (x + y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF00FF00 << (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF00FF00 << (x * y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF00FF00 >> (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF00FF00 >> (x * y));

    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF0000FF + (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF0000FF + (x * y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF0000FF + x");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF0000FF + x);
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF0000FF + x");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF0000FF + y);
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF0000FF << (x + y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF0000FF << (x + y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF0000FF >> (x + y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF0000FF >> (x + y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF0000FF << (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF0000FF << (x * y));
    CurrentDisplay->ResetPrintPosition();
    printf("Next: 0xFF0000FF >> (x * y)");
    sleep(1);
    for (uint32_t x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (uint32_t y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF0000FF >> (x * y));

    sleep(1);
    for (int x = 0; x < CurrentDisplay->GetFramebuffer()->Width; x++)
        for (int y = 0; y < CurrentDisplay->GetFramebuffer()->Height; y++)
            CurrentDisplay->SetPixel(x, y, 0xFF000000);

    CurrentDisplay->SetPrintLocation(CurrentDisplay->GetFramebuffer()->Width / 2 - (CurrentDisplay->CurrentFont->GetFontSize().Width * 17), CurrentDisplay->GetFramebuffer()->Height / 2);
    printf("End of the tests.");
    sleep(10);
}

int main(int argc, char *argv[])
{
    CurrentDisplay->Clear();
    SierpinskiTriangle(2);
    sleep(1);
    CurrentDisplay->Clear();
    SierpinskiTriangle(4);
    sleep(1);
    CurrentDisplay->Clear();
    SierpinskiTriangle(8);
    sleep(1);
    CurrentDisplay->Clear();
    SierpinskiTriangle(16);
    sleep(1);
    CurrentDisplay->Clear();
    SierpinskiTriangle(32);
    sleep(1);
    CurrentDisplay->Clear();
    SierpinskiTriangle(64);
    sleep(1);
    CurrentDisplay->Clear();
    DisplayPixelTest(); 
}
