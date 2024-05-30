#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 40
#define HEIGHT 20

typedef struct SnakeSegment {
    int x;
    int y;
    struct SnakeSegment *next;
} SnakeSegment;

typedef struct {
    int x;
    int y;
} Food;

void initGame(WINDOW *win, SnakeSegment **head, Food *food, int *score);
void drawBorders(WINDOW *win);
void drawSnake(WINDOW *win, SnakeSegment *head);
void drawFood(WINDOW *win, Food food);
void moveSnake(SnakeSegment **head, int direction);
void growSnake(SnakeSegment **head);
int checkCollision(SnakeSegment *head);
int checkFoodCollision(SnakeSegment *head, Food *food, int *score);
int getDirection(int currentDirection);

int main() {
    int score = 0;
    int direction = KEY_RIGHT;
    int newDirection;
    SnakeSegment *snakeHead = NULL;
    Food food;
    WINDOW *win;

    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(100); // Set the game speed (100 ms)

    win = newwin(HEIGHT, WIDTH, 0, 0);
    initGame(win, &snakeHead, &food, &score);

    while (1) {
        werase(win);
        drawBorders(win);
        drawSnake(win, snakeHead);
        drawFood(win, food);
        wrefresh(win);

        newDirection = getDirection(direction);
        if (newDirection != -1) {
            direction = newDirection;
        }
        
        moveSnake(&snakeHead, direction);
        if (checkCollision(snakeHead)) {
            break; // Game over
        }

        if (checkFoodCollision(snakeHead, &food, &score)) {
            growSnake(&snakeHead);
        }
    }

    endwin();
    printf("Game Over! Your score: %d\n", score);

    // Free snake memory
    SnakeSegment *current = snakeHead;
    while (current != NULL) {
        SnakeSegment *next = current->next;
        free(current);
        current = next;
    }

    return 0;
}

void initGame(WINDOW *win, SnakeSegment **head, Food *food, int *score) {
    *score = 0;

    // Initialize the snake
    *head = (SnakeSegment*)malloc(sizeof(SnakeSegment));
    (*head)->x = WIDTH / 2;
    (*head)->y = HEIGHT / 2;
    (*head)->next = NULL;

    // Initialize the food
    food->x = rand() % (WIDTH - 2) + 1;
    food->y = rand() % (HEIGHT - 2) + 1;
}

void drawBorders(WINDOW *win) {
    for (int i = 0; i < WIDTH; ++i) {
        mvwaddch(win, 0, i, '#');
        mvwaddch(win, HEIGHT - 1, i, '#');
    }
    for (int i = 0; i < HEIGHT; ++i) {
        mvwaddch(win, i, 0, '#');
        mvwaddch(win, i, WIDTH - 1, '#');
    }
}

void drawSnake(WINDOW *win, SnakeSegment *head) {
    SnakeSegment *current = head;
    while (current != NULL) {
        mvwaddch(win, current->y, current->x, 'O');
        current = current->next;
    }
}

void drawFood(WINDOW *win, Food food) {
    mvwaddch(win, food.y, food.x, '*');
}

void moveSnake(SnakeSegment **head, int direction) {
    SnakeSegment *newHead = (SnakeSegment*)malloc(sizeof(SnakeSegment));
    newHead->x = (*head)->x;
    newHead->y = (*head)->y;
    newHead->next = *head;

    switch (direction) {
        case KEY_UP:
            newHead->y -= 1;
            break;
        case KEY_DOWN:
            newHead->y += 1;
            break;
        case KEY_LEFT:
            newHead->x -= 1;
            break;
        case KEY_RIGHT:
            newHead->x += 1;
            break;
    }

    *head = newHead;

    // Remove the tail
    SnakeSegment *current = *head;
    while (current->next->next != NULL) {
        current = current->next;
    }
    free(current->next);
    current->next = NULL;
}

void growSnake(SnakeSegment **head) {
    // Simply add a new segment at the head, the snake will grow naturally
}

int checkCollision(SnakeSegment *head) {
    if (head->x <= 0 || head->x >= WIDTH - 1 || head->y <= 0 || head->y >= HEIGHT - 1) {
        return 1;
    }

    SnakeSegment *current = head->next;
    while (current != NULL) {
        if (current->x == head->x && current->y == head->y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int checkFoodCollision(SnakeSegment *head, Food *food, int *score) {
    if (head->x == food->x && head->y == food->y) {
        *score += 10;
        food->x = rand() % (WIDTH - 2) + 1;
        food->y = rand() % (HEIGHT - 2) + 1;
        return 1;
    }
    return 0;
}

int getDirection(int currentDirection) {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
            if (currentDirection != KEY_DOWN) return KEY_UP;
            break;
        case KEY_DOWN:
            if (currentDirection != KEY_UP) return KEY_DOWN;
            break;
        case KEY_LEFT:
            if (currentDirection != KEY_RIGHT) return KEY_LEFT;
            break;
        case KEY_RIGHT:
            if (currentDirection != KEY_LEFT) return KEY_RIGHT;
            break;
    }
    return -1; // No change
}
