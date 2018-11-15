# Resizing
// resizes a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: resize n infile outfile\n");
        return 1;
    }

    // remember filenames
    int n = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf, writting_bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    writting_bf =bf;

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi, writting_bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
     writting_bi =bi;

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    writting_bi.biHeight = bi.biHeight * n;
    writting_bi.biWidth =bi.biWidth * n;

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int Writting_padding = (4 - (writting_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    writting_bi.biSizeImage = ((sizeof(RGBTRIPLE) * writting_bi.biWidth) + Writting_padding) * abs (writting_bi.biHeight);
    writting_bf.bfSize = writting_bi.biSizeImage + sizeof(BITMAPFILEHEADER) +  sizeof(BITMAPINFOHEADER);
      // write outfile's BITMAPFILEHEADER
    fwrite(&writting_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&writting_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {

       for (int p = 0; p < n ; p++)
       {
        // iterate over pixels in scanline
         for (int j = 0; j < bi.biWidth; j++)
          {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            for (int b = 0; b < n ; b ++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1 , outptr);
                }
          }
            for (int k = 0; k < Writting_padding; k++)
                {
                    fputc(0x00, outptr);
                }
            if ( p < n-1)
                    fseek (inptr, - bi.biWidth * sizeof(RGBTRIPLE), SEEK_CUR);


       }

         fseek(inptr, padding, SEEK_CUR);
    }

    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
