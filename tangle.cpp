#include <chrono>
#include <iostream>
#include <list>
#include <string>
#include <iomanip>

#include <pico/stdlib.h>

using namespace std;
using namespace chrono;

const string tile_specs[25] = {"gyrybgrb", "gryrbgyb", "gybyrgbr", "gbybrgyr", "grbrygby",
                               "gbrbygry", "byrygbrg", "bryrgbyg", "bygyrbgr", "bgygrbyr",
                               "brgrybgy", "bgrgybry", "rybygrbg", "rbybgryg", "rygybrgb",
                               "rgygbryb", "rbgbyrgy", "rgbgyrby", "yrbrgybg", "ybrbgyrg",
                               "yrgrbygb", "ygrgbyrb", "ybgbrygr", "ygbgrybr", "gyrybgrb"};

static uint8_t tiles[2][25][4][4];
static bool used[25];

struct square {
    bool occupied;
    int piece;
    int rotation;
};

static pair<int, int> neighbors[4] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

static square board[5][5];

static int soln = 0;

static bool fits(int y, int x) {
    auto& sq = board[y][x];
    for (int n = 0; n < 4; n++) {
        int ny = y + neighbors[n].first;
        if (ny < 0 || ny > 4)
            continue;
        int nx = x + neighbors[n].second;
        if (nx < 0 || nx > 4)
            continue;
        if (!board[ny][nx].occupied)
            continue;
        auto& sqn = board[ny][nx];
        if (tiles[0][sq.piece][sq.rotation][n] != tiles[1][sqn.piece][sqn.rotation][(n + 2) % 4])
            return false;
    }
    return true;
}

static void search(int y, int x) {
    if (!board[y][x].occupied) {
        board[y][x].occupied = true;
        for (int p = 0; p < 25; p++)
            if (!used[p]) {
                used[p] = true;
                board[y][x].piece = p;
                for (int r = 0; r < 4; r++) {
                    board[y][x].rotation = r;
                    if (fits(y, x)) {
                        if (y == 4 && x == 4) {
                            cout << "Solution #" << ++soln << endl;
                            for (int yy = 0; yy < 5; yy++) {
                                for (int xx = 0; xx < 5; xx++)
                                    cout << ' ' << setw(2) << board[yy][xx].piece << ','
                                         << board[yy][xx].rotation << ' ';
                                cout << endl;
                            }
                            cout << endl;
                        } else {
                            int sx = x, sy = y;
                            if (++x > 4) {
                                x = 0;
                                ++y;
                            }
                            search(y, x);
                            y = sy;
                            x = sx;
                        }
                    }
                }
                used[p] = false;
            }
        board[y][x].occupied = false;
    }
}

static uint8_t s2u(string s) {
    uint8_t n = 0;
    for (auto c : s) {
        n <<= 4;
        switch (c) {
        case 'r':
            n += 0;
            break;
        case 'g':
            n += 1;
            break;
        case 'b':
            n += 2;
            break;
        case 'y':
            n += 3;
            break;
        }
    }
    return n;
}

static uint8_t swap(uint8_t u) { return (u << 4) | (u >> 4); }

int main() {
	stdio_init_all();
    auto strt = high_resolution_clock::now();
    int t = 0;
    for (auto& s : tile_specs) {
        for (auto r : {0, 2, 4, 6}) {
            tiles[0][t][0][r / 2] = s2u(s.substr(r, 2));
            tiles[0][t][1][r / 2] = s2u(s.substr((r + 2) % 8, 2));
            tiles[0][t][2][r / 2] = s2u(s.substr((r + 4) % 8, 2));
            tiles[0][t][3][r / 2] = s2u(s.substr((r + 6) % 8, 2));
            tiles[1][t][0][r / 2] = swap(tiles[0][t][0][r / 2]);
            tiles[1][t][1][r / 2] = swap(tiles[0][t][1][r / 2]);
            tiles[1][t][2][r / 2] = swap(tiles[0][t][2][r / 2]);
            tiles[1][t][3][r / 2] = swap(tiles[0][t][3][r / 2]);
        }
        t++;
    }
    search(0, 0);
    cout << duration_cast<microseconds>(high_resolution_clock::now() - strt).count() / 1000000.0
         << " s." << endl;
}
