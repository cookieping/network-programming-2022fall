#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <vector>

using namespace std;
#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))

typedef struct {
    uint32_t magic;
    int32_t  off_str;
    int32_t  off_content;
    uint32_t n_files;
} __attribute((packed)) pako_header_t;

typedef struct {
    int32_t off_str;  // in the string section
    uint32_t file_size;  // * Big endian
    int32_t off_content;  // in the content section
    uint64_t checksum;  // * Big endian
} __attribute((packed)) pako_FILE_E_t;

int main(int argc, char* argv[]) {
    char* pakFile = argv[1];
    char* dst = argv[2];
    int fd = open(pakFile, O_RDWR);

    // 1. Read the header section
    pako_header_t header = {};
    lseek(fd, 0, SEEK_SET);
    read(fd, &header, 16);
    cout << "Magic number: " << hex << header.magic << endl;
    cout << "String section offset: " << hex << header.off_str << endl;
    cout << "Content section offset: " << hex << header.off_content << endl;
    cout << "Number of files: " << dec << header.n_files << endl;

    // 2_0.Read the FILE_Es section
    pako_FILE_E_t FILE_E[header.n_files];
    for(int i = 0; i < header.n_files; i++) {
        read(fd, &FILE_E[i], 20);
    }

    // 2_1. Read file name and show the filesize in bytes
    char buf[1];
    vector<string> fileName;
    for(int i = 0; i < header.n_files; i++) {
        // file name
        string strName = "";
        cout << dec << "\nFile " << i + 1 << endl;
        cout << "file name: ";
        while(read(fd, buf, 1)) {
            if(buf[0] == '\0') break;
            cout << buf[0];
            strName = strName + buf[0];
        }
        fileName.push_back(strName);
        // file size
        cout << "\nfile size in hex (bytes): " << hex << htonl(FILE_E[i].file_size);
        cout << "\nfile size in decimal (bytes): " << dec << htonl(FILE_E[i].file_size); // in decimal
        cout << endl;
    }
    cout << endl;
    
    // 3 & 4. check the checksum and unpack the file
    for(int i = 0; i < header.n_files; i++) {
        // read the content of the file
        lseek(fd, header.off_content + FILE_E[i].off_content, SEEK_SET);
        uint32_t file_size = htonl(FILE_E[i].file_size);  // real file size of the current file
        char contentBuffer[file_size + 10];
        read(fd, &contentBuffer, file_size);

        // calculate the checksum of the file
        uint64_t myChecksum = 0, segment = 0;  // 0 XOR 1 = 1, 0 XOR 0 = 0
        string str = "";
        for(int i = 0; i < file_size; i++) {
            str = str + contentBuffer[i];
            if((i % 8 == 7)) {
                for (auto j : str) {
                    segment = segment << 8 | (int)j;
                }
                myChecksum = myChecksum ^ htonll(segment);
                str = "";
                segment = 0;
            } else if ((i / 8 == (file_size - 1) / 8)) {  // length of the last segment is less than 8 bytes
                str = "";
                while(i < file_size) {
                    str = str + contentBuffer[i];
                    i++;
                }
                int cnt = 0;
                for (auto j : str) {
                    segment = segment << 8 | (int)j;
                    cnt++;
                }
                while(cnt < 8) {
                    segment = segment << 8;  // pad zero at the end of the segment
                    cnt++;
                }
                myChecksum = myChecksum ^ htonll(segment);
            }
        }

        // Compare the checksum. Unpack if the checksum is correct.
        uint64_t checksum_little = htonll(FILE_E[i].checksum);  // turn the checksum to little endian
        if(myChecksum == checksum_little) {
            cout << "File " << i + 1 << "'s checksum is correct, create new file.\n";
            // create the file
            char op[] = "/";
            char final_dst[100];
            strcpy(final_dst, dst);
            strcat(final_dst, op);
            strcat(final_dst, fileName[i].c_str());
            int fd_newFile = open(final_dst, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR);

            // write the content to the file
            write(fd_newFile, &contentBuffer, file_size);
            close(fd_newFile);
        } else {
            cout << "File " << i + 1 << "'s checksum is incorrect.\n";
        }
    }
    close(fd);
    return 0;
}
