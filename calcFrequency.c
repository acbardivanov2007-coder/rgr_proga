#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Структура для хранения слова и его частоты
typedef struct {
    char *word;
    int count;
} WordFreq;

// Компаратор для qsort (сортировка по убыванию частоты)
int compareWords(const void *a, const void *b) {
    WordFreq *wordA = (WordFreq *)a;
    WordFreq *wordB = (WordFreq *)b;
    return wordB->count - wordA->count;
}

int main(int argc, char *argv[]) {
    // 1. Проверка аргументов командной строки
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <входной_файл> <выходной_файл>\n", argv[0]);
        return 1;
    }

    // 2. Открытие входного файла
    FILE *inFile = fopen(argv[1], "rb");
    if (!inFile) {
        perror("Ошибка открытия входного файла");
        return 1;
    }

    // 3. Определение размера файла
    fseek(inFile, 0, SEEK_END);
    long fileSize = ftell(inFile);
    fseek(inFile, 0, SEEK_SET);

    // 4. ДИНАМИЧЕСКОЕ ВЫДЕЛЕНИЕ ПАМЯТИ под входные данные (Критерий на "хорошо")
    char *fileData = (char *)malloc(fileSize + 1);
    if (!fileData) {
        perror("Ошибка выделения памяти");
        fclose(inFile);
        return 1;
    }

    // Чтение всего файла в память и закрытие потока
    fread(fileData, 1, fileSize, inFile);
    fileData[fileSize] = '\0';
    fclose(inFile);

    // 5. Динамический массив для словаря
    int capacity = 100;
    int wordCount = 0;
    WordFreq *words = (WordFreq *)malloc(capacity * sizeof(WordFreq));

    // Набор разделителей для выделения слов (пробелы и основные знаки препинания)
    const char *delimiters = " \t\n\r.,!?:;\"'()[]{}<>-";

    // 6. Разбиение текста на слова (токенизация)
    char *token = strtok(fileData, delimiters);
    while (token != NULL) {
        int found = 0;
        
        // Поиск слова в уже сохраненном списке
        for (int i = 0; i < wordCount; i++) {
            if (strcmp(words[i].word, token) == 0) {
                words[i].count++;
                found = 1;
                break;
            }
        }

        // Если слово встретилось впервые — добавляем в массив
        if (!found) {
            // Расширяем массив, если не хватает места
            if (wordCount >= capacity) {
                capacity *= 2;
                words = (WordFreq *)realloc(words, capacity * sizeof(WordFreq));
            }
            words[wordCount].word = token; // Указываем на строку в выделенном буфере fileData
            words[wordCount].count = 1;
            wordCount++;
        }

        // Переход к следующему слову
        token = strtok(NULL, delimiters);
    }

    // 7. Сортировка массива по убыванию частоты
    qsort(words, wordCount, sizeof(WordFreq), compareWords);

    // 8. Запись результатов в выходной файл
    FILE *outFile = fopen(argv[2], "w");
    if (!outFile) {
        perror("Ошибка открытия выходного файла");
        free(words);
        free(fileData);
        return 1;
    }

    // Сохраняем только те слова, которые встречаются больше 1 раза
    for (int i = 0; i < wordCount; i++) {
        if (words[i].count > 1) {
            fprintf(outFile, "%s - %d\n", words[i].word, words[i].count);
        }
    }

    // 9. Освобождение ресурсов
    fclose(outFile);
    free(words);
    free(fileData);

    return 0;
}
