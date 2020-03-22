#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <ctime>
#include <string.h>
#include <curses.h>
using namespace std;


#define OFFSET 2

wstring tetromino[7];

int nFieldWidth = 12;   // 10 or 12
int nFieldHeight = 18;  // 20 or 18
unsigned char *pField = nullptr;

int nScreenWidth = 80;      // console screen size x
int nScreenHeight = 30;     // console screen size y

/**
 * given (px, py) and rotation index r
 * return the corresponding tetromino index
 * (0, 90, 180, 270) clockwise rotation for r = (0, 1, 2, 3)
 */
int Rotate(int px, int py, int r)
{
    const int width = 4;
    int index = -1;

    if (r % 4 == 0) {
        index = py * width + px;        // rotate 0 deg
    } else if (r % 4 == 1) {
        index = 12 + py - (px * width); // rotate 90 deg
    } else if (r % 4 == 2) {
        index = 15 - (py * width) - px; // rotation by 180 deg
    } else if (r % 4 == 3) {
        index = 3 - py + (px * width);  // rotation by 270 deg
    }

    return index;
}

/**
 * Print and refresh a window given a screen input.
 * 
 * The window is provided after it has been initialised.
 * The screen consist of a set number of characters defined by screen width and
 * screen height.
 * 
 * @param win Window which will be refreshed
 * @param screen Screen whose contents will be displayed
 */
void PrintAndRefreshScreen(WINDOW *win, char *screen)
{
    // wclear(win);
    mvwprintw(win, 0, 0, screen);
    wrefresh(win);
}

bool DoesPieceFit(int nTetromino, int rotation, int pos_x, int pos_y)
{
    for (int px = 0; px < 4; px++) {
        for (int py = 0; py < 4; py++) {
            // get index into piece
            int pi = Rotate(px, py, rotation);

            // get index into field
            int fi = (pos_y + py) * nFieldWidth + (pos_x + px);

            if (pos_x + px >= 0 && pos_x + px < nFieldWidth) {
                if (pos_y + py >= 0 && pos_y + py < nFieldHeight) {
                    if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0) {
                        return false; // collision detected
                    }
                }
            }
        }
    }
    return true;
}

int main(int argc, char ** argv)
{
    // create our assets
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");

    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"..X.");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"....");

    tetromino[5].append(L"....");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");

    tetromino[6].append(L"....");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L".X..");
    tetromino[6].append(L".X..");

    pField = new unsigned char[nFieldWidth * nFieldHeight];

    for (int x = 0; x < nFieldWidth; x++) {
        for (int y = 0; y < nFieldHeight; y++) {
            // set board to 0 unless it is on the border
            pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

            // DEBUG
            // cout << static_cast<unsigned>(pField[y * nFieldWidth + x]) << endl;
        }
    }

    char *screen = new char[nScreenWidth * nScreenHeight];

    for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
        screen[i] = ' ';
    }

    // init screen
    // set up memory and clear screen
    initscr();
    cbreak();
    noecho();

    int height, width, start_y, start_x;
    height = nScreenHeight;
    width = nScreenWidth;
    start_y = start_x = 0;

    WINDOW * win = newwin(height, width, start_y, start_x);

    // enable arrow keys to be compared with defined constants
    keypad(win, true);
    nodelay(win, true);
    
    refresh();

    PrintAndRefreshScreen(win, screen);

    getch();




    bool game_over = false;

    int nCurrentPiece = 0;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;

    bool bKey[4];
    bool bRotateHold = false;

    const int nSpeed = 20;
    int nSpeedCounter = 0;
    bool bForceDown = false;
    srand(time(NULL));

    vector<int> vLines;

    while (!game_over)
    {
        // game timing -------------------------------
        this_thread::sleep_for(chrono::milliseconds(50)); // game tick
        nSpeedCounter++;
        bForceDown = (nSpeedCounter == nSpeed);

        // input -------------------------------------
        int c = wgetch(win);

        // for (int k = 0; k < 4; k++) {
        //     bKey[k] = (0x8000 & GetAsync)
        // }

        // game logic --------------------------------
        if (c == KEY_LEFT && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) {
            nCurrentX = nCurrentX - 1;
        }

        if (c == KEY_RIGHT && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) {
            nCurrentX = nCurrentX + 1;
        }

        if (c == KEY_DOWN && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) {
            nCurrentY = nCurrentY + 1;
        }

        if (c == KEY_UP && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) {
            nCurrentRotation += 1;
        }

        if (bForceDown) {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) {
                nCurrentY++;
            } else {
                // --- Lock current piece in the field
                for (int px = 0; px < 4; px++) {
                    for (int py = 0; py < 4; py++) {
                        if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') {
                            pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
                            // +1 in the end because the character string's first character denotes empty space
                        }
                    }
                }

                // --- Check for complete lines 
                for (int py = 0; py < 4; py++) {
                    if (nCurrentY + py < nFieldHeight - 1) {
                        bool bLine = true;
                        
                        for (int px = 1; px < nFieldWidth - 1; px++) {
                            if (pField[(nCurrentY + py) * nFieldWidth + px] == 0) {
                                // empty space found
                                bLine = false;
                            } 
                        }

                        if (bLine) {
                            // set line to =
                            for (int px = 1; px < nFieldWidth - 1; px++) {
                                pField[(nCurrentY + py) * nFieldWidth + px] = 8;
                            }

                            // store the line number where we have complete line
                            vLines.push_back(nCurrentY + py);
                        }
                    }
                }

                // --- Choose next piece
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = rand() % 7;

                // --- If piece does not fit
                game_over = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }

            nSpeedCounter = 0;
        }
        

        // render output -----------------------------
        
        // draw the tetris field
        for (int x = 0; x < nFieldWidth; x++) {
            for (int y = 0; y < nFieldHeight; y++) {
                screen[(y + OFFSET) * nScreenWidth + (x + OFFSET)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
            }
        }

        // draw current piece
        for (int px = 0; px < 4; px++) {
            for (int py = 0; py < 4; py++) {
                if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') {
                    // cout << "Statement is true" << endl;
                    screen[(nCurrentY + py + OFFSET) * nScreenWidth + (nCurrentX + px + OFFSET)] = nCurrentPiece + 65;
                }
            }
        }

        if (!vLines.empty())
        {
            PrintAndRefreshScreen(win, screen);
            this_thread::sleep_for(chrono::milliseconds(400));

            for (auto &v : vLines) {
                for (int px = 1; px < nFieldWidth - 1; px++) {
                    // move everything down by one row
                    for (int py = v; py > 0; py --) {
                        pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
                    }
                    // set the top row to be 0 (empty)
                    pField[px] = 0;
                }
            }
            
            vLines.clear();
        }

        // display the frame
        PrintAndRefreshScreen(win, screen);
    }
    
    
    int c = getch();

    // deallocate memory and ends ncurses
    endwin();

    return 0;
}