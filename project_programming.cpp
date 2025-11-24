#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <locale.h>
#include <ctype.h>

typedef struct {
    char question[256];
    char answers[4][128];
    char correct;
} Question;


int menu_main(void);
int menu_choose_topic(void);
const char* get_filename_for_topic(int t);
int menu_choose_history_subtype(void);
int load_questions(const char* filename, Question questions[], int max);
void shuffle_answers(Question* q, int out_idx[4]);
void start_quiz(Question questions[], int count, int topic, int subtopic);
int menu_choose_record_type(void);
void save_score_for_topic(int topic, int subtopic, int score);
int ask_retry();
int load_best_score();
int load_best_score(const char* filename);
void save_best_score(int score);
void save_best_score(int score, const char* filename);

int main() {
    Question* questions = (Question*)malloc(50 * sizeof(Question));
    if (!questions) {
        printf("Blad alokacji pamieci!");
        return 1;
    }
    int subtopic = 0;
    int count = 0;
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    int w;
    setlocale(LC_ALL, "pl_PL.UTF-8");
    srand(time(NULL));

    while (1) {
        w = menu_main();
        if (w == 1) {
           
            int t = menu_choose_topic();
            if (t <= 5 && t >= 1) {
                printf("▶ Start quizu...\n");
                const char* filename = get_filename_for_topic(t);
                count = load_questions(filename, questions, 50);
                if (count > 0) {
                    start_quiz(questions, count, t, subtopic);
                    if (ask_retry() == 1) {
                        continue;   // wracamy do menu głównego
                    }
                    else {
                        break;      // kończymy program
                    }

                }

                else
                    printf("❌ Nie udało się wczytać pytań.\n");

                break;
            }
            
            
            else {
                while (t > 5 || t < 1) {
                    printf("Są tylko 5 tematów do wyboru\n");
                    t = menu_choose_topic();
                }
                
                printf("▶ Start quizu...\n");
                const char* filename = get_filename_for_topic(t);
                count = load_questions(filename, questions, 50);
                if (count > 0) {
                    if (t == 2) subtopic = menu_choose_history_subtype();

                    start_quiz(questions, count, t, subtopic);
                    if (ask_retry() == 1) {
                        continue;   // wracamy do początku main
                    }
                    else {
                        break;
                    }
                    break;
                }
                
            }
        }
        else if (w == 2) {
            int r = menu_choose_record_type();

            int best = 0;

            switch (r) {
            case 1: best = load_best_score(); break;
            case 2: best = load_best_score("rekord_kultura.txt"); break;
            case 3: {
                int sub = menu_choose_history_subtype();
                if (sub == 1)
                    best = load_best_score("rekord_historia_polska.txt");
                else
                    best = load_best_score("rekord_historia_swiat.txt");
                break;
            }
            case 4: best = load_best_score("rekord_geografia.txt"); break;
            case 5: best = load_best_score("rekord_matematyka.txt"); break;
            case 6: best = load_best_score("rekord_fizyka.txt"); break;
            default:
                printf("Niepoprawny wybor.\n");
                continue;
            }

            printf("\n==============================\n");
            printf("  Rekord: %d\n", best);
            printf("==============================\n\n");
        }
        else if (w == 3) {
            printf("Do zobaczenia!\n");
            break;
        }

        else {
           printf("Niepoprawna opcja.\n");
            break;
        }
    }
    free(questions);
    return 0;

}
int menu_main(void) {
    int w;
    do {
        printf("===============================\n");
        printf("        QUIZ EDUKACYJNY       \n");
        printf("===============================\n\n");
        printf("1. Rozpocznij quiz\n");
        printf("2. Pokaż najlepszy wynik\n");
        printf("3. Zakończ program\n\n");
        printf("Wybierz opcję (1-3): ");
        scanf_s("%d", &w);
        if (w < 1 || w > 3) {

            printf("Niepoprawna opcja. Spróbuj ponownie.\n\n");
        }
    } while (w < 1 || w > 3);
    return w;
}

int menu_choose_topic(void) {
    int t;
    printf("\n=== Wybierz temat ===\n");
    printf("1. Kultura\n");
    printf("2. Historia\n");
    printf("3. Geografia\n");
    printf("4. Matematyka\n");
    printf("5. Fizyka\n");
    printf("Wybierz temat (1-5): ");
    scanf_s("%d", &t);
    return t;
}
int menu_choose_history_subtype(void) {
    int h;

    while (1) {
        printf("\n=== Wybierz rodzaj historii ===\n");
        printf("1. Historia Polski\n");
        printf("2. Historia świata\n");
        printf("Wybór (1-2): ");

        if (scanf_s("%d", &h) != 1) {
            while (getchar() != '\n');
            printf("Niepoprawny format, spróbuj ponownie.\n");
            continue;
        }

        if (h == 1 || h == 2)
            return h;

        printf("Do wyboru są tylko opcje 1 lub 2.\n");
    }
}




const char* get_filename_for_topic(int t) {
    switch (t) {
    case 1: return "pytania_kultura.txt";
    case 2: {
        int h = menu_choose_history_subtype(); // <-- нове підменю
        if (h == 1) return "pytania_historia_polska.txt";
        else return "pytania_historia_swiat.txt";
    }
    case 3: return "pytania_geografia.txt";
    case 4: return "pytania_matematyka.txt";
    case 5: return "pytania_fizyka.txt";
    default: return NULL;  
    }
}
int load_questions(const char* filename, Question questions[], int max) {
    int count = 0;
    FILE* f;
    errno_t err;
    err = fopen_s(&f, filename, "r");
    if (err != 0) {
        printf("Nie można otworzyć pliku: %s\n", filename);
        return 0;
    }

  //  printf("✅ Plik otwarty poprawnie: %s\n", filename);

    while (count < max && fgets(questions[count].question, 256, f)) {
        questions[count].question[strcspn(questions[count].question, "\n")] = 0;

        for (int i = 0; i < 4; i++) {
            if (fgets(questions[count].answers[i], 128, f)) {
                questions[count].answers[i][strcspn(questions[count].answers[i], "\n")] = 0;
            }
        }

        // читаємо правильну літеру (A/B/C/D)
        fscanf_s(f, " %c\n", &questions[count].correct, 1);
        count++;
    }

    fclose(f);
    return count;
}
void shuffle_answers(Question* q, int out_idx[4]) {
    int idx[4] = { 0, 1, 2, 3 };
    int correct_index;
    char new_correct;
    int temp;
    // Fisher-Yates shuffle
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        temp = idx[i];
        idx[i] = idx[j];
        idx[j] = temp;
    }

    // kopiujemy wynik do tablicy wynikowej
    for (int i = 0; i < 4; i++) {
        out_idx[i] = idx[i];
    }

    // aktualizacja poprawnej odpowiedzi
    correct_index = q->correct - 'A';
    new_correct = 'A';

    for (int i = 0; i < 4; i++) {
        if (idx[i] == correct_index) {
            new_correct = 'A' + i;
            break;
        }
    }

    q->correct = new_correct;
}
void start_quiz(Question questions[], int count, int topic, int subtopic) {
    int limit = 20;
    int score = 0;
    int q;
    char answer;
    char user_answers[50]; // масив для збереження відповідей користувача
    int order[50];
    float percent;
    int temp;
    int answer_order[4];
    int best = load_best_score();

    for (int i = 0; i < count; i++) {
        order[i] = i;
    }

    // === 2. Losowanie indeksow ===
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        temp = order[i];
        order[i] = order[j];
        order[j] = temp;
    }
    if (count < limit)
        limit = count;

    printf("\n==============================\n");
    printf("       ✳ START QUIZU ✳\n");
    printf("==============================\n\n");

    for (int i = 0; i < limit; i++) {

         q = order[i]; // <-- ось ГОЛОВНЕ
         shuffle_answers(&questions[q], answer_order);
        printf("Pytanie %d: %s\n", i + 1, questions[q].question);
        for (int j = 0; j < 4; j++) {
            printf("%c) %s\n", 'A' + j, questions[q].answers[answer_order[j]]);
           // printf("%s\n", questions[q].answers[j]);
        }

        // Введення відповіді користувача
        do {
            printf("Wybierz odpowiedź (A-D): ");
            scanf_s(" %c", &answer, 1);
            answer = toupper(answer);
            if (answer < 'A' || answer > 'D') {
                printf("❌ Niepoprawna odpowiedź! Spróbuj ponownie.\n");
            }
        } while (answer < 'A' || answer > 'D');

        user_answers[i] = answer; // зберігаємо вибір користувача
        printf("\n");
        if (answer == questions[q].correct) {
            printf("\x1b[32m✔ Poprawna odpowiedź!\x1b[0m\n\n");
        }
        else {
            printf("\x1b[31m❌ Błędna odpowiedź! Poprawna to: %c\x1b[0m\n\n", questions[q].correct);
        }
    }

    // Після всіх питань — перевірка результату
    for (int i = 0; i < limit; i++) {
        int q = order[i];
        if (user_answers[i] == questions[q].correct)
            score++;
        

    }
    percent = (score * 100.0f) / limit;
    printf("Wynik procentowy: %.2f%%\n", percent);

    if (percent < 40)
        printf("\033[31mSłabo 😞\033[0m\n");
    else if (percent < 70)
        printf("\033[33mNieźle 🙂\033[0m\n");
    else
        printf("\033[32mŚwietnie! 😄\033[0m\n");

    printf("==============================\n");
    printf("Twój wynik: %d / %d\n", score, limit);
    printf("==============================\n");
    save_score_for_topic(topic, subtopic, score);
    best = load_best_score();

    if (score > best) {
        printf("\033[32mNowy rekord! 🎉\033[0m\n");
        save_best_score(score);
    }
    else {
        printf("Najlepszy wynik do tej pory: %d\n", best);
    }
}
int menu_choose_record_type(void) {
    int pick;
    printf("\n=== Wybierz rekord do wyświetlenia ===\n");
    printf("1. Ogólny rekord\n");
    printf("2. Rekord – Kultura\n");
    printf("3. Rekord – Historia\n");
    printf("4. Rekord – Geografia\n");
    printf("5. Rekord – Matematyka\n");
    printf("6. Rekord – Fizyka\n");
    printf("Wybierz opcję (1-6): ");
    scanf_s("%d", &pick);
    return pick;
}
int ask_retry() {
    char wybor;

    while (1) {
        printf("Czy chcesz zagrać ponownie? (T/N): ");
        scanf_s(" %c", &wybor, 1);

        wybor = toupper(wybor);

        if (wybor == 'T')
            return 1;
        else if (wybor == 'N') {
            printf("Do zobaczenia!!");
            return 0;
        }
        printf("Niepoprawny wybór. Wpisz T lub N.\n");
    }
}
void save_score_for_topic(int topic, int subtopic, int score) {
    switch (topic) {

    case 1:
        save_best_score(score, "rekord_kultura.txt");
        break;

    case 2:  // Historia
        if (subtopic == 1)
            save_best_score(score, "rekord_historia_polska.txt");
        else
            save_best_score(score, "rekord_historia_swiat.txt");
        break;

    case 3:
        save_best_score(score, "rekord_geografia.txt");
        break;

    case 4:
        save_best_score(score, "rekord_matematyka.txt");
        break;

    case 5:
        save_best_score(score, "rekord_fizyka.txt");
        break;

    default:
        save_best_score(score); // ogólny
        break;
    }
}

int load_best_score() {
    FILE* f;
    int best = 0;
    errno_t err;
    err = fopen_s(&f, "wyniki.txt", "r");
    if (err != 0 || !f) {
        return 0; // jeśli pliku nie ma, zwracamy 0
    }

    fscanf_s(f, "%d", &best);
    fclose(f);
    return best;
}
int load_best_score(const char* filename) {
    FILE* f;
    int best = 0;

    if (fopen_s(&f, filename, "r") != 0 || !f)
        return 0;

    fscanf_s(f, "%d", &best);
    fclose(f);
    return best;
}
void save_best_score(int score) {
    FILE* f;
    fopen_s(&f, "wyniki.txt", "w");
    if (!f) return;

    fprintf(f, "%d", score);
    fclose(f);
}
void save_best_score(int score, const char* filename) {
    FILE* f;
    fopen_s(&f, filename, "w");
    if (!f) return;

    fprintf(f, "%d", score);
    fclose(f);
}
