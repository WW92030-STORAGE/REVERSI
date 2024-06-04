#include <iostream>
#include "reversi.h"
#include "genetic.h"

// Manually play Othello as 2 players
void manual() {
    ReversiGame game;
    
    game.disp();
    
    ReversiAI ai;
    
    int x, y;
    while (std::cin >> x >> y) {
        int p = game.execute(x, y);
        if (p < 0) {
            std::cout << "INVALID MOVE\n";
            continue;
        }
        game.disp();
        game.sidetomove = !game.sidetomove;
        
        std::pair<int, int> move = ai.pick(game);
        if (move.first < 0 && move.second < 0) break;
        game.execute(move);
        game.disp();
        
        game.swapside();
        
        // for (auto i : game.getAllLegalMoves()) std::cout << i.first << " " << i.second << "\n";
    }
}

// Train an Othello playing model using genetic evolution
void train() {
    std::vector<ReversiAI> v;
    for (int i = 0; i < 32; i++) v.push_back(Genetic::randomAI());
    
    for (int i = 0; i < 8; i++) {
        std::vector<ReversiAI> top = Genetic::tournament(v);
        for (auto i : top) std::cout << i.toString() << "\n";
        std::vector<ReversiAI> res;
        for (int s = 0; s < 4; s++) {
            std::random_shuffle(top.begin(), top.end());
            for (int j = 0; j < top.size() - 1; j += 2) res.push_back(Genetic::mutate(Genetic::cross(top[j], top[j + 1])));
        }
        v = res;
    }
    
    // Reduction
    
    while (v.size() > 1) {
        std::vector<ReversiAI> top = Genetic::tournament(v);
        for (auto i : top) std::cout << i.toString() << "\n";
        std::vector<ReversiAI> res;
        for (int s = 0; s < 2; s++) {
            std::random_shuffle(top.begin(), top.end());
            for (int j = 0; j < top.size() - 1; j += 2) res.push_back(Genetic::mutate(Genetic::cross(top[j], top[j + 1])));
        }
        v = res;
    }
    
    for (auto i : v) std::cout << i.toString() << "\n";
    
    for (int i = 0; i < 16; i++) {
        std::cout << Genetic::test(v[0], Genetic::randomAI()) << " ";
    }
    std::cout << "\n";
    
    for (int i = 0; i < 16; i++) {
        std::cout << Genetic::test(Genetic::randomAI(), v[0]) << " ";
    }
    std::cout << "\n";
}

int main()
{
    srand(time(0));
    ReversiAI res(0.884372, -1.941420);
    
    /*
    
    for (int i = 0; i < 16; i++) {
        std::cout << Genetic::test(res, Genetic::randomAI(), false, 32) << " ";
    }
    std::cout << "\n";
    
    for (int i = 0; i < 16; i++) {
        std::cout << Genetic::test(Genetic::randomAI(), res, false, 32) << " ";
    }
    std::cout << "\n";
    
    */
    
    for (int i = 0; i < 16; i++) {
        std::cout << Genetic::test(res, res, false, 16) << " ";
    }
    std::cout << "\n";

    return 0;
    
    // 0.884372 -1.941420
}
