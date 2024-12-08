#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to open the file and read it into a buffer
char *readFile(const char *filename, long *len) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *len = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc((*len + 1) * sizeof(char));  // +1 for null terminator
    if (buffer == NULL) {
        perror("Failed to allocate memory for file buffer");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, *len, file);
    buffer[*len] = '\0';  // Null-terminate the string

    fclose(file);
    return buffer;
}

// Function to count the number of words in the buffer (split by commas)
size_t countWords(char *buffer, const char *delimiter) {
    size_t word_count = 0;
    for (char *temp = buffer; *temp != '\0'; temp++) {
        if (*temp == delimiter[0]) word_count++;
    }
    word_count++;  // Account for the last word after the final delimiter
    return word_count;
}

// Function to tokenize the buffer and store the words in an array
char **tokenizeBuffer(char *buffer, const char *delimiter, size_t word_count) {
    char **words = malloc(word_count * sizeof(char *));
    if (words == NULL) {
        perror("Failed to allocate memory for words array");
        return NULL;
    }

    size_t index = 0;
    char *token = strtok(buffer, delimiter);
    while (token != NULL) {
        if (token[0] == ' ') {
            memmove(token, token + 1, strlen(token) - 1);
            token[strlen(token) - 1] = '\0';
        }

        words[index] = strdup(token);
        if (words[index] == NULL) {
            perror("Failed to allocate memory for word");
            return NULL;
        }
        index++;
        token = strtok(NULL, delimiter);
    }
    return words;
}

// Function to free the allocated memory for words
void freeWords(char **words, size_t word_count) {
    for (size_t i = 0; i < word_count; i++) {
        free(words[i]);
    }
    free(words);
}

typedef struct {
    char key[50];
    int value;
} KeyValuePair;

int get_random_number(int x, int y) {
    srand(time(NULL));
    int randomValue = x + rand() % (y - x + 1);
    return randomValue;
}

void playGame() {
    int randNum = get_random_number(0, 100);

    // Read file and count the number of words
    long len;
    char *buffer = readFile("TestText.txt", &len);
    if (buffer == NULL) {
        return;
    }

    // Count the number of words based on commas
    const char delimiter[] = ",";
    size_t word_count = countWords(buffer, delimiter);

    // Make sure randNum is within bounds of word_count
    randNum = get_random_number(0, word_count - 1);

    // Tokenize the buffer and store the words
    char **words = tokenizeBuffer(buffer, delimiter, word_count);
    if (words == NULL) {
        free(buffer);
        return;
    }

    char *selectedWord = words[randNum];

    char post[128] = "  ________\n"
                     "  |      |\n"
                     "  |        \n"
                     "  |        \n"
                     "  |        \n"
                     "__|__\n";

    char manPeices[6] ={'o', '|', '/', '\\', '/', '\\'};
    int indexes[6] = {31, 43, 42, 44, 54, 56};

    KeyValuePair **pairs = malloc(6 * sizeof(KeyValuePair*));
    for (int i = 0; i < 6; ++i) {
        pairs[i] = malloc(sizeof(KeyValuePair));
        if (pairs[i] == NULL) {
            perror("Failed to allocate memory for KeyValuePair");
            exit(EXIT_FAILURE);
        }
        (pairs[i])->key[0] = manPeices[i];
        (pairs[i])->key[1] = '\0';
        (pairs[i])->value = indexes[i];
    }

    int wordLength = strlen(selectedWord);
    char playerInput[4];
    char currentWordState[wordLength + 1];
    memset(currentWordState, '_', wordLength);
    currentWordState[wordLength] = '\0';

    int failCount = 0;
    int count = 0;

    // Loop to prompt the player for input
    while (failCount < 6) {
        printf("Fail %d of %d: Please enter a character: ", failCount, 6);
        scanf(" %c", &playerInput[0]);  // The space before %c ensures it skips any leading whitespace

        bool correct = false;
        for (int j = 0; j < wordLength; ++j) {
            if (playerInput[0] == selectedWord[j]) {
                currentWordState[j] = selectedWord[j];
                correct = true;
            }
        }
        if (!correct) {
            post[pairs[failCount]->value] = pairs[failCount]->key[0];
            failCount++;
        }
        printf("%s\n", currentWordState);
        printf("%s\n", post);
        count++;

        if (strcmp(selectedWord, currentWordState) == 0){
            printf("You won!\n");
            break;
        }
    }
    if (strcmp(selectedWord, currentWordState) != 0){
        printf("You lose!\n");
    }
    // Free allocated memory
    freeWords(words, word_count);
    free(buffer);
}

int main() {
    char playAgain;

    // Game loop
    do {
        playGame();

        // Ask the player if they want to play again
        printf("Do you want to play again? (y/n): ");
        scanf(" %c", &playAgain);  // Space before %c to skip any whitespace

    } while (playAgain == 'y' || playAgain == 'Y');

    printf("Thanks for playing!\n");

    return 0;
}
