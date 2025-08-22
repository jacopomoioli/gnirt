int open(const char *path, int flags, int mode){
    int result;

    asm volatile (
        "mov x16, #5\n"     // system call 5 = open() in macos (see https://github.com/opensource-apple/xnu/blob/master/bsd/kern/syscalls.master for all others syscalls index)
        "mov x0, %1\n"      // first mapped value (pointer to path string) in register x0. It's a pointer, so use x (64 bit register) 
        "mov w1, %w2\n"      // flag in w1 (w is for 32bit registers, w0 is the lower half of x0)
        "mov w2, %w3\n"      // mode in w2
        "svc #0\n"       // interrupt to call the kernel, same of int 0x80 in linux x86
        "mov %w0, w0\n"      // put return value (saved in x0 by the kernel) into mapped var result (%0)
        : "=r" (result)                         // output operands 
        : "r" (path), "r" (flags), "r" (mode)    // input operands
        : "x0", "w1", "w2", "x16"               // clobbered registers
    );

    // FIXME: there's something wrong with the return code if the syscall fails. I need to investigate more.

    return result;
}

int read(int file_descriptor, void *buffer, int bytes){
    int result;

    asm volatile (
        "mov x16, #3\n"
        "mov w0, %w1\n"
        "mov x1, %2\n"
        "mov w2, %w3\n"
        "svc #0\n"
        "mov %w0, w0\n" 
        : "=r" (result)
        : "r" (file_descriptor), "r" (buffer), "r" (bytes)
        : "x0", "w1", "w2", "x16"
    );

    return result;
}

int write(int file_descriptor, void *buffer, int bytes){
    int result;

    asm volatile (
        "mov x16, #4\n"
        "mov w0, %w1\n"
        "mov x1, %2\n"
        "mov w2, %w3\n"
        "svc #0\n"
        "mov %w0, w0\n" 
        : "=r" (result)
        : "r" (file_descriptor), "r" (buffer), "r" (bytes)
        : "x0", "w1", "w2", "x16"
    );

    return result;
}

int atoi(char *input){
    int iterator = 0;
    int number = 0;
    int digit = 0;
    while(input[iterator]!='\0'){
        digit = input[iterator] - 48;
        number = (number*10)+digit;
        iterator++;
    }
    return number;
}

int is_printable(char character){
    if(((int)character >= 32 && (int)character <= 126) || (int)character == 9){
        return 1;
    }else{
        return 0;
    }
}

int length(char* buffer){
    int i=0;
    while(buffer[i] != '\0'){
        i++;
    }
    return i;
}

int clean_buffer(char* buffer, int length){
    for(int i=0; i<length; i++){
        buffer[i] = 0;
    }
    return 0;
}

int print(char* buffer){
    write(0, buffer, length(buffer));
    write(0, "\n", 1);
    return 0;
}


int main(int argc, char **argv){
    int minimum_readable_length = 500;
    int string_buffer_size = 2048;
    int file_descriptor;
    int read_bytes;
    int string_length;
    char readable_string_buffer[string_buffer_size];
    int chunk_size = 4096;
    char buf[chunk_size];

    if(argc < 2){
        print("Usage: gnirt <path to file> [minimum length of printable string. default=5]");
        return 0;
    }

    if(argc == 3){
        minimum_readable_length = atoi(argv[2]);
    }
    
    file_descriptor = open(argv[1], 0, 0);
    if (file_descriptor < 0) {
        print("Error while opening the file. Quitting");
        return -1;
    }
    
    // on-the-fly file read and string recognition loop
    string_length = 0;
    read_bytes = read(file_descriptor, buf, chunk_size);
    while (read_bytes > 0){
        for(int i=0; i<read_bytes; i++){
            if (is_printable(buf[i])){
                readable_string_buffer[string_length] = buf[i];
                string_length++;
            }else{
                if (string_length > 0){
                    readable_string_buffer[string_length] = '\0';
                    if (string_length >= minimum_readable_length){
                        print(readable_string_buffer);
                    }
                    clean_buffer(readable_string_buffer, string_buffer_size);
                    string_length = 0;
                }
            }
        }
        read_bytes = read(file_descriptor, buf, chunk_size);

        // edge case, but to avoid that if the file ends with a printable string, i add a "fake" terminator at the end
        if(read_bytes == 0 && string_length > 0){   
            read_bytes = 1;
            buf[0] = '\0';
        }
    }

    return 0;
}