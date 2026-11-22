// Colorful Snake Game - a simple C++ console game for Windows
// Controls: W A S D to move, X to quit
// Eat the apples () to grow and score points. Don't hit the walls or yourself!

#include <iostream>
#include <vector>
#include <conio.h>     // _kbhit(), _getch() - read key presses
#include <windows.h>   // Sleep(), colors, cursor control
#include <cstdlib>     // rand(), system()
#include <ctime>       // time()
using namespace std;

const int WIDTH = 20;      // each cell is 2 characters wide, so it looks square
const int HEIGHT = 15;

struct Point {
    int x;
    int y;
};

// The snake's body cycles through these colors (a rainbow effect!)
// 9 = blue, 11 = aqua, 13 = pink, 5 = purple
const int RAINBOW[4] = {9, 11, 13, 5};

// Colors: 0 black, 1 blue, 2 green, 3 aqua, 4 red, 5 purple, 6 dark yellow,
// 7 white, 8 gray, 9 bright blue, 10 bright green, 11 bright aqua,
// 12 bright red, 13 pink, 14 yellow, 15 bright white
void setColor(int textColor, int backColor) {
    static int current = -1;                   // remember the last color used
    int value = textColor + backColor * 16;
    if (value == current) return;              // skip if nothing changed
    current = value;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), value);
}

// Move the cursor to the top-left so we redraw the screen without flicker
void gotoTop() {
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// Show or hide the blinking cursor
void showCursor(bool visible) {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = visible;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

// Find the snake segment at a point (returns its number, or -1 if none)
int snakeIndexAt(const vector<Point>& snake, int x, int y) {
    for (size_t i = 0; i < snake.size(); i++) {
        if (snake[i].x == x && snake[i].y == y) {
            return (int)i;
        }
    }
    return -1;
}

// Put the food at a random spot that is not on the snake
Point newFood(const vector<Point>& snake) {
    Point food;
    do {
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;
    } while (snakeIndexAt(snake, food.x, food.y) != -1);
    return food;
}

int main() {
    srand(time(0));
    showCursor(false);

    // ---------- START SCREEN ----------
    setColor(7, 0);
    system("cls");
    setColor(10, 0);
    cout << "\n\n    ===  S N A K E   G A M E  ===\n\n";
    setColor(14, 0);
    cout << "    Eat the red apples to grow!\n";
    cout << "    Move with W A S D, quit with X.\n\n";
    setColor(11, 0);
    cout << "    Press any key to start...";
    _getch();
    setColor(7, 0);
    system("cls");

    // ---------- SET UP THE GAME ----------
    vector<Point> snake;                       // snake[0] is the head
    Point p1 = {WIDTH / 2, HEIGHT / 2};
    Point p2 = {WIDTH / 2 - 1, HEIGHT / 2};
    Point p3 = {WIDTH / 2 - 2, HEIGHT / 2};
    snake.push_back(p1);
    snake.push_back(p2);
    snake.push_back(p3);

    Point food = newFood(snake);
    int dx = 1, dy = 0;                        // start moving right
    int score = 0;

    while (true) {
        // ---------- DRAW ----------
        gotoTop();

        setColor(14, 0);
        cout << "  ~~~ SNAKE ~~~   Score: " << score << "      \n";

        // top wall
        setColor(15, 9);
        for (int i = 0; i < (WIDTH + 2) * 2; i++) cout << ' ';
        setColor(7, 0);
        cout << "\n";

        for (int y = 0; y < HEIGHT; y++) {
            setColor(15, 9);                   // left wall (blue)
            cout << "  ";

            for (int x = 0; x < WIDTH; x++) {
                int index = snakeIndexAt(snake, x, y);

                if (index == 0) {
                    setColor(0, 14);           // yellow head with eyes
                    cout << "oo";
                } else if (index > 0) {
                    setColor(15, RAINBOW[index % 4]);   // rainbow body
                    cout << "  ";
                } else if (x == food.x && y == food.y) {
                    setColor(15, 12);          // red apple
                    cout << "()";
                } else {
                    // grass: checkerboard of two greens
                    int grass = ((x + y) % 2 == 0) ? 2 : 10;
                    if ((x * 7 + y * 3) % 17 == 0) {
                        setColor(13, grass);   // little pink flowers
                        cout << "* ";
                    } else {
                        setColor(7, grass);
                        cout << "  ";
                    }
                }
            }

            setColor(15, 9);                   // right wall (blue)
            cout << "  ";
            setColor(7, 0);
            cout << "\n";
        }

        // bottom wall
        setColor(15, 9);
        for (int i = 0; i < (WIDTH + 2) * 2; i++) cout << ' ';
        setColor(11, 0);
        cout << "\n  W A S D = move    X = quit      \n";
        setColor(7, 0);

        // ---------- INPUT ----------
        if (_kbhit()) {
            char key = _getch();
            // the extra checks stop the snake from reversing into itself
            if ((key == 'w' || key == 'W') && dy == 0) { dx = 0; dy = -1; }
            if ((key == 's' || key == 'S') && dy == 0) { dx = 0; dy = 1; }
            if ((key == 'a' || key == 'A') && dx == 0) { dx = -1; dy = 0; }
            if ((key == 'd' || key == 'D') && dx == 0) { dx = 1; dy = 0; }
            if (key == 'x' || key == 'X') { break; }
        }

        // ---------- MOVE ----------
        Point head = {snake[0].x + dx, snake[0].y + dy};

        // hit a wall?
        if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT) {
            break;
        }
        // hit yourself?
        if (snakeIndexAt(snake, head.x, head.y) != -1) {
            break;
        }

        snake.insert(snake.begin(), head);     // add new head

        if (head.x == food.x && head.y == food.y) {
            score += 10;                       // ate an apple: grow (keep the tail)
            food = newFood(snake);
        } else {
            snake.pop_back();                  // no apple: remove the tail
        }

        // the game speeds up as your score grows
        int delay = 150 - (score / 10) * 5;
        if (delay < 60) delay = 60;
        Sleep(delay);
    }

    // ---------- GAME OVER ----------
    showCursor(true);
    setColor(12, 0);
    cout << "\n  GAME OVER!\n";
    setColor(14, 0);
    cout << "  Final score: " << score << "\n";
    setColor(7, 0);
    return 0;
}
