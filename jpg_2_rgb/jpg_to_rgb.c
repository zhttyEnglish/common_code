#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

#define WIDTH 320
#define HEIGHT 320

int main(int argc, char **argv) 
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride, i, j;
	
    FILE *infile = fopen(argv[1], "rb");
    if (!infile) {
        printf("Error opening file %s\n", argv[1]);
        return -1;
    }
	
    cinfo.err = jpeg_std_error(&jerr);
	
    jpeg_create_decompress(&cinfo);
	
    jpeg_stdio_src(&cinfo, infile);
	
    jpeg_read_header(&cinfo, TRUE);
	
    jpeg_start_decompress(&cinfo);
	
    unsigned char * rgb_buffer = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * 3);
    row_stride = cinfo.output_width * cinfo.output_components;
    
    while (cinfo.output_scanline < cinfo.output_height) 
	{
        row_pointer[0] = &rgb_buffer[cinfo.output_scanline * cinfo.output_width * 3];
        (void) jpeg_read_scanlines(&cinfo, row_pointer, 1);
     }
     // Output RGB data to file
     FILE *outfile = fopen(argv[2], "wb");
     fwrite(rgb_buffer, cinfo.output_width * cinfo.output_height * 3, 1, outfile);
     fclose(infile);
     fclose(outfile);
     free(rgb_buffer);
     return 0;
}
