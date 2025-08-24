/**
 * gnirt
 * An educational clone of GNU strings with no includes
 */


#ifdef __APPLE__
    #ifdef __aarch64__
        int macos_arm64_syscall(long number, long arg1, long arg2, long arg3){
            int result;
            
            asm volatile (
                "mov x16, %1\n"     // (see https://github.com/opensource-apple/xnu/blob/master/bsd/kern/syscalls.master for all others syscalls index)
                "mov x0, %2\n"
                "mov x1, %3\n"
                "mov x2, %4\n"
                "svc #0\n"
                "mov %w0, w0\n" 
                : "=r" (result)
                : "r" (number), "r" (arg1), "r" (arg2), "r" (arg3)
                : "x0", "x1", "x2", "x16"
            );
            
            return result;
        }
    #endif
#elif __linux__
    #ifdef __x86_64__
        int linux_x64_syscall(long number, long arg1, long arg2, long arg3){
            long result;

            asm volatile (
                "movq %1, %%rax\n"     // https://filippo.io/linux-syscall-table/
                "movq  %2, %%rdi\n"
                "movq  %3, %%rsi\n"
                "movq  %4, %%rdx\n"
                "syscall\n"
                "movq %%rax, %0\n"
                : "=r" (result)
                : "r" (number), "r" (arg1), "r" (arg2), "r" (arg3)
                : "rax", "rdi", "rsi", "rdx"
            );
            return (int)result;
        }
    #endif
#endif

int open(const char *path, int flags, int mode){
    #ifdef __APPLE__
        #ifdef __aarch64__
            return macos_arm64_syscall(5, (long)path, flags, mode);
        #endif
    #elif __linux__
        #ifdef __x86_64__
            return linux_x64_syscall(2, (long)path, flags, mode); 
        #endif
    #endif
}

int read(int file_descriptor, void *buffer, int bytes){
    #ifdef __APPLE__
        #ifdef __aarch64__
            return macos_arm64_syscall(3, file_descriptor, (long)buffer, bytes);
        #endif
    #elif __linux__
        #ifdef __x86_64__
            return linux_x64_syscall(0, file_descriptor, (long)buffer, bytes); 
        #endif
    #endif
}

int write(int file_descriptor, void *buffer, int bytes){
    #ifdef __APPLE__
        #ifdef __aarch64__
            return macos_arm64_syscall(4, file_descriptor, (long)buffer, bytes);
        #endif
    #elif __linux__
        #ifdef __x86_64__
            return linux_x64_syscall(1, file_descriptor, (long)buffer, bytes); 
        #endif
    #endif
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
    int minimum_readable_length = 5;
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