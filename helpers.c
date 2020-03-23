#include "helpers.h"
#include <math.h>
#include <string.h>

// Average the R, G, B to make grayscale then apply that single
// value to the R, G, B to make it grayscale
RGBTRIPLE calculateGrayScalePixel(RGBTRIPLE pixel)
{
    BYTE r = pixel.rgbtRed;
    BYTE g = pixel.rgbtGreen;
    BYTE b = pixel.rgbtBlue;

    BYTE avg = (BYTE)((int) round((r + g + b) / 3.0));

    RGBTRIPLE result;
    result.rgbtRed = avg;
    result.rgbtGreen = avg;
    result.rgbtBlue = avg;

    return result;
}

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            RGBTRIPLE pixel = image[y][x];
            RGBTRIPLE grayscalePixel = calculateGrayScalePixel(pixel);
            image[y][x] = grayscalePixel;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width / 2; x++)
        {
            // get a pixel from each side of the line, and swap them
            RGBTRIPLE leftPixel = image[y][x];
            RGBTRIPLE rightPixel = image[y][width - 1 - x];

            image[y][x] = rightPixel;
            image[y][width - 1 - x] = leftPixel;
        }
    }
    return;
}

// Average 4 byte values
BYTE avg4(BYTE b1, BYTE b2, BYTE b3, BYTE b4)
{
    return (BYTE)round(((int)b1 + b2 + b3 + b4) / 4.0);
}

// Average 6 byte values
BYTE avg6(BYTE b1, BYTE b2, BYTE b3, BYTE b4, BYTE b5, BYTE b6)
{
    return (BYTE)round(((int)b1 + b2 + b3 + b4 + b5 + b6) / 6.0);
}

// Average 9 byte values
BYTE avg9(BYTE b1, BYTE b2, BYTE b3, BYTE b4, BYTE b5, BYTE b6, BYTE b7, BYTE b8, BYTE b9)
{
    return (BYTE)round(((int)b1 + b2 + b3 + b4 + b5 + b6 + b7 + b8 + b9) / 9.0);
}

RGBTRIPLE blurPixel4(RGBTRIPLE p1, RGBTRIPLE p2, RGBTRIPLE p3, RGBTRIPLE p4)
{
    RGBTRIPLE result;
    result.rgbtRed = avg4(p1.rgbtRed, p2.rgbtRed, p3.rgbtRed, p4.rgbtRed);
    result.rgbtGreen = avg4(p1.rgbtGreen, p2.rgbtGreen, p3.rgbtGreen, p4.rgbtGreen);
    result.rgbtBlue = avg4(p1.rgbtBlue, p2.rgbtBlue, p3.rgbtBlue, p4.rgbtBlue);
    return result;
}

RGBTRIPLE blurPixel6(RGBTRIPLE p1, RGBTRIPLE p2, RGBTRIPLE p3, RGBTRIPLE p4, RGBTRIPLE p5, RGBTRIPLE p6)
{
    RGBTRIPLE result;
    result.rgbtRed = avg6(p1.rgbtRed, p2.rgbtRed, p3.rgbtRed, p4.rgbtRed, p5.rgbtRed, p6.rgbtRed);
    result.rgbtGreen = avg6(p1.rgbtGreen, p2.rgbtGreen, p3.rgbtGreen, p4.rgbtGreen, p5.rgbtGreen, p6.rgbtGreen);
    result.rgbtBlue = avg6(p1.rgbtBlue, p2.rgbtBlue, p3.rgbtBlue, p4.rgbtBlue, p5.rgbtBlue, p6.rgbtBlue);
    return result;
}

RGBTRIPLE blurPixel9(RGBTRIPLE p1, RGBTRIPLE p2, RGBTRIPLE p3, RGBTRIPLE p4, RGBTRIPLE p5, RGBTRIPLE p6, RGBTRIPLE p7,
                     RGBTRIPLE p8, RGBTRIPLE p9)
{
    RGBTRIPLE result;
    result.rgbtRed = avg9(p1.rgbtRed, p2.rgbtRed, p3.rgbtRed, p4.rgbtRed, p5.rgbtRed, p6.rgbtRed, p7.rgbtRed,
                          p8.rgbtRed, p9.rgbtRed);
    result.rgbtGreen = avg9(p1.rgbtGreen, p2.rgbtGreen, p3.rgbtGreen, p4.rgbtGreen, p5.rgbtGreen, p6.rgbtGreen, p7.rgbtGreen,
                            p8.rgbtGreen, p9.rgbtGreen);
    result.rgbtBlue = avg9(p1.rgbtBlue, p2.rgbtBlue, p3.rgbtBlue, p4.rgbtBlue, p5.rgbtBlue, p6.rgbtBlue, p7.rgbtBlue,
                           p8.rgbtBlue, p9.rgbtBlue);
    return result;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // The blur can't easily be done "in place", so make a copy of the
    // sourc image so we can read from it and write back into image[][]
    RGBTRIPLE imageCopy[height][width];
    memcpy(imageCopy, image, sizeof(imageCopy));

    // the four corners are special cases with only 4 pixels
    image[0][0] = blurPixel4(imageCopy[0][0], imageCopy[0][1], imageCopy[1][0], imageCopy[1][1]);
    image[0][width - 1] = blurPixel4(imageCopy[0][width - 1], imageCopy[0][width - 2], imageCopy[1][width - 1],
                                     imageCopy[1][width - 2]);
    image[height - 1][0] = blurPixel4(imageCopy[height - 1][0], imageCopy[height - 1][1], imageCopy[height - 2][0],
                                      imageCopy[height - 2][1]);
    image[height - 1][width - 1] = blurPixel4(imageCopy[height - 1][width - 1], imageCopy[height - 1][width - 2],
                                   imageCopy[height - 2][width - 1], imageCopy[height - 2][width - 2]);

    // top and bottom scanline are special cases with only 6 pixels
    for (int x = 1; x < width - 1; x++)
    {
        image[0][x] = blurPixel6(imageCopy[0][x - 1], imageCopy[0][x], imageCopy[0][x + 1],
                                 imageCopy[1][x], imageCopy[1][x - 1], imageCopy[1][x + 1]);
        image[height - 1][x] = blurPixel6(imageCopy[height - 1][x - 1], imageCopy[height - 1][x], imageCopy[height - 1][x + 1],
                                          imageCopy[height - 2][x], imageCopy[height - 2][x - 1], imageCopy[height - 2][x + 1]);
    }

    for (int y = 1; y < height - 1; y++)
    {
        // left and right scanline pixels are special cases with only 6 pixels
        image[y][0] = blurPixel6(imageCopy[y][0], imageCopy[y - 1][0], imageCopy[y + 1][0], imageCopy[y][1],
                                 imageCopy[y - 1][1], imageCopy[y + 1][1]);
        image[y][width - 1] = blurPixel6(imageCopy[y][width - 1], imageCopy[y - 1][width - 1], imageCopy[y + 1][width - 1],
                                         imageCopy[y][width - 2], imageCopy[y - 1][width - 2], imageCopy[y + 1][width - 2]);

        // All the interior pixels have centre pixel plus 8 surrounding pixels
        for (int x = 1; x < width - 1; x++)
        {
            image[y][x] = blurPixel9(imageCopy[y - 1][x - 1], imageCopy[y - 1][x], imageCopy[y - 1][x + 1],
                                     imageCopy[y][x - 1], imageCopy[y][x], imageCopy[y][x + 1],
                                     imageCopy[y + 1][x - 1], imageCopy[y + 1][x], imageCopy[y + 1][x + 1]);
        }
    }
    return;
}

const int GXVALS[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
const int GYVALS[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
BYTE channelSobel(BYTE b1, BYTE b2, BYTE b3, BYTE b4, BYTE b5, BYTE b6, BYTE b7, BYTE b8, BYTE b9)
{
    // From:  https://en.wikipedia.org/wiki/Kernel_(image_processing)#Convolution
    // a b c      1 2 3
    // d e f   .  4 5 6  = (i*1) + (h*2) + (g*3) + (f*4) + (e*5) + (d*6) + (c*7) + (b*8) + (a*0)
    // g h i      7 8 9
    // but the supplied matrices are already modified to allow a1, b2, c3, etc
    int gx = GXVALS[0] * b1 + GXVALS[1] * b2 + GXVALS[2] * b3 + GXVALS[3] * b4 + GXVALS[4] * b5 + GXVALS[5] * b6 +
             GXVALS[6] * b7 + GXVALS[7] * b8 + GXVALS[8] * b9;
    int gy = GYVALS[0] * b1 + GYVALS[1] * b2 + GYVALS[2] * b3 + GYVALS[3] * b4 + GYVALS[4] * b5 + GYVALS[5] * b6 +
             GYVALS[6] * b7 + GYVALS[7] * b8 + GYVALS[8] * b9;

    int px = (int) round(sqrt(gx * gx + gy * gy));
    if (px >= 255)
    {
        return 255;
    }
    return (BYTE) px;
}

RGBTRIPLE pixelSobel(RGBTRIPLE p1, RGBTRIPLE p2, RGBTRIPLE p3, RGBTRIPLE p4, RGBTRIPLE p5, RGBTRIPLE p6, RGBTRIPLE p7,
                     RGBTRIPLE p8, RGBTRIPLE p9)
{
    BYTE r = channelSobel(p1.rgbtRed, p2.rgbtRed, p3.rgbtRed, p4.rgbtRed, p5.rgbtRed, p6.rgbtRed, p7.rgbtRed,
                          p8.rgbtRed, p9.rgbtRed);
    BYTE g = channelSobel(p1.rgbtGreen, p2.rgbtGreen, p3.rgbtGreen, p4.rgbtGreen, p5.rgbtGreen, p6.rgbtGreen, p7.rgbtGreen,
                          p8.rgbtGreen, p9.rgbtGreen);
    BYTE b = channelSobel(p1.rgbtBlue, p2.rgbtBlue, p3.rgbtBlue, p4.rgbtBlue, p5.rgbtBlue, p6.rgbtBlue, p7.rgbtBlue,
                          p8.rgbtBlue, p9.rgbtBlue);
    RGBTRIPLE result;
    result.rgbtRed = r;
    result.rgbtGreen = g;
    result.rgbtBlue = b;
    return result;
}

const RGBTRIPLE BLACKPIXEL = {0, 0, 0};
// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // The blur can't easily be done "in place", so make a copy of the
    // sourc image so we can read from it and write back into image[][]
    RGBTRIPLE imageCopy[height][width];
    memcpy(imageCopy, image, sizeof(imageCopy));

    // the four corners are special cases with only 4 pixels
    image[0][0] = pixelSobel(BLACKPIXEL, BLACKPIXEL, BLACKPIXEL,
                             BLACKPIXEL, imageCopy[0][0], imageCopy[0][1],
                             BLACKPIXEL, imageCopy[1][0], imageCopy[1][1]);
    image[0][width - 1] = pixelSobel(BLACKPIXEL, BLACKPIXEL, BLACKPIXEL,
                                     imageCopy[0][width - 2], imageCopy[0][width - 1], BLACKPIXEL,
                                     imageCopy[1][width - 2], imageCopy[1][width - 1], BLACKPIXEL);
    image[height - 1][0] = pixelSobel(BLACKPIXEL, imageCopy[height - 2][0], imageCopy[height - 2][1],
                                      BLACKPIXEL, imageCopy[height - 1][0], imageCopy[height - 1][1],
                                      BLACKPIXEL, BLACKPIXEL, BLACKPIXEL);
    image[height - 1][width - 1] = pixelSobel(imageCopy[height - 2][width - 2], imageCopy[height - 2][width - 1], BLACKPIXEL,
                                   imageCopy[height - 1][width - 2], imageCopy[height - 1][width - 1], BLACKPIXEL,
                                   BLACKPIXEL, BLACKPIXEL, BLACKPIXEL);

    // top and bottom scanline are special cases with only 6 pixels
    for (int x = 1; x < width - 1; x++)
    {
        image[0][x] = pixelSobel(BLACKPIXEL, BLACKPIXEL, BLACKPIXEL,
                                 imageCopy[0][x - 1], imageCopy[0][x], imageCopy[0][x + 1],
                                 imageCopy[1][x - 1], imageCopy[1][x], imageCopy[1][x + 1]);
        image[height - 1][x] = pixelSobel(imageCopy[height - 2][x - 1], imageCopy[height - 2][x], imageCopy[height - 2][x + 1],
                                          imageCopy[height - 1][x - 1], imageCopy[height - 1][x], imageCopy[height - 1][x + 1],
                                          BLACKPIXEL, BLACKPIXEL, BLACKPIXEL);
    }

    for (int y = 1; y < height - 1; y++)
    {
        // left and right scanline pixels are special cases with only 6 pixels
        image[y][0] = pixelSobel(BLACKPIXEL, imageCopy[y - 1][0], imageCopy[y - 1][1],
                                 BLACKPIXEL, imageCopy[y][0], imageCopy[y][1],
                                 BLACKPIXEL, imageCopy[y + 1][0], imageCopy[y + 1][1]);
        image[y][width - 1] = pixelSobel(imageCopy[y - 1][width - 2], imageCopy[y - 1][width - 1], BLACKPIXEL,
                                         imageCopy[y][width - 2], imageCopy[y][width - 1], BLACKPIXEL,
                                         imageCopy[y + 1][width - 2], imageCopy[y + 1][width - 1], BLACKPIXEL);

        // All the interior pixels have centre pixel plus 8 surrounding pixels
        for (int x = 1; x < width - 1; x++)
        {
            image[y][x] = pixelSobel(imageCopy[y - 1][x - 1], imageCopy[y - 1][x], imageCopy[y - 1][x + 1],
                                     imageCopy[y][x - 1], imageCopy[y][x], imageCopy[y][x + 1],
                                     imageCopy[y + 1][x - 1], imageCopy[y + 1][x], imageCopy[y + 1][x + 1]);
        }
    }
    return;
}
