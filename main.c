#include <stdio.h>
#include <stdlib.h>

int Get_Size (const char * filenameIN, FILE ** FilenameOUT)
{
    FILE * inFile = fopen(filenameIN, "rb");

    // ошибка открытия файла
    if (inFile == NULL)
    {
        printf ("ОШИБКА открытия файла чтения \"%s\". Код ошибки -1\n", inFile);
        return -1;
    }

    fseek(inFile, 0, SEEK_END);      // устанавливаем указатель файла в конец
    int file_size = ftell(inFile);   // получаем размер файла в байтах
    fseek(inFile, 0, SEEK_SET);      // возвращаем указатель файла в начало

    char *buffer = (char *)malloc(file_size * sizeof(char));

    // считываем данные из файла в буфер по байтам
    int num_bytes = fread(buffer, 1, file_size, inFile);
    if (num_bytes == 0 && !feof(inFile) && num_bytes!=file_size)
    {
        printf("ОШИБКА чтения файла. Код ошибки -2\n");
        return -2;
    }
    // файл отработан - закрываем его
    fclose(inFile);

    fwrite(buffer, 1, num_bytes, *FilenameOUT); // записываем данные из буфера в файл
    free(buffer);                               // освобождаем память, выделенную для буфера
    return num_bytes;
}

int main()
{




    printf("                 ______   _______  _______  _______  _______  _______  _______  \n");
    printf("                (  __  \\ (  ____ \\(  ___  )(       )(  ____ \\(  __   )(  __   ) \n");
    printf("                | (  \\  )| (    \\/| (   ) || () () || (    \\/| (  )  || (  )  | \n");
    printf("   ____   ____  | |   ) || |      | |   | || || || || (__    | | /   || | /   |  ____   ____ \n");
    printf("  (____) (____) | |   | || |      | |   | || |(_)| ||  __)   | (/ /) || (/ /) | (____) (____)\n");
    printf("                | |   ) || |      | |   | || |   | || (      |   / | ||   / | | \n");
    printf("                | (__/  )| (____/\\| (___) || )   ( || (____/\\|  (__) ||  (__) | \n");
    printf("                (______/ (_______/(_______)|/     \\|(_______/(_______)(_______) \n");








    

    const char *filenameIN = "/home/dmitru/Lab_Examples/inputFile.txt";
    const char *filenameOUTaddr = "/home/dmitru/Lab_Examples/outputFile.txt";
    FILE * filenameOUT = fopen(filenameOUTaddr, "wb");

    if (filenameOUT == NULL) 
    {
        printf("ОШИБКА открытия файла записи \"%s\". Код ошибки -3\n", filenameOUT);
    }

    int num_bytes = Get_Size(filenameIN, &filenameOUT);

    if (num_bytes < 0) 
    {
        return num_bytes;
    } 
    else 
    {
        printf("Файл успешно прочитан. Количество байтов: %d\n", num_bytes);
    }

    fclose(filenameOUT);

    return 0;
}