// #include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <stdbool.h>

void strrev(char* str)
{
    // if the string is empty
    if (!str) {
        return;
    }
    // pointer to start and end at the string
    int i = 0;
    int j = strlen(str) - 1;

    // reversing string
    while (i < j) {
        char c = str[i];
        str[i] = str[j];
        str[j] = c;
        i++;
        j--;
    }
}

#define line_len 100

struct key_value{
    char key[line_len];
    short value;
};
short Current_Avail_Index;
short Current_Avail_Memory = 16;

#define inst_len 17
#define nvars 1000
struct key_value lookup_table[nvars];

char* decode_dest (char* dest) {
    if (!strcmp(dest, "M")) return "001";
    else if (!strcmp(dest, "D")) return "010";
    else if (!strcmp(dest, "MD")) return "011";
    else if (!strcmp(dest, "A")) return "100";
    else if (!strcmp(dest, "AM")) return "101";
    else if (!strcmp(dest, "AD")) return "110";
    else return "111";
}

char* decode_op (char* operation) {
    if (!strcmp(operation, "0")) return "0101010";
    else if (!strcmp(operation, "1")) return "0111111";
    else if (!strcmp(operation, "-1")) return "0111010";
    else if (!strcmp(operation, "D")) return "0001100";
    else if (!strcmp(operation, "A")) return "0110000";
    else if (!strcmp(operation, "M")) return "1110000";
    else if (!strcmp(operation, "M")) return "1110000";
    else if (!strcmp(operation, "!D")) return "0001101";
    else if (!strcmp(operation, "!A")) return "0110001";
    else if (!strcmp(operation, "!M")) return "1110001";
    else if (!strcmp(operation, "-D")) return "0001111";
    else if (!strcmp(operation, "-A")) return "0110011";
    else if (!strcmp(operation, "-M")) return "1110011";
    else if (!strcmp(operation, "D+1")) return "0011111";
    else if (!strcmp(operation, "A+1")) return "0110111";
    else if (!strcmp(operation, "M+1")) return "1110111";
    else if (!strcmp(operation, "D-1")) return "0001110";
    else if (!strcmp(operation, "A-1")) return "0110010";
    else if (!strcmp(operation, "M-1")) return "1110010";
    else if (!strcmp(operation, "D+A")) return "0000010";
    else if (!strcmp(operation, "D+M")) return "1000010";
    else if (!strcmp(operation, "D-A")) return "0010011";
    else if (!strcmp(operation, "D-M")) return "1010011";
    else if (!strcmp(operation, "A-D")) return "0000111";
    else if (!strcmp(operation, "M-D")) return "1000111";
    else if (!strcmp(operation, "D&A")) return "0000000";
    else if (!strcmp(operation, "D&M")) return "1000000";
    else if (!strcmp(operation, "D|A")) return "0010101";
    else (!strcmp(operation, "D|M")); return "1010101";

}
char* decode_jmp (char* jump) {
    if (!strcmp(jump, "JGT")) return "001";
    else if (!strcmp(jump, "JEQ")) return "010";
    else if (!strcmp(jump, "JGE")) return "011";
    else if (!strcmp(jump, "JLT")) return "100";
    else if (!strcmp(jump, "JNE")) return "101";
    else if (!strcmp(jump, "JLE")) return "110";
    else return "111";
}

void initialize_table() {
    for (int i=0;i<10;i++) {
        char ch = (char)'0'+i;
        sprintf(lookup_table[i].key, "R%c",ch);
        lookup_table[i].value = i;
    }
    strcpy(lookup_table[10].key, "R10"); lookup_table[10].value = 10;
    strcpy(lookup_table[11].key, "R11"); lookup_table[11].value = 11;
    strcpy(lookup_table[12].key, "R12"); lookup_table[12].value = 12;
    strcpy(lookup_table[13].key, "R13"); lookup_table[13].value = 13;
    strcpy(lookup_table[14].key, "R14"); lookup_table[14].value = 14;
    strcpy(lookup_table[15].key, "R15"); lookup_table[15].value = 15;


    strcpy(lookup_table[16].key, "SCREEN"); lookup_table[16].value = 16384;
    strcpy(lookup_table[17].key, "KBD"); lookup_table[17].value = 24576;
    strcpy(lookup_table[18].key, "SP"); lookup_table[18].value = 0;
    strcpy(lookup_table[19].key, "LCL"); lookup_table[19].value = 1;
    strcpy(lookup_table[20].key, "ARG"); lookup_table[20].value = 2;
    strcpy(lookup_table[21].key, "THIS"); lookup_table[21].value = 3;
    strcpy(lookup_table[22].key, "THAT"); lookup_table[22].value = 4;
    Current_Avail_Index = 23;

}

// Returns the index if the term is found returns NULL otherwise
int look_for(char* term) {
    for (int i=0; i<nvars ;i++) {
        if (!strcmp(term, lookup_table[i].key)) {
            return i;
        }
    }
    return -1;
}

void index_symbols(FILE* fptr) {
    char line[line_len];
    char inst[17];
    short Current_Line = 0;
    while (fgets(line,line_len, fptr)) {
        char testline[line_len];
        memcpy(testline, line, line_len);
        //remove \t \r characters
        while (NULL != strchr(testline, '\t')) {
            char *ptr = strchr(testline, '\t');
            *ptr = ' ';
        }
        while (NULL != strchr(testline, '\r')) {
            char *ptr = strchr(testline, '\r');
            *ptr = ' ';
        }
        char* token = strtok(testline, " ");
        if (!strncmp(token, "\n",1)){
            continue;
        }
        else if (!strncmp(token, "//",2)) {
            // printf("that was a comment\n");
            continue;
        }
        // else if (!strncmp(token, "@", 1) && !isdigit(*(token+1))) {
        //     char* ptr = strchr(token, '\n');
        //     if (ptr) {*ptr = '\0';}

        //     Current_Line++;
        //     if (-1 != look_for(token+1)) {continue;}

        //     strcpy(lookup_table[Current_Avail_Index].key, token+1);
        //     lookup_table[Current_Avail_Index].value = Current_Avail_Memory;
        //     Current_Avail_Index++;
        //     Current_Avail_Memory++;
        // }
        else if (!strncmp(token, "(", 1)){
            char* ptr = strchr(token, ')');
            if (ptr) {*ptr = '\0';}

            if (-1 != look_for(token+1)) {continue;}

            strcpy(lookup_table[Current_Avail_Index].key, token+1);
            lookup_table[Current_Avail_Index].value = Current_Line;
            Current_Avail_Index++;
        }
        else {
            Current_Line++;
        }
    }
}

void Ainst(char* inst,char* result) {
    inst[0] = '0';
    short decimal = atoi(inst);
    if (!isdigit(*(inst+1))) {
        char* ptr = strchr(inst, '\n');
        if (ptr) {*ptr = '\0';}

        int index = look_for(inst+1);

        if (-1 == index) {
            memcpy(lookup_table[Current_Avail_Index].key, inst+1,line_len);
            lookup_table[Current_Avail_Index].value = Current_Avail_Memory;
            decimal = Current_Avail_Memory;
            Current_Avail_Index++;
            Current_Avail_Memory++;
        }
        else {
            decimal = lookup_table[index].value;
        }

    }
    char binary[inst_len];
    int i = 0;
    // converting to binary
    while (decimal) {
        binary[i++] = '0' + (decimal & 1);
        decimal >>= 1;
    }
    binary[i] = '\0';
    // adding the padding zeros
    short lenth = strlen(binary);
    while (15-lenth >= 0) {
        binary[lenth] = '0';
        lenth++;
    }
    strrev(binary);
    binary[16] = '\0';

    memcpy(result, binary, inst_len);
}

void Cinst(char* inst,char* result) {
    //destination
    char* token = "     ";
    char* dest = "000";
    char* op   = "0000000";
    char* jmp  = "000";
    if (NULL != strchr(inst, '=')){
        token = strtok(inst, "=");
        dest = decode_dest(token);
        short i = strlen(inst)+1;
        token = inst+=i;
    }
    if (NULL != strchr(inst, ';')) {
        // if ; is found this means that there is a jump
        token = strtok(inst, ";");
        op = decode_op(token);
        short i = strlen(inst)+1;
        token = inst+=i;
        jmp = decode_jmp(token);
    }
    else {
        op = decode_op(inst);
    }
    char bin_inst[17];
    for (int i = 0 ; i < 17;i++) {
        bin_inst[i] = '\0';
    }
    char filler[] = "111"; //you need 3 ones before a C Inst
    //compine all the results to make the Cinst
    strncat(bin_inst, filler,3);
    strncat(bin_inst, op,7);
    strncat(bin_inst, dest,3);
    strncat(bin_inst, jmp,3);
    memcpy(result , bin_inst, 17);
}

// TODO  create file.hack with the binary code in it DONE
// remove debug printf statments DONE
// replace the hard coded path with argv DONE
// remove non-used header files DONE
// add some comments for documentation
// implement symbols

int main(int argc,char* argv[]) {



    // open the file

    if (argc == 1) {
        printf("Please enter the path to the assembly file\n");
        return 1;
    }
    char* asm_path = argv[1];


    char bin_path[strlen(asm_path)+2];


    strcpy(bin_path, asm_path);
    char* pos = strstr(bin_path, ".asm");
    *pos = '\0';
    strcat(bin_path, ".hack");

    FILE *asm_fptr = fopen(asm_path, "r");
    FILE *bin_fptr = fopen(bin_path, "w");
    // fprintf(bin_fptr, "\0");
    // fclose(bin_fptr);
    // bin_fptr = fopen(bin_path, "w");


    initialize_table();
    index_symbols(asm_fptr);
    fclose(asm_fptr);
    asm_fptr = fopen(asm_path, "r");



    // get lines and convert them to tokens then send them to be decoded

    char line[line_len];
    char inst[17];
    while (fgets(line,line_len, asm_fptr)) {
        char testline[line_len];
        memcpy(testline, line,line_len);
        //remove \t \r characters
        while (NULL != strchr(testline, '\t')) {
            char *ptr = strchr(testline, '\t');
            *ptr = ' ';
        }
        while (NULL != strchr(testline, '\r')) {
            char *ptr = strchr(testline, '\r');
            *ptr = ' ';
        }
        char* token = strtok(testline, " ");
        if (!strncmp(token, "\n",1)){
            continue;
        }
        else if (!strncmp(token, "//",2)) {
            // printf("that was a comment\n");
            continue;
        }
        else if (!strncmp(token, "@", 1)) {
            Ainst(token,inst);
        }
        else if (!strncmp(token, "(", 1)) {continue;}
        else {
            Cinst(token,inst);
        }
        //printf("%s",line);
        // printf("Inst = %s \n",inst);
        fprintf(bin_fptr, "%s\n",inst);
    }

    fclose(asm_fptr);
    fclose(bin_fptr);
    return 0;

}
