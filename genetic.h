#ifndef GENETIC_H
#define GENETIC_H

#include <vector>
#include <cfloat>
#include <climits>
#include <algorithm>
#include <iostream>
#include <string>
#include "reversi.h"

class ReversiAI {
    public:
    double piececount = 1; // coefficient for piece count
    double legalmoves = -0.5; // coefficient for number of opponent legal moves
    
    ReversiAI() {
        piececount = 1;
        legalmoves = -0.5;
    }
    
    ReversiAI(double pc, double lm) {
        piececount = pc;
        legalmoves = lm;
    }
    
    ReversiAI(const ReversiAI& ai) {
        piececount = ai.piececount;
        legalmoves = ai.legalmoves;
    }
    
    // This gets the one sided score.
    double getOneSidedScore(ReversiGame game) {
        int pc = 0;
        for (int x = 0; x < game.width; x++) {
            for (int y = 0; y < game.height; y++) {
                if (game.get(x, y) == game.getCur()) pc++;
            }
        }
        
        ReversiGame game2(game);
        game2.sidetomove = !game.sidetomove;
        int legals = game2.getAllLegalMoves().size();
        return pc * piececount + legals * legalmoves;
    }
    
    double getScore(ReversiGame game) {
        double res = getOneSidedScore(game);
        ReversiGame game2(game);
        game2.sidetomove = !game2.sidetomove;
        return res - getOneSidedScore(game2);
    }
    
    std::pair<int, int> chosenmove = {-1, -1};
    int leafcount = 0;
    
    double abprune(ReversiGame game, int rem, double alpha, double beta, bool isMaximizing, int max_cons = 64) {
        if (rem <= 0) {
            leafcount++;
            return getScore(game);
        }
        
        if (isMaximizing) {
            double res = -1 * DBL_MAX;

            std::vector<std::pair<int, int>> legals = game.getAllLegalMoves();
            std::random_shuffle(legals.begin(), legals.end());

            while (legals.size() > max_cons) legals.pop_back();

            for (auto p : legals) {
                ReversiGame game2(game);
                game2.execute(p);
                game2.swapside();

                double value = abprune(game2, rem - 1, alpha, beta, false, max_cons);
                if (value > res) {
                    chosenmove = p;
                    res = value;
                }

                alpha = std::max(alpha, res);
                if (beta <= alpha) break;
            }
            return res;
        }
        else {
            double res = DBL_MAX;
            std::vector<std::pair<int, int>> legals = game.getAllLegalMoves();
            std::random_shuffle(legals.begin(), legals.end());

            while (legals.size() > max_cons) legals.pop_back();

            for (auto p : legals) {
                ReversiGame game2(game);
                game2.execute(p);
                game2.swapside();

                double value = abprune(game2, rem - 1, alpha, beta, true, max_cons);
                if (value < res) {
                    // chosenmove = p;
                    res = value;
                }

                alpha = std::min(alpha, res);
                if (beta <= alpha) break;
            }

            return res;
        }

        return -1;
    }
    
    std::pair<int, int> pick(ReversiGame game, int maxcons = 64) {
        leafcount = 0;
        auto legals = game.getAllLegalMoves();
        if (legals.size() <= 0) return {-1, -1};
        chosenmove = legals[0];
        
        abprune(game, 2, -1 * DBL_MAX, DBL_MAX, true, maxcons);
        return chosenmove;
    }
    
    std::string toString() {
        return "PC " + std::to_string(piececount) + " LM " + std::to_string(legalmoves);
    }
};

bool operator<(const ReversiAI& ai, const ReversiAI& other) {
    if (ai.piececount != other.piececount) return ai.piececount < other.piececount;
    if (ai.legalmoves != other.legalmoves) return ai.legalmoves < other.legalmoves;
    return false;
}

namespace Genetic {
    
double randf() { return (double)(rand()) / (double)(RAND_MAX); } // [0, 1)
double randrad() { return 2 * (randf() - 0.5); } // [-1, 1)

ReversiAI randomAI() {
    ReversiAI ai;
    ai.piececount = randrad() * 2;
    ai.legalmoves = randrad() * 2;
    return ai;
}

ReversiAI mutate(ReversiAI ai) {
    ReversiAI res(ai);
    int piece = rand() % 32;
    if (piece == 0) res.piececount = randrad() * 2;
    if (piece == 1) res.legalmoves = randrad() * 2;
    return res;
}

ReversiAI cross(ReversiAI a1, ReversiAI a2) {
    ReversiAI res(a1);
    if (rand() % 2 == 0) res.piececount = a2.piececount;
    if (rand() % 2 == 0) res.legalmoves = a2.legalmoves;
    
    return res;
}

// WHITE - BLACK
int finalscore(ReversiGame game) {
    int res = 0;
    for (int x = 0; x < game.width; x++) {
        for (int y = 0; y < game.height; y++) {
            if (game.get(x, y) == 1) res++;
            else if (game.get(x, y) == 0) res--;
        }
    }
    return res;
}

int test(ReversiAI ai1, ReversiAI ai2, bool verbose = false, int maxcons = 64) {
    ReversiGame game;
    while (true) {
        if (game.gameover()) break;
        auto move = (game.sidetomove) ? ai1.pick(game, maxcons) : ai2.pick(game, maxcons);
        if (move.first < 0 && move.second < 0) {
            if (game.gameover()) break;
            game.swapside();
            continue;
        }
        
        game.execute(move);
        game.swapside();
        
        if (verbose) game.disp();
    }
    return finalscore(game);
}

std::vector<ReversiAI> tournament(std::vector<ReversiAI> v, bool verbose = false) {
    std::vector<ReversiAI> res;
    
    for (int i = 0; i < v.size() - 1; i += 2) {
        if (i % 8 == 0) std::cout << "\n";
        int beep = test(v[i], v[i + 1], verbose);
        if (beep > 0) res.push_back(v[i]);
        else if (beep < 0) res.push_back(v[i + 1]);
        else res.push_back(v[(rand() % 2 == 0) ? i : i + 1]);
        std::cout << "X";
    }
    std::cout << "\n";

    return res;
}

}

#endif
