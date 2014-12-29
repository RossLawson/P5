/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n-factor, infile, outfile\n");
        return 1;
    }

    // convert n to int
    int n = atoi(argv[1]);    
    
    // error checking - n
    
    if ((n < 1) || (n > 100))
    {
        printf("Usage: resize (n) factor must be > 0 and < 100.\n");
        return 10;
    }
    
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER in_bf;
    fread(&in_bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER in_bi;
    fread(&in_bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (in_bf.bfType != 0x4d42 || in_bf.bfOffBits != 54 || in_bi.biSize != 40 || 
        in_bi.biBitCount != 24 || in_bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // copy infile header
    BITMAPFILEHEADER out_bf = in_bf;
    BITMAPINFOHEADER out_bi = in_bi;
    
    // then scale attriubtes
    out_bi.biWidth *= n;
    out_bi.biHeight *= n;
    
    // determine padding for scanlines
    int in_padding =  (4 - (in_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int out_padding = (4 - (out_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    out_bi.biSizeImage = (sizeof(RGBTRIPLE) * abs(out_bi.biHeight) * out_bi.biWidth) + (out_bi.biWidth * out_padding);
    out_bf.bfSize = out_bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&out_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&out_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    int offset = in_bi.biWidth * sizeof(RGBTRIPLE) + in_padding;

    // iterate over infile's scanlines
    
    for (int i = 0, biHeight = abs(in_bi.biHeight); i < biHeight; i++)
    {
        for (int vcount = 0; vcount < n; vcount++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < in_bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                
                for (int hcount = 0; hcount < n; hcount++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
        
        
        // skip over padding, if any
        fseek(inptr, in_padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < out_padding; k++)
        {
            fputc(0x00, outptr);
        }
        
        fseek(inptr, -offset, SEEK_CUR);
        }
    
    fseek(inptr, offset, SEEK_CUR);
    }
    
    // close infile
    fclose(inptr);
    
    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
