#include <utils.h>

const char* ReadFile(const char* path)
{
    FILE* fp = fopen(path, "rb");

    fseek(fp, 0, SEEK_END);
    size_t fs = ftell(fp);
    rewind(fp);

    char* buf = malloc(fs+1);
    if(fread(buf, sizeof(char), fs, fp) != fs)
    {
        printf("ERROR: Coudln't read file %s\n", path);
        return NULL;
    }

    buf[fs] = 0;
    fclose(fp);
    
    return buf;
}

unsigned char IsWS(char ch) {
    return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' || ch == '\f');
}

unsigned char IsNum(char ch) {
    return (ch >= '0' && ch <= '9');
}

unsigned char IsAlpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}