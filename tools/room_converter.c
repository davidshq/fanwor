/**
 * Fanwor Room File Converter
 * 
 * Converts Fanwor room files between binary and text formats
 * Based on the level.h and fwdisk.c code from the original game
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>

/* Structures from the original game */
typedef struct {
    unsigned char entry_type;
    unsigned char art;
    unsigned char xpos;
    unsigned char ypos;
    unsigned char xl;
    unsigned char yl;
    unsigned char spec1;
    unsigned char spec2;
} LEVEL_ENTRY;

/* Header for level files */
typedef struct {
    uint32_t hmagic;        /* magic constant, must be 'FAWO' */
    uint16_t version;       /* For checking the version - 0x0123 for version 1.23 */
    char title[32];         /* name of the level */
    uint16_t obj_count;     /* number of objects (LEVEL_ENTRYs) in this room */
    uint16_t r_wdth, r_hght; /* Width and height of this room */
} LEVEL_HEADER;

/* Entry type descriptions for better readability */
const char* get_entry_type_desc(unsigned char type) {
    switch(type) {
        case 1: return "Enemy";
        case 2: return "Door";
        case 3: return "People";
        case 4: return "Item";
        default: return "Unknown";
    }
}

/* Magic number constant */
#define FANWOR_MAGIC 0x4641574FL  /* 'FAWO' */

/* Swap byte order for cross-platform compatibility */
static uint16_t swap_u16(uint16_t in) {
    uint16_t out;
    out = in << 8;
    out |= in >> 8;
    return out;
}

static uint32_t swap_u32(uint32_t in) {
    uint32_t out;
    uint16_t o1, o2;
    o1 = (uint16_t)(in>>16);
    o2 = (uint16_t)in;
    o1 = swap_u16(o1);
    o2 = swap_u16(o2);
    out = ((uint32_t)o2) << 16;
    out |= o1;
    return out;
}

/* Check if machine is little endian */
bool is_little_endian() {
    uint16_t x = 1;
    return *((uint8_t*)&x);
}

/* Convert a binary room file to a text file */
int binary_to_text(const char* input_file, const char* output_file) {
    FILE *in_file, *out_file;
    LEVEL_HEADER header;
    LEVEL_ENTRY entry;
    unsigned char *room_data, *ffield_data;
    bool need_swap = false;
    int i, x, y;
    
    in_file = fopen(input_file, "rb");
    if (!in_file) {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_file);
        return 1;
    }
    
    out_file = fopen(output_file, "w");
    if (!out_file) {
        fprintf(stderr, "Error: Cannot open output file %s\n", output_file);
        fclose(in_file);
        return 1;
    }
    
    /* Read the header */
    if (fread(&header, sizeof(LEVEL_HEADER), 1, in_file) != 1) {
        fprintf(stderr, "Error: Cannot read header from %s\n", input_file);
        fclose(in_file);
        fclose(out_file);
        return 1;
    }
    
    /* Check if we need to swap byte order */
    need_swap = (header.hmagic != FANWOR_MAGIC);
    if (need_swap) {
        printf("Input file requires byte swapping\n");
        header.hmagic = swap_u32(header.hmagic);
        header.version = swap_u16(header.version);
        header.obj_count = swap_u16(header.obj_count);
        header.r_wdth = swap_u16(header.r_wdth);
        header.r_hght = swap_u16(header.r_hght);
    }
    
    /* Check if this is a valid Fanwor level file */
    if (header.hmagic != FANWOR_MAGIC) {
        fprintf(stderr, "Error: %s is not a valid Fanwor level file (magic: %lx)\n", 
                input_file, (unsigned long)header.hmagic);
        fclose(in_file);
        fclose(out_file);
        return 1;
    }
    
    /* Write header information to text file */
    fprintf(out_file, "# Fanwor Room File: %s\n", input_file);
    fprintf(out_file, "version: 0x%04x\n", header.version);
    fprintf(out_file, "title: %s\n", header.title);
    fprintf(out_file, "width: %d\n", header.r_wdth);
    fprintf(out_file, "height: %d\n", header.r_hght);
    fprintf(out_file, "object_count: %d\n", header.obj_count);
    fprintf(out_file, "endian: %s\n\n", is_little_endian() ? "little" : "big");
    
    /* Allocate memory for room data */
    room_data = malloc(header.r_wdth * header.r_hght);
    ffield_data = malloc(header.r_wdth * header.r_hght);
    
    if (!room_data || !ffield_data) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(in_file);
        fclose(out_file);
        free(room_data);
        free(ffield_data);
        return 1;
    }
    
    /* Read room data */
    if (fread(room_data, 1, header.r_wdth * header.r_hght, in_file) != header.r_wdth * header.r_hght) {
        fprintf(stderr, "Error: Cannot read room data\n");
        fclose(in_file);
        fclose(out_file);
        free(room_data);
        free(ffield_data);
        return 1;
    }
    
    /* Read field data */
    if (fread(ffield_data, 1, header.r_wdth * header.r_hght, in_file) != header.r_wdth * header.r_hght) {
        fprintf(stderr, "Error: Cannot read field data\n");
        fclose(in_file);
        fclose(out_file);
        free(room_data);
        free(ffield_data);
        return 1;
    }
    
    /* Write room data */
    fprintf(out_file, "[Room Data]\n");
    for (y = 0; y < header.r_hght; y++) {
        for (x = 0; x < header.r_wdth; x++) {
            fprintf(out_file, "%3d", room_data[x + y * header.r_wdth]);
            if (x < header.r_wdth - 1) {
                fprintf(out_file, " ");
            }
        }
        fprintf(out_file, "\n");
    }
    
    /* Write field data */
    fprintf(out_file, "\n[Field Data]\n");
    for (y = 0; y < header.r_hght; y++) {
        for (x = 0; x < header.r_wdth; x++) {
            fprintf(out_file, "%3d", ffield_data[x + y * header.r_wdth]);
            if (x < header.r_wdth - 1) {
                fprintf(out_file, " ");
            }
        }
        fprintf(out_file, "\n");
    }
    
    /* Write object data */
    fprintf(out_file, "\n[Objects]\n");
    fprintf(out_file, "# type, art, xpos, ypos, xl, yl, spec1, spec2, description\n");
    
    for (i = 0; i < header.obj_count; i++) {
        if (fread(&entry, sizeof(LEVEL_ENTRY), 1, in_file) != 1) {
            fprintf(stderr, "Error: Cannot read object entry %d\n", i);
            fclose(in_file);
            fclose(out_file);
            free(room_data);
            free(ffield_data);
            return 1;
        }
        
        fprintf(out_file, "%d, %d, %d, %d, %d, %d, %d, %d  # %s\n",
                entry.entry_type, entry.art, entry.xpos, entry.ypos, 
                entry.xl, entry.yl, entry.spec1, entry.spec2,
                get_entry_type_desc(entry.entry_type));
    }
    
    /* Clean up */
    fclose(in_file);
    fclose(out_file);
    free(room_data);
    free(ffield_data);
    
    printf("Successfully converted %s to %s\n", input_file, output_file);
    return 0;
}

/* Create binary file by copying the original and then modifying it */
int text_to_binary(const char* input_file, const char* output_file) {
    FILE *in_file, *out_file, *tmp_file;
    LEVEL_HEADER header;
    LEVEL_ENTRY entry;
    unsigned char *room_data = NULL, *ffield_data = NULL;
    char line[1024];
    char tmp_name[256];
    char *token;
    int line_count = 0;
    int x;
    int section = 0; /* 0=header, 1=room data, 2=field data, 3=objects */
    int obj_count = 0;
    char original_file[512] = {0};
    int room_rows = 0;
    int field_rows = 0;
    
    /* Create temporary file name */
    strcpy(tmp_name, "tmp_room_XXXXXX");
    int fd = mkstemp(tmp_name);
    if (fd == -1) {
        fprintf(stderr, "Error: Could not create temporary file\n");
        return 1;
    }
    close(fd);
    
    in_file = fopen(input_file, "r");
    if (!in_file) {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_file);
        return 1;
    }
    
    /* First pass: read the header info to determine dimensions */
    while (fgets(line, sizeof(line), in_file)) {
        /* Check for original file path */
        if (strncmp(line, "# Fanwor Room File:", 19) == 0) {
            char *start = line + 19;
            while (*start == ' ') start++;
            strncpy(original_file, start, sizeof(original_file) - 1);
            original_file[sizeof(original_file) - 1] = '\0';  /* Ensure null-termination */
            char *nl = strchr(original_file, '\n');
            if (nl) *nl = '\0';
            continue;
        }
        
        /* Skip other comment lines and empty lines */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        /* Check for section markers */
        if (strstr(line, "[Room Data]")) {
            break;  /* We have enough header info */
        }
        
        /* Parse header section */
        token = strtok(line, ":");
        if (!token) continue;
        
        if (strstr(token, "width")) {
            token = strtok(NULL, "\n");
            if (token) {
                header.r_wdth = atoi(token);
            }
        } else if (strstr(token, "height")) {
            token = strtok(NULL, "\n");
            if (token) {
                header.r_hght = atoi(token);
            }
        }
    }
    
    /* Check if we have original file and dimensions */
    if (original_file[0] != '\0' && access(original_file, R_OK) == 0) {
        printf("Using original file as template: %s\n", original_file);
        
        /* Copy the original file to our temporary file */
        tmp_file = fopen(tmp_name, "wb");
        if (!tmp_file) {
            fprintf(stderr, "Error: Cannot create temporary file\n");
            fclose(in_file);
            return 1;
        }
        
        FILE *orig = fopen(original_file, "rb");
        if (!orig) {
            fprintf(stderr, "Error: Cannot open original file %s\n", original_file);
            fclose(in_file);
            fclose(tmp_file);
            return 1;
        }
        
        /* Copy the file */
        char buffer[4096];
        size_t bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), orig)) > 0) {
            fwrite(buffer, 1, bytes, tmp_file);
        }
        
        fclose(orig);
        fclose(tmp_file);
        
        /* Now reopen in read-write mode */
        tmp_file = fopen(tmp_name, "r+b");
        if (!tmp_file) {
            fprintf(stderr, "Error: Cannot reopen temporary file\n");
            fclose(in_file);
            return 1;
        }
    } else {
        fprintf(stderr, "Error: Cannot find original file or dimensions are not specified\n");
        fclose(in_file);
        return 1;
    }
    
    /* Rewind the input file */
    rewind(in_file);
    
    /* Allocate memory for room data */
    room_data = malloc(header.r_wdth * header.r_hght);
    ffield_data = malloc(header.r_wdth * header.r_hght);
    
    if (!room_data || !ffield_data) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(in_file);
        fclose(tmp_file);
        free(room_data);
        free(ffield_data);
        return 1;
    }
    
    /* Initialize data arrays */
    memset(room_data, 0, header.r_wdth * header.r_hght);
    memset(ffield_data, 0, header.r_wdth * header.r_hght);
    
    /* Parse the text file - read room data */
    printf("Parsing room data (width=%d, height=%d)\n", header.r_wdth, header.r_hght);
    
    /* Find the [Room Data] section */
    rewind(in_file);
    while (fgets(line, sizeof(line), in_file)) {
        if (strstr(line, "[Room Data]")) {
            break;
        }
    }
    
    /* Read room data */
    room_rows = 0;
    while (room_rows < header.r_hght && fgets(line, sizeof(line), in_file)) {
        /* Skip comment lines, empty lines, and section markers */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == '[') {
            continue;
        }
        
        /* Parse room data line */
        char *ptr = line;
        for (x = 0; x < header.r_wdth && *ptr; x++) {
            while (*ptr == ' ') ptr++; /* Skip spaces */
            if (*ptr) {
                room_data[x + room_rows * header.r_wdth] = atoi(ptr);
                while (*ptr && *ptr != ' ') ptr++; /* Skip to next number */
            }
        }
        room_rows++;
    }
    
    printf("Parsed %d room data rows of %d expected\n", room_rows, header.r_hght);
    
    /* Find the [Field Data] section */
    rewind(in_file);
    while (fgets(line, sizeof(line), in_file)) {
        if (strstr(line, "[Field Data]")) {
            break;
        }
    }
    
    /* Read field data */
    field_rows = 0;
    while (field_rows < header.r_hght && fgets(line, sizeof(line), in_file)) {
        /* Skip comment lines, empty lines, and section markers */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == '[') {
            continue;
        }
        
        /* Parse field data line */
        char *ptr = line;
        for (x = 0; x < header.r_wdth && *ptr; x++) {
            while (*ptr == ' ') ptr++; /* Skip spaces */
            if (*ptr) {
                ffield_data[x + field_rows * header.r_wdth] = atoi(ptr);
                while (*ptr && *ptr != ' ') ptr++; /* Skip to next number */
            }
        }
        field_rows++;
    }
    
    printf("Parsed %d field data rows of %d expected\n", field_rows, header.r_hght);
    
    /* Write room data to the temporary file */
    fseek(tmp_file, sizeof(LEVEL_HEADER), SEEK_SET);
    fwrite(room_data, 1, header.r_wdth * header.r_hght, tmp_file);
    
    /* Write field data to the temporary file */
    fwrite(ffield_data, 1, header.r_wdth * header.r_hght, tmp_file);
    
    /* Now check for objects */
    rewind(in_file);
    while (fgets(line, sizeof(line), in_file)) {
        if (strstr(line, "[Objects]")) {
            section = 3;
            break;
        }
    }
    
    /* Position tmp_file at the start of objects */
    fseek(tmp_file, sizeof(LEVEL_HEADER) + 2 * header.r_wdth * header.r_hght, SEEK_SET);
    
    /* Parse objects */
    obj_count = 0;
    while (fgets(line, sizeof(line), in_file)) {
        /* Skip comment lines and empty lines */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        /* Process object entry */
        memset(&entry, 0, sizeof(LEVEL_ENTRY));
        
        int values[8];
        int count = 0;
        char *ptr = line;
        
        while (count < 8 && *ptr) {
            while (*ptr == ' ' || *ptr == '\t') ptr++; /* Skip whitespace */
            
            if (*ptr == '#') break; /* Comment */
            
            if (isdigit(*ptr) || *ptr == '-') {
                values[count++] = atoi(ptr);
                
                /* Skip to next value */
                while (*ptr && *ptr != ',' && *ptr != '#') ptr++;
                if (*ptr == ',') ptr++;
            } else {
                ptr++;
            }
        }
        
        if (count >= 8) {
            entry.entry_type = values[0];
            entry.art = values[1];
            entry.xpos = values[2];
            entry.ypos = values[3];
            entry.xl = values[4];
            entry.yl = values[5];
            entry.spec1 = values[6];
            entry.spec2 = values[7];
            
            /* Write the entry to the temporary file */
            fwrite(&entry, sizeof(LEVEL_ENTRY), 1, tmp_file);
            
            obj_count++;
        }
    }
    
    /* Update the object count in the header */
    if (obj_count > 0) {
        /* Read the original header */
        fseek(tmp_file, 0, SEEK_SET);
        if (fread(&header, sizeof(LEVEL_HEADER), 1, tmp_file) != 1) {
            fprintf(stderr, "Error: Cannot read header from temporary file\n");
            fclose(in_file);
            fclose(tmp_file);
            free(room_data);
            free(ffield_data);
            unlink(tmp_name);
            return 1;
        }
        
        /* Update object count */
        bool need_swap = (header.hmagic != FANWOR_MAGIC);
        if (need_swap) {
            header.hmagic = swap_u32(header.hmagic);
            header.version = swap_u16(header.version);
            header.r_wdth = swap_u16(header.r_wdth);
            header.r_hght = swap_u16(header.r_hght);
            header.obj_count = swap_u16(obj_count);
            header.hmagic = swap_u32(header.hmagic);
            header.version = swap_u16(header.version);
            header.r_wdth = swap_u16(header.r_wdth);
            header.r_hght = swap_u16(header.r_hght);
        } else {
            header.obj_count = obj_count;
        }
        
        /* Write updated header */
        fseek(tmp_file, 0, SEEK_SET);
        fwrite(&header, sizeof(LEVEL_HEADER), 1, tmp_file);
    }
    
    /* Close the temporary file */
    fclose(tmp_file);
    
    /* Create the output file by copying the temporary file */
    tmp_file = fopen(tmp_name, "rb");
    if (!tmp_file) {
        fprintf(stderr, "Error: Cannot reopen temporary file\n");
        fclose(in_file);
        free(room_data);
        free(ffield_data);
        unlink(tmp_name);
        return 1;
    }
    
    out_file = fopen(output_file, "wb");
    if (!out_file) {
        fprintf(stderr, "Error: Cannot open output file %s\n", output_file);
        fclose(in_file);
        fclose(tmp_file);
        free(room_data);
        free(ffield_data);
        unlink(tmp_name);
        return 1;
    }
    
    /* Copy the file */
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), tmp_file)) > 0) {
        fwrite(buffer, 1, bytes, out_file);
    }
    
    /* Clean up */
    fclose(in_file);
    fclose(tmp_file);
    fclose(out_file);
    free(room_data);
    free(ffield_data);
    unlink(tmp_name);
    
    printf("Successfully converted %s to %s\n", input_file, output_file);
    return 0;
}

void print_usage(const char* program_name) {
    printf("Usage:\n");
    printf("  %s -d [input_dir] [output_dir]  # Convert all files in directory\n", program_name);
    printf("  %s -b [input_file] [output_file]  # Convert binary to text\n", program_name);
    printf("  %s -t [input_file] [output_file]  # Convert text to binary\n", program_name);
    printf("\nExample:\n");
    printf("  %s -d rooms/ rooms_text/  # Convert all binary files in rooms/ to text files in rooms_text/\n", program_name);
    printf("  %s -b rooms/room512 room512.txt  # Convert a single binary file to text\n", program_name);
    printf("  %s -t room512.txt rooms/room512  # Convert a single text file to binary\n", program_name);
}

/* Process all files in a directory */
int process_directory(const char* input_dir, const char* output_dir, bool to_text) {
    DIR *dir;
    struct dirent *entry;
    char input_path[512];
    char output_path[512];
    int count = 0;
    int errors = 0;
    
    /* Open the directory */
    dir = opendir(input_dir);
    if (!dir) {
        fprintf(stderr, "Error: Cannot open directory %s\n", input_dir);
        return 1;
    }
    
    /* Create the output directory if it doesn't exist */
    struct stat st = {0};
    if (stat(output_dir, &st) == -1) {
        #ifdef _WIN32
        mkdir(output_dir);
        #else
        mkdir(output_dir, 0755);
        #endif
    }
    
    /* Process each file in the directory */
    while ((entry = readdir(dir)) != NULL) {
        /* Skip . and .. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        /* Skip directories */
        sprintf(input_path, "%s/%s", input_dir, entry->d_name);
        if (stat(input_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            continue;
        }
        
        /* Create output path */
        if (to_text) {
            /* For binary to text, add .txt extension */
            sprintf(output_path, "%s/%s.txt", output_dir, entry->d_name);
            if (binary_to_text(input_path, output_path) == 0) {
                count++;
            } else {
                errors++;
            }
        } else {
            /* For text to binary, strip .txt extension if present */
            char base_name[256];
            strcpy(base_name, entry->d_name);
            char *dot = strrchr(base_name, '.');
            if (dot && strcmp(dot, ".txt") == 0) {
                *dot = '\0';
            }
            
            /* Output filename should not have .txt extension */
            sprintf(output_path, "%s/%s", output_dir, base_name);
            if (text_to_binary(input_path, output_path) == 0) {
                count++;
            } else {
                errors++;
            }
        }
    }
    
    closedir(dir);
    
    printf("Processed %d files with %d errors\n", count, errors);
    return (errors > 0) ? 1 : 0;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }
    
    /* Convert binary to text */
    if (strcmp(argv[1], "-b") == 0) {
        return binary_to_text(argv[2], argv[3]);
    }
    /* Convert text to binary */
    else if (strcmp(argv[1], "-t") == 0) {
        return text_to_binary(argv[2], argv[3]);
    }
    /* Process directory */
    else if (strcmp(argv[1], "-d") == 0) {
        return process_directory(argv[2], argv[3], true);
    }
    /* Process directory (text to binary) */
    else if (strcmp(argv[1], "-r") == 0) {
        return process_directory(argv[2], argv[3], false);
    }
    else {
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
} 