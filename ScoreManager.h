#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

class ScoreManager {
private:
    int score;
public:
    ScoreManager() : score(0) {}
    void addScore(int amount) { score += amount; }
    int getScore() const { return score; }
    void resetScore() { score = 0; }
};

#endif // SCORE_MANAGER_H 