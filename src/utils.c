#include "utils.h"

unsigned char IsWS(char ch) {
    return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' || ch == '\f');
}

unsigned char IsNum(char ch) {
    return (ch >= '0' && ch <= '9');
}

unsigned char IsAlpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}