/**
 * Mutex protected class containing global variables.
 */

#ifndef SERVER_HIGHESTSCORE_H
#define SERVER_HIGHESTSCORE_H

class HighestScore {
public:
    HighestScore();

    double wins;
    double gamesPlayed;
    double winRate;
};

HighestScore::HighestScore() {
    winRate = 0.0;
    gamesPlayed = 0;
    wins = 0;
}


#endif //SERVER_HIGHESTSCORE_H
