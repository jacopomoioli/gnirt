// Badass Libraries
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

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
    write(STDOUT_FILENO, buffer, length(buffer));
    write(STDOUT_FILENO, "\n", 1);
    return 0;
}


int main(){
    int minimum_readable_length = 10;
    int file_descriptor;
    struct stat file_status;
    long long int file_size;
    ssize_t read_bytes;
    int string_length;
    char readable_string_buffer[2048];
    
    file_descriptor = open("./gnirt", O_RDONLY);
    if (file_descriptor < 0) {
        print("Error while opening the file. Quitting\n");
        return -1;
    }
    
    if(fstat(file_descriptor, &file_status) != 0) {
        print("Error while fetching the length of the file. Quitting\n");
        return -1;
    }
    
    file_size = file_status.st_size;
    
    char buf[file_size];
    
    read_bytes = read(file_descriptor, buf, file_size);
    if(read_bytes < 0){
        print("Error while reading file. Quitting\n");
    }

    
    string_length = 0;
    for(int i=0; i<read_bytes; i++){
        if (is_printable(buf[i])){
            readable_string_buffer[string_length] = buf[i];
            string_length++;
        }else{
            if (string_length > 0){
                string_length++;
                readable_string_buffer[string_length] = '\0';
                if (string_length >= minimum_readable_length){
                    print(readable_string_buffer);
                }
                clean_buffer(readable_string_buffer, sizeof(readable_string_buffer));
                string_length = 0;
            }
        }
    }

    return 0;
}