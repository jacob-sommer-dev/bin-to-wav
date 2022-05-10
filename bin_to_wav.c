#include "bin_to_wav.h"

int main(int argc, char *argv[])
{

    printf("cat: test\n");

    FILE *ifp;
    FILE *ofp;

    if(ofp == NULL)
    {
        printf("cat: can't open out.bin\n");
        return 1;
    }

    if(argc == 1)
    {
        filecopy(stdin, stdout);
    }
    else
    {
        while (--argc > 0)
        {
            if((ifp = fopen(*++argv, "r")) == NULL)
            {
                printf("cat: can't open %s\n", *argv);
                return 1;
            }
            else if (strpbrk(*argv, ".bin") != NULL)
            {
                // open out file, same name but .wav
                char *fname = strtok(*argv, ".bin");
                strcat(fname, ".wav");
                ofp = fopen(fname, "w");


                long len;
                // get length of file for header
                if(fseek(ifp, 0L, SEEK_END) == 0)
                {
                    len = ftell(ifp);

                    // go back to the beginning
                    rewind(ifp);

                    if(len > -1L)
                    {
                        // write WAVE header to new out file
                        genwavhdr(ofp, len);
                    }
                }

                // copy the track
                filecopy(ifp, ofp);

                // close files
                fclose(ifp);
                fclose(ofp);
            }
        }
    }
    
    return 0;
}

void filecopy(FILE *ifp, FILE *ofp)
{
    int c;
    while ((c = getc(ifp)) != EOF)
    {
        putc(c, ofp);
    }
}

void genwavhdr(FILE * of, long len)
{
    // "RIFF"
    fwrite("RIFF", __SIZEOF_INT__, 1, of);

    // wav header - 8 for 'RIFF' and this
    unsigned int ilen = ((unsigned int)len + 36); // data + total header
    fwrite(&ilen, __SIZEOF_INT__, 1, of);

    // "WAVEfmt "
    fwrite("WAVEfmt ", __SIZEOF_INT__, 2, of);

    // chunk header bytes
    unsigned int fmthdrsize = 16;
    fwrite(&fmthdrsize, __SIZEOF_INT__, 1, of);

    // compression code
    unsigned short compcode = 1; // uncompressed LPCM
    fwrite(&compcode, __SIZEOF_SHORT__, 1, of);

    // number of channels
    unsigned short numchan = 2; // stereo
    fwrite(&numchan, __SIZEOF_SHORT__, 1, of);

    // sample rate
    unsigned int samprate = 44100; // Hz
    fwrite(&samprate, __SIZEOF_INT__, 1, of);

    // bytes per sec
    unsigned int bps = 176400; // 2 (channels) * 44100 (sample rate) * 16 (bits per sample)
    fwrite(&bps, __SIZEOF_INT__, 1, of);

    // bytes per frame
    unsigned short bpf = 4; // CD sector size or "frame", 75 per sec
    fwrite(&bpf, __SIZEOF_SHORT__, 1, of);

    //bits per sample
    unsigned short bitspsamp = 16;
    fwrite(&bitspsamp, __SIZEOF_SHORT__, 1, of);

    // "data"
    fwrite("data", __SIZEOF_INT__, 1, of);

    // data length
    unsigned int dlen = (unsigned int)len; // data
    fwrite(&dlen, __SIZEOF_INT__, 1, of);

}
