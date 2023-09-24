
void print_word(char *word, char *guesses) {
    while (*word != '\0') {
        if (strchr(guesses, *word) != NULL)
            printf("%c ", *word);
        else
            printf("_ ");
        word++;
    }
}

int check_guess(char guess, char *word, char *guesses) {
    if (strchr(word, guess) == NULL)
        return -1;
    
    guesses[strlen(guesses)] = guess;
    
    return 0;
}

int check_game_over(char *word, char *guesses, int tries_left) {
    int i;

    for (i = 0; i < strlen(word); i++) {
        if (strchr(guesses, word[i]) == NULL)
            return tries_left <= 0 ? -1 : 0;
    }

    return 1;
}



int main() {
	srand(time(NULL));
	
	int random_index=rand()%num_words;

	hangman(words[random_index]);

	return EXIT_SUCCESS; 
}