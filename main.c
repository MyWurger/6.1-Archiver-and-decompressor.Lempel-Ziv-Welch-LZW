#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

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

    // Записываем последовательность "*****" в конец файла
    const char *sequence = "\n*****\n";
    fwrite(sequence, sizeof(char), strlen(sequence), *FilenameOUT);
    free(buffer);                               // освобождаем память, выделенную для буфера
    return num_bytes;
}

int Get_Folder_Size(const char* folderPath) 
{
    DIR* dir;
    struct dirent* entry;
    struct stat fileStat;
    double totalSize = 0;

    dir = opendir(folderPath);
    if (dir == NULL) {
        printf("Ошибка открытия папки: %s\n", folderPath);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        char filePath[256];
        snprintf(filePath, sizeof(filePath), "%s/%s", folderPath, entry->d_name);

        if (stat(filePath, &fileStat) == 0) {
            if (S_ISDIR(fileStat.st_mode)) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    double dirSize = Get_Folder_Size(filePath);
                    if (dirSize == -1) {
                        closedir(dir);
                        return -1;
                    }
                    totalSize += dirSize;
                }
            } else {
                totalSize += fileStat.st_size;
            }
        } else {
            printf("Ошибка получения информации о файле: %s\n", filePath);
        }
    }

    closedir(dir);

    return totalSize;
}


void changeDirectory(char* dir, const char* path)
{
    char choice[10];
    while (true) 
    {
        printf(" Хотите изменить директорию? (Y/N): ");
        scanf("%9s", choice);

        if ((strcmp(choice, "Y") == 0) || (strcmp(choice, "y") == 0)) 
        {
            printf(" Введите новый адрес директории: ");
            scanf("%99s", dir);

            // проверка на существование директории
            if(opendir(dir) != NULL)
            {
                printf(" Директория существует.\n");

                if ((access(dir, W_OK) == 0) && (strcmp(dir, "/") != 0) && (strcmp(dir, "/home")) != 0)
                {
                    printf(" Директория доступна для записи.\n");
                    break;
                }

                else 
                {
                    printf(" Директория недоступна для записи.\n\n");
                }
            }

            else 
            {
                printf(" Директория не существует.\n\n");
            }
        }

        else 
        {
            strcpy(dir, path);
            break;
        }
    }

    printf (" Директория успешно сохранена\n");
    return;
}


int find_last_occurrence(const char *str, char ch, int *last_nonzero_index) 
{
    int last_index = -1;                // Индекс последнего вхождения, изначально устанавливаем на -1

    int i;

    // Проходим по каждому символу строки, пока не достигнем конца строки
    for (i = 0; str[i] != '\0'; i++) 
    {
        // Если текущий символ равен заданному символу
        if (str[i] == ch) 
        {
           last_index = i; // Устанавливаем индекс последнего вхождения
        }
    }

    *last_nonzero_index = i;
    return last_index;
}


// создадим директорию для хранения вспомогательных файлов
void create_directory(const char* path) 
{
    if(opendir(path) == NULL)
    {
        int result = mkdir(path, 0777);
        if (result == 0) 
        {
            printf(" Директория успешно создана\n");
            return ;
        } 
        else 
        {
            printf("\n\033[35m ОШИБКА создания новой директории. Код ошибки -5.\033[0m\n\n");
            exit(0);
        }
    }
    else
    {
        printf(" Директория для загрузки временных файлов уже существует \n");
        return ;
    }
}


// функция удаления директории с файлами
void remove_directory(const char* path) 
{
    DIR *dir = opendir(path);
    struct dirent *entry;

    if (dir == NULL) 
    {
        printf("\n\033[35m ОШИБКА открытия директории.\033[0m\n\n");
        exit(0);
    }

    while ((entry = readdir(dir)) != NULL) 
    {
        char file_path[1024];
        snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name);

        struct stat statbuf;
        if (stat(file_path, &statbuf) == -1) 
        {
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
            {
                remove_directory(file_path);
            }
        } 
        
        else 
        {
            remove(file_path);
        }
    }

    closedir(dir);
    rmdir(path);
}


void print_dir(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;


    if((dp = opendir(dir)) == NULL)
    {
        printf("\n\033[35m ОШИБКА открытия существующей директории %s. Код ошибки -10.\033[0m\n\n", dir);
        return;
    }
    chdir(dir);
    while ((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);

        if(S_ISDIR(statbuf.st_mode))
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
            {
                continue;
            }

            printf(" %*s%s/\n", depth, "", entry->d_name);
            // Рекурсивынй вызов с новым доступом
            print_dir(entry->d_name, depth+4);
        }

        else
        {
            printf("%*s%s\n", depth, " ", entry->d_name);
        }
    }
    chdir("..");
    closedir(dp);
}


void scanDirectory(const char *dir, char work_directory[100], FILE **gen_arch, FILE **data_arch, FILE **way_arch)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;


    if((dp = opendir(dir)) == NULL)
    {
        printf("\n\033[35m ОШИБКА открытия существующей директории %s. Код ошибки -10.\033[0m\n\n", dir);
        return;
    }
    chdir(dir);
    while ((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);

        char way[256];
        memset(way, 0, sizeof(way)); // очистка массива
        strcpy(way, work_directory);
        strcat(way, "/");
        strcat(way, entry->d_name);

        if(S_ISDIR(statbuf.st_mode))
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0 || strcmp("sources", entry->d_name) == 0)
            {
                continue;
            }

            printf(" %s\n", way);
            int size = Get_Folder_Size(entry->d_name);
            printf(" Размер директории: %i байтов\n", size);
            printf(" Тип: ДИРЕКТОРИЯ\n\n");
            fprintf(*way_arch, "%s, %i, %s\n", entry->d_name, size, way);
            // Рекурсивынй вызов с новым доступом
            scanDirectory(entry->d_name, way, gen_arch, data_arch, way_arch);
        }

        else
        {
            printf(" %s\n", way);
            int size = Get_Size(entry->d_name, data_arch);
            printf(" Размер файла: %i байтов\n", size);
            printf(" Тип: ФАЙЛ\n\n");
            fprintf(*gen_arch, "%s, %i, %s\n", entry->d_name, size, way);
        }
    }
    chdir("..");
    closedir(dp);

    return ;
}

void transfer_content(FILE *src, FILE *dst) 
{
    char buffer[1024];
    size_t bytes_read;

    // Читаем содержимое и записываем в целевой файл
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src)) > 0) 
    {
        fwrite(buffer, 1, bytes_read, dst);
    }
}



void archiver()
{
    // директория для работы
    char directory[256];
    char directory_copy[256];
    char work_directory[100];
    char before_work[256];
    char etc[100] = "../";
    char copy[100];
    int last_index = -1;
    int last_nonzero_index = -1;

    memset(directory, 0, sizeof(directory)); // очистка массива
    memset(directory_copy, 0, sizeof(directory_copy)); // очистка массива
    memset(before_work, 0, sizeof(before_work)); // очистка массива
    memset(copy, 0, sizeof(copy)); // очистка массива

    // Просто работа с текстовыми путями
    // gen_arch[0][256] - информация о файлах и их размерах
    // data_arch[1][256] - данные из файлов
    // way_arch[2][256] - пути файлов в рассматриваемой директории
    // rest_arch[3][256] - итоговый файл архива
    char info_arch[4][256];

    // устанавливаем директорию для работы
    changeDirectory(directory, "/home/dmitru/Lab_Examples/Lab1");
    strcpy(directory_copy, directory);

    
    // выводим рабочую директорию
    printf("\n\033[32m Текущая рабочая директория: %s\n\033[0m", directory);

    printf("\n\t\t\t\t\t   \033[35m Работа с архивом\033[0m\n\n");

    for(int i=0; i < sizeof(info_arch) / sizeof(info_arch[0]); i++)
    {
        // очистка массива
        strcpy(info_arch[i], "");
        strcat(info_arch[i], directory);
    }

    memset(work_directory, 0, sizeof(work_directory)); // очистка массива

    // Определение индекса, с которого начинается рабочая директория и индекса последнего символа в названии.
    last_index = find_last_occurrence(directory, '/', &last_nonzero_index);
   
    if((last_index == -1) || (last_nonzero_index == -1))
    {
        printf("\n\033[35m ОШИБКА нахождения индексов. Повторите ввод. Код ошибки -4.\033[0m");
        exit(0);
    }

    printf(" Индекс последнего элемента в строке адреса: %i\n", last_nonzero_index);
    printf(" Индекс последнего вхождения элемента \\ в строку: %i\n", last_index);

    create_directory(strcat(directory, "/sources")); 
    
    int j = 1;
    // выделим название рабочей директории для архивации
    for(int i = 0; i < (last_nonzero_index - last_index) - 1; i++)
    {

        work_directory[i] = directory[last_index + j];
        j = j+1;
    }

    for(int i = 0; i <= last_index - 1; i++)
    {
        before_work[i] = directory[i];
    }

    // надбавки к файлам для 
    char files[3][16] = 
    {
        "/info_arch.txt",
        "/data_arch.txt",
        "/way_arch.txt"
    };

    strcpy(info_arch[3], "");
    strcat(info_arch[3],  before_work);

    printf(" Путь до рабочей папки: %s", before_work);
    printf("\n Рабочая папка в директории: %s\n", work_directory);
    printf("\n Путь, где создадутся новые информационные файлы и файл архива: \n");
    
    for(int i=0; i < sizeof(info_arch) / sizeof(info_arch[0]); i++)
    {
        if(i!=3)
        {
            strcat(info_arch[i], "/sources");
            strcat(info_arch[i], files[i]);
        }
        else
        {
            strcat(strcat(info_arch[i], "/"), strcat(work_directory, ".linrar"));
        }
        printf(" %i %s\n", i, info_arch[i]);
    }


    // Работа непосредственно с файлами
    // Открытие для записи
    FILE *gen_arch = fopen(info_arch[0], "wb");
    if (gen_arch == NULL) 
    {
        printf(" ОШИБКА открытия файла записи \"%s\". Код ошибки -6\n", info_arch);
        exit (0);
    }

    FILE *data_arch = fopen(info_arch[1], "wb");
    if (data_arch == NULL) 
    {
        printf(" ОШИБКА открытия файла записи \"%s\". Код ошибки -7\n", data_arch);
        exit (0);
    }

    FILE *way_arch = fopen(info_arch[2], "wb");
    if (way_arch == NULL) 
    {
        printf(" ОШИБКА открытия файла записи \"%s\". Код ошибки -8\n", way_arch);
        exit (0);
    }

    printf("\n Файлы в рабочей директории: \n");
    print_dir(directory_copy, 0);
    printf("\n\n");
    printf(" Каталоги и файлы архива: \n");
    scanDirectory(directory_copy, work_directory, &gen_arch, &data_arch, &way_arch);
    
    fclose(gen_arch);
    fclose(data_arch);
    fclose(way_arch);

    gen_arch = fopen(info_arch[0], "rb");
    if (gen_arch == NULL) 
    {
        printf(" ОШИБКА открытия файла чтения \"%s\". Код ошибки -11\n", info_arch);
        exit (0);
    }

    data_arch = fopen(info_arch[1], "rb");
    if (data_arch == NULL) 
    {
        printf(" ОШИБКА открытия файла чтения \"%s\". Код ошибки -12\n", data_arch);
        exit (0);
    }

    way_arch = fopen(info_arch[2], "rb");
    if (way_arch == NULL) 
    {
        printf(" ОШИБКА открытия файла чтения \"%s\". Код ошибки -13\n", way_arch);
        exit (0);
    }


    FILE *rest_arch = fopen(info_arch[3], "wb");
    if (rest_arch == NULL) 
    {
        printf(" ОШИБКА открытия файла записи \"%s\". Код ошибки -9\n", rest_arch);
        exit (0);
    }

    // Переносим содержимое файлов
    transfer_content(way_arch, rest_arch);
    fprintf(rest_arch, "-----\n");
    transfer_content(gen_arch, rest_arch);
    fprintf(rest_arch, "-----\n");  // Разделитель между записями
    transfer_content(data_arch, rest_arch);

    fclose(gen_arch);
    fclose(data_arch);
    fclose(way_arch);
    fclose(rest_arch);

    // удаление директории и файлов в ней
    remove_directory(strcat(directory_copy, "/sources"));

    printf("\n\t\t\t\t    \033[35m Архивация завершена успешно!\033[0m\n");
    return ; 
}

// очистка буфера ввода
void clearInputBuffer() 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    return;
}

bool check_file_access(const char* file_path) 

{
    FILE* file = fopen(file_path, "r");
    if (file == NULL) 
    {
        return false;
    }
    fclose(file);
    return true;
}

bool check_file_extension(const char* file_name) 
{
    const char* extension = ".linrar";
    size_t extension_length = strlen(extension);
    size_t file_name_length = strlen(file_name);
    if (file_name_length < extension_length) 
    {
        return false;
    }

    const char* file_extension = file_name + (file_name_length - extension_length);

    if (strcmp(file_extension, extension) == 0) 
    {
        return true;
    }
    return false;
}


void unarchiver()
{
    char file_path[256];
    printf(" Введите путь до файла-архива: ");
    scanf(" %s", file_path);

    while(true)
    {
        // проверка доступа к файлу
        if (check_file_access(file_path)) 
        {
            printf(" Доступ к файлу возможен\n");
            // проверка, что файл - архив .linrar
            if (check_file_extension(file_path)) 
            {
                printf(" Доступ к файлу возможен, и расширение в названии файла является '.linrar'\n");
                printf(" Файл может быть доступен для разархивации\n");
                break;
            } 

            else 
            {
                char choice[10];
                printf(" Доступ к файлу возможен, но расширение в названии файла не является '.librar'\n\n");
                printf(" Хотите попробовать указать путь ещё раз? (Y/N): ");
                scanf("%9s", choice);
                printf("\n");

                if((strcmp(choice, "Y") == 0) || (strcmp(choice, "y") == 0))
                {
                    printf(" **********\n");
                    printf(" Введите путь до файла-архива: ");
                    scanf(" %s", file_path);
                    continue;
                }
                else
                {
                    printf("Ошибка! Введённый Вами файл невозможно разорхивировать;\n");
                    return;
                }
            }
        }
        else 
        {
            printf(" Доступ к файлу невозможен\n");
        }
    }

        
    FILE *rest_arch = fopen(file_path, "rb");
    if (rest_arch == NULL) 
     {
        printf(" ОШИБКА открытия архива \"%s\" для разархивации. Код ошибки -9\n", rest_arch);
        exit (0);
    }

    char directory[256];
    // устанавливаем директорию для работы
    changeDirectory(directory, "/home/dmitru/Lab_Examples/Lab1_repack");
    create_directory(directory);
    
    return;

}

int main()
{
    bool continueLoop = true;
    while(continueLoop)
    {
        printf("\n\n");
        printf("                   ______   _______  _______  _______  _______  _______  _______  \n");
        printf("                  (  __  \\ (  ____ \\(  ___  )(       )(  ____ \\(  __   )(  __   ) \n");
        printf("                  | (  \\  )| (    \\/| (   ) || () () || (    \\/| (  )  || (  )  | \n");
        printf("     ____   ____  | |   ) || |      | |   | || || || || (__    | | /   || | /   |  ____   ____ \n");
        printf("    (____) (____) | |   | || |      | |   | || |(_)| ||  __)   | (/ /) || (/ /) | (____) (____)\n");
        printf("                  | |   ) || |      | |   | || |   | || (      |   / | ||   / | | \n");
        printf("                  | (__/  )| (____/\\| (___) || )   ( || (____/\\|  (__) ||  (__) | \n");
        printf("                  (______/ (_______/(_______)|/     \\|(_______/(_______)(_______) \n");
        
        
        printf("\n\n\t\t        \033[35m Добро пожаловать в программу для работы с архивами!\033[0m\n\n\n");
        printf("\033[36m Выберите действие. Введите необходимую букву:\033[0m\n\n");
        printf("      \033[37m a - Архивировать;\033[0m    \033[34m b - Разархивировать;\033[0m    \033[31m c - Завершить выполнение программы;\033[0m\n\n");
        printf(" Ввод: ");
        char choice[6];
        scanf("%5s", choice);
        clearInputBuffer();
        printf(" Вы ввели: %5s", choice);
        printf("\n");
        
        if(strlen(choice) == 1)
        {
            switch(choice[0])
            {
                case 'a':
                case 'A':
                    archiver();
                    clearInputBuffer();
                    getchar();
                    system ("clear");
                break;
 
                case 'b':
                case 'B':
                    unarchiver();
                    clearInputBuffer();
                    getchar();
                    system ("clear");
                break;

                case 'c':
                case 'C':
                    continueLoop = false;
                break;

                default:
                    printf("\n\033[35m ОШИБКА выбора действия. Повторите ввод. Код ошибки -3.\033[0m");
                    fflush(NULL); // Выталкивание содержимого буфера вывода на экран
                    clearInputBuffer();
                    system ("clear");
                break;
            }
        }

        else
        {
            clearInputBuffer();
            printf("\033[35m ОШИБКА выбора действия. Повторите ввод. Код ошибки -3.\033[0m");
            clearInputBuffer();
            system ("clear");
        }
    }

    printf("\n\t\t\t\t   \033[35m Завершение работы программы!\033[0m\n\n");
    printf("⠀\t\t\t\t       ⠀⠀⠀⠀⠀⢀⣠⣤⠤⠤⢤⣤⣄⣀⡀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀\t\t\t\t       ⠀⠀⣠⣴⣾⡿⠛⠁⠀⠀⠀⠈⠛⠛⠿⣷⣄⡀⠀⠀⠀⠀\n");
    printf("\t\t\t\t       ⠀⢀⣴⠟⠋⢁⢤⠴⠚⠉⠉⠉⠀⠀⠉⢲⡄⠈⠳⣄⠀⠀\n");
    printf("\t\t\t\t       ⠠⣾⠟⠁⠀⡾⠁⠀⠀⠀⠀⠀⠀⢠⡀⠈⠙⣆⢀⢸⡆⠀\n");
    printf("\t\t\t\t       ⣸⠇⠀⠀⠀⢿⣇⠀⠀⢸⡏⠦⢤⡞⢴⣄⠀⠈⣾⠇⠀⠀\n");
    printf("\t\t\t\t       ⣿⠀⠠⡴⠋⠈⠛⠻⠿⠶⢿⣇⣼⠾⡿⠟⠻⠛⠁⠀⠀⠀\n");
    printf("\t\t\t\t       ⠸⣦⣼⠀⠀⠀⠀⠀⠤⠄⢸⣿⠁⠀⠀⠀⠀⠀⢀⣠⠇⠀\n");
    printf("\t\t\t\t⠀       ⠉⠻⢿⡀⠀⠀⠀⢀⡠⢊⠸⠇⠀⠀⠀⡀⠄⣠⠞⠀⠀\n");
    printf("\t\t\t\t⠀⠀       ⠀⠘⡆⠀⠄⢸⣾⣋⡁⢀⡀⠀⠀⢠⡿⠁⠀⠀⠀⠀\n");
    printf("\t\t\t\t       ⠀⠀⠀⠀⠈⠳⢄⡉⠁⠸⡇⠀⠈⠳⡄⢸⠇⠀⠀⠀⠀⠀\n");
    printf("\t\t\t\t⠀⠀⠀⠀⠀       ⠀⠀⠈⠛⠀⠀⠀⠀⠈⠛⠁⠀⠀⠀⠀⠀⠀\n\n\n");


    return 0;
}