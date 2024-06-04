#ifndef REVERSI_H
#define REVERSI_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <string>

class ReversiGame {
    public:
    bool sidetomove = 0; // 1 WHITE 0 BLACK
    std::vector<std::vector<int8_t>> board; // -1 empty 0 black 1 white
    
    int width = 8;
    int height = 8;
    
    int prevconvs = 0;
    
    bool operator<(ReversiGame& other) {
        if (sidetomove != other.sidetomove) return sidetomove < other.sidetomove;
        if (width != other.width) return width < other.width;
        if (height != other.height) return height < other.height;
        if (board != other.board) return board < other.board;
        return false;
    }
    
    ReversiGame() {
        width = 8;
        height = 8;
        board = std::vector<std::vector<int8_t>>(width, std::vector<int8_t>(height, -1));
        
        reset();
    }
    
    ReversiGame(ReversiGame& game) {
        sidetomove = game.sidetomove;
        width = game.width;
        height = game.height;
        board = std::vector<std::vector<int8_t>>(width, std::vector<int8_t>(height, -1));
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) board[x][y] = game.board[x][y];
        }
    }
    
    ReversiGame(int w, int h) {
        width = w;
        height = h;
        if (width & 1) width++;
        if (height & 1) height++; // Maintain even board size
        board = std::vector<std::vector<int8_t>>(width, std::vector<int8_t>(height, -1));
        
        reset();
    }
    
    void reset() {
        sidetomove = 0;
        
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) board[x][y] = -1;
        }
        board[(width>>1)][(height>>1)] = 0;
        board[(width>>1) - 1][(height>>1) - 1] = 0;
        board[(width>>1) - 1][(height>>1)] = 1;
        board[(width>>1)][(height>>1) - 1] = 1;
    }
    
    int8_t get(std::pair<int, int> p) { return board[p.first][p.second]; }
    int8_t get(int x, int y) { return get(std::make_pair(x, y)); }
    
    bool inBounds(std::pair<int, int> p) {
        if (p.first < 0 || p.second < 0) return false;
        if (p.first >= width || p.second >= height) return false;
        return true;
    }
    
    bool inBounds(int x, int y) { return inBounds(std::make_pair(x, y)); }
    
    int8_t getCur() { return sidetomove ? 1 : 0; }
    int8_t getOpp() { return sidetomove ? 0 : 1; }
    
    // Executes a move (returns -1 if already occupied or OOB) and returns the number of converted pieces. (Pieces are also converted)
    int execute(std::pair<int, int> p) {
        if (!inBounds(p)) return -1;
        if (get(p) != -1) return -1;
        
        int dx[8] = {01, 01, 00, -1, -1, -1, 00, 01};
        int dy[8] = {00, 01, 01, 01, 00, -1, -1, -1};
        
        board[p.first][p.second] = getCur();
        
        int res = 0;
        
        for (int d = 0; d < 8; d++) {
            int x = p.first + dx[d];
            int y = p.second + dy[d];
            while (true) {
                if (!inBounds(x, y)) break;
                if (get(x, y) != getOpp()) break;
                x += dx[d];
                y += dy[d];
            }
            if (!inBounds(x, y)) continue;
            if (get(x, y) == -1) continue;
            int px = p.first + dx[d];
            int py = p.second + dy[d];
            while (true) {
                if (px == x && py == y) break;
                board[px][py] = getCur();
                res++;
                px += dx[d];
                py += dy[d];
            }
        }
        prevconvs = res;
        return res;
    }
    
    int execute(int x, int y) { return execute(std::make_pair(x, y)); }
    
    bool isLegalMove(std::pair<int, int> p) {
        ReversiGame game2(*this);
        return game2.execute(p) > 0;
    }
    bool isLegalMove(int x, int y) { return isLegalMove(std::make_pair(x, y)); }
    
    std::vector<std::pair<int, int>> getAllLegalMoves() {
        std::vector<std::pair<int, int>> res;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (isLegalMove(x, y)) res.push_back(std::make_pair(x, y));
            }
        }
        return res;
    }
    
    void swapside() {
        sidetomove = !sidetomove;
    }
    
    std::string toString() {
        std::string res = "";
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (board[x][y] == -1) res = res + ".";
                else if (board[x][y] == 0) res = res + "X";
                else if (board[x][y] == 1) res = res + "O";
                else res = res + "?";
            }
            res = res + "\n";
        }
        res = res + "[" + std::to_string(sidetomove) + "]";
        return res;
    }
    
    void disp() {
        std::cout << toString() << "\n";
    }
    
    bool gameover() { // No legal moves for both players
        if (getAllLegalMoves().size() > 0) return false;
        ReversiGame game2(*this);
        game2.swapside();
        return game2.getAllLegalMoves().size() <= 0;
    }
};

#endif
