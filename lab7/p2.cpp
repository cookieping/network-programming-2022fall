#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int main() {
    static int oseed = 0x7817c9b8;

    static int len;
	static char buf1[64];

    len  = snprintf(buf1,     10, "%x", rand());
	len += snprintf(buf1+len, 10, "%x", rand());
	len += snprintf(buf1+len, 10, "%x", rand());
	len += snprintf(buf1+len, 10, "%x", rand());
	buf1[len] = '\0';
	cout << "buf1 = " << buf1 << endl;

    rand();

    static char buf2[64];
    len  = snprintf(buf2,     10, "%x", rand());
	len += snprintf(buf2+len, 10, "%x", rand());
	len += snprintf(buf2+len, 10, "%x", rand());
	len += snprintf(buf2+len, 10, "%x", rand());
	buf2[len] = '\0';
	cout << "buf2 = " << buf2 << endl;

    return 0;
}