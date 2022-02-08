#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

const int BUF_SIZE = 4096;

int main(int argc, char *argv[])
{
    char buffer[BUF_SIZE];
    FILE *fp, *fin, *fout;
    if (argc != 3) {
        fprintf(stderr, "[usage]: ./acp.out src_file dstfile\n");
        exit(-1);
    }
    // open file 1, 2
    if (!(fin = fopen(argv[1], "r")))
        perror("file open error");
    if (!(fout = fopen(argv[2], "w+")))
        perror("file open error");
	
    // make a temp file
    char tmp_fn[] = "/tmp/acp_XXXXXX";
    // the declared name's data type should be array of char, not pointer of char
    // the end of filename must be XXXXXX for mkstemp
    mkstemp(tmp_fn);
    if (!(fp = fopen(tmp_fn, "w+")))
        perror("file open error");

    //copy fin's content to temp
    int ret = 0;
    while (1) {
        ret = fread(buffer, 1, BUF_SIZE, fin);
        ret = fwrite(buffer, 1, ret, fp);
        if (feof(fin))
            break;
    }
    // flush the buffer
    fflush(fp);
    // write the fd's data retrived from fileno() (meta data is included) to disk from buffer	
    fsync(fileno(fp));

    // pause	
    printf("Process is paused. Enter any single character to continue.\n");
    char c = getchar();
	
    // copy temp to file2
    // SEEK_SET: begin of file
    fseek(fp, 0, SEEK_SET);
    while (1) {
        ret = fread(buffer, 1, BUF_SIZE, fp);
        ret = fwrite(buffer, 1, ret, fout);
        if (feof(fp))
            break;
    }
	
    //remove temp file and close file pointer
    remove(tmp_fn);
    fclose(fp);
    fclose(fin);
    fclose(fout);
    return 0;
}
