#ifndef WORLDCUP2022_H
#define WORLDCUP2022_H

#include <memory>
#include <utility>
#include <vector>

#include "worldcup.h"

class TooManyDiceException : public std::exception {
};

class TooFewDiceException : public std::exception {
};

class TooManyPlayersException : public std::exception {
};

class TooFewPlayersException : public std::exception {
};

class Player {
private:
    std::string name;
    unsigned int money;
    unsigned int position;
    unsigned int waiting;
    bool inGame;

public:
    Player(std::string name) :
            name(std::move(name)),
            money(1000),
            position(0),
            waiting(0),
            inGame(true) {}

    virtual ~Player() = default;

    void addMoney(unsigned int m) {
        if (inGame) {
            this->money += m;
        }
    }

    unsigned int subMoney(unsigned int m) {
        if (this->money < m) {
            inGame = false;
            unsigned int tmp = this->money;
            this->money = 0;
            return tmp;
        } else {
            this->money -= m;
            return m;
        }
    }

    const std::string &getName() const {
        return name;
    }

    bool isPlaying() const {
        return inGame;
    }

    unsigned int getMoney() const {
        return money;
    }

    unsigned int getPosition() const {
        return position;
    }

    void setPosition(unsigned int p) {
        this->position = p;
    }

    void giveYellowCard(unsigned int turns) {
        waiting += turns;
    }

    void wait() {
        if (waiting > 0) {
            waiting--;
        }
    }

    bool isWaiting() const {
        return waiting > 0;
    }

    static void checkPlayers(size_t amount) {
        if (amount > 11) {
            throw TooManyPlayersException();
        }
        if (amount < 2) {
            throw TooFewPlayersException();
        }
    }

    const std::string status() const {
        if (isPlaying() && isWaiting()) {
            return "*** czekanie: " + std::to_string(waiting) + " ***";
        } else if (isPlaying()) {
            return "w grze";
        } else {
            return "*** bankrut ***";
        }
    }
};

class Field {
protected:
    const std::string name;
    int zdzislaws;

public:
    Field(std::string nameOfField, int actionZdzislaws = 0) :
            name(std::move(nameOfField)),
            zdzislaws(actionZdzislaws) {}

    virtual ~Field() = default;

    virtual void onEnter([[maybe_unused]] Player &player) {};

    virtual void onPassing([[maybe_unused]] Player &player) {};

    const std::string &getName() const {
        return name;
    }
};

class FieldSeasonStart : public Field {
public:
    FieldSeasonStart(const std::string &nameOfField, int actionZdzislaws) :
            Field(nameOfField, actionZdzislaws) {}

    void onEnter(Player &player) override {
        player.addMoney(zdzislaws);
    }

    void onPassing(Player &player) override {
        player.addMoney(zdzislaws);
    }
};

class FieldGoal : public Field {
public:
    FieldGoal(const std::string &nameOfField, int actionZdzislaws) :
            Field(nameOfField, actionZdzislaws) {}

    void onEnter(Player &player) override {
        player.addMoney(zdzislaws);
    }
};

class FieldPenalty : public Field {
public:
    FieldPenalty(const std::string &nameOfField, int actionZdzislaws) :
            Field(nameOfField, actionZdzislaws) {}

    void onEnter(Player &player) override {
        player.subMoney(zdzislaws);
    }
};

class FieldBookmaker : public Field {
private:
    int bets{};

public:
    FieldBookmaker(const std::string &nameOfField, int actionZdzislaws) :
            Field(nameOfField, actionZdzislaws) {}

    void onEnter(Player &player) override {
        bets = (bets + 1) % 3;
        if (bets == 1) {
            player.addMoney(zdzislaws);
        } else {
            player.subMoney(zdzislaws);
        }
    }
};

class FieldYellow : public Field {
private:
    int breakTime;

public:
    FieldYellow(const std::string &nameOfField, int turns)
            : Field(nameOfField), breakTime(turns) {}

    void onEnter(Player &player) override { player.giveYellowCard(breakTime); }
};

class FieldMatch : public Field {
private:
    float fifaWeight;
    float sumOfPayments;

public:
    enum match_type {
        FRIENDLY, POINTS_MATCH, FINAL
    };

    FieldMatch(const std::string &nameOfField, int actionZdzislaws, match_type matchType) :
            Field(nameOfField, actionZdzislaws),
            sumOfPayments(0) {
        switch (matchType) {
            case FRIENDLY:
                fifaWeight = 1;
                break;
            case POINTS_MATCH:
                fifaWeight = 2.5;
                break;
            case FINAL:
                fifaWeight = 4;
                break;
        }
    }

    void onEnter(Player &player) override {
        player.addMoney((unsigned int) (sumOfPayments * fifaWeight));
        sumOfPayments = 0;
    };

    void onPassing(Player &player) override {
        sumOfPayments += (float) player.subMoney(zdzislaws);
    }
};

class FieldDayOff : public Field {
public:
    FieldDayOff(const std::string &nameOfField) :
            Field(nameOfField) {}
};

class Board {
private:
    std::vector<std::shared_ptr<Field>> fields;

public:
    Board() :
            fields() {}

    void addField(const std::shared_ptr<Field> &field) {
        fields.emplace_back(field);
    }

    void onPassing(Player &player, unsigned int field) {
        fields[field]->onPassing(player);
    }

    void OnEnter(Player &player, unsigned int field) {
        fields[field]->onEnter(player);
    }

    size_t fieldsCount() const {
        return fields.size();
    }

    const std::string &fieldName(unsigned int field) const {
        return fields[field]->getName();
    }
};

class Dices {
private:
    std::vector<std::shared_ptr<Die>> dices;
public:
    Dices() = default;

    virtual ~Dices() = default;

    void addDie(const std::shared_ptr<Die> &die) {
        if (die == nullptr) {
            return;
        }
        dices.emplace_back(die);
    }

    void checkDices() {
        if (dices.size() > 2) {
            throw TooManyDiceException();
        }
        if (dices.size() < 2) {
            throw TooFewDiceException();
        }
    }

    int roll() {
        return dices[0]->roll() + dices[1]->roll();
    }
};

class WorldCup2022 : public WorldCup {
private:
    Dices dices;
    std::vector<Player> players;
    std::shared_ptr<ScoreBoard> scoreboard;
    unsigned int round;
    Board board;

public:
    WorldCup2022() :
            dices(),
            players(),
            scoreboard(nullptr),
            round(0),
            board() {}

    void addDie(std::shared_ptr<Die> die) override {
        dices.addDie(die);
    }

    void addPlayer(std::string const &name) override {
        players.emplace_back(name);
    }

    void setScoreBoard(std::shared_ptr<ScoreBoard> sb) override {
        scoreboard = sb;
    }

    void setBoard() {
        board.addField(
                std::make_shared<FieldSeasonStart>("Początek sezonu", 50));
        board.addField(std::make_shared<FieldMatch>("Mecz z San Marino", 160,
                                                    FieldMatch::FRIENDLY));
        board.addField(
                std::make_shared<FieldDayOff>("Dzień wolny od treningu"));
        board.addField(std::make_shared<FieldMatch>("Mecz z Lichtensteinem",
                                                    220, FieldMatch::FRIENDLY));
        board.addField(std::make_shared<FieldYellow>("Żółta kartka", 3));
        board.addField(std::make_shared<FieldMatch>("Mecz z Meksykiem", 300,
                                                    FieldMatch::POINTS_MATCH));
        board.addField(std::make_shared<FieldMatch>(
                "Mecz z Arabią Saudyjską", 280, FieldMatch::POINTS_MATCH));
        board.addField(std::make_shared<FieldBookmaker>("Bukmacher", 100));
        board.addField(std::make_shared<FieldMatch>("Mecz z Argentyną", 250,
                                                    FieldMatch::POINTS_MATCH));
        board.addField(std::make_shared<FieldGoal>("Gol", 120));
        board.addField(std::make_shared<FieldMatch>("Mecz z Francją", 400,
                                                    FieldMatch::FINAL));
        board.addField(std::make_shared<FieldPenalty>("Rzut karny", 180));
    }

    void play(unsigned int rounds) override {
        dices.checkDices();
        Player::checkPlayers(players.size());
        setBoard();
        size_t playing = players.size();

        for (unsigned int i = 0; i < rounds && playing > 1; i++) {
            if (scoreboard != nullptr) {
                scoreboard->onRound(round);
            }

            for (auto &player: players) {
                if (playing == 1) {
                    size_t it = 0;
                    while (!players[it].isPlaying()) {
                        it++;
                    }
                    if (scoreboard != nullptr) {
                        scoreboard->onWin(players[it].getName());
                    }
                    return;
                }
                if (player.isPlaying()) {
                    if (!player.isWaiting()) {
                        int moves = dices.roll();

                        for (int j = 1; (j < moves) && player.isPlaying(); j++) {
                            board.onPassing(player, (player.getPosition() + j) % board.fieldsCount());
                        }
                        if (player.isPlaying()) {
                            board.OnEnter(player, (player.getPosition() + moves) % board.fieldsCount());
                        } else {
                            playing--;
                        }
                        player.setPosition((player.getPosition() + moves) % board.fieldsCount());
                    }
                    if (scoreboard != nullptr) {
                        scoreboard->onTurn(player.getName(), player.status(),
                                           board.fieldName(player.getPosition()),
                                           player.getMoney());
                    }
                    if (player.isWaiting()) {
                        player.wait();
                    }
                }
            }
            round++;
        }
        if (playing == 1) {
            size_t it = 0;
            while (!players[it].isPlaying()) {
                it++;
            }
            if (scoreboard != nullptr) {
                scoreboard->onWin(players[it].getName());
            }
        } else {
            Player winner = players[0];
            for (auto &player: players) {
                if (player.getMoney() > winner.getMoney()) {
                    winner = player;
                }
            }
            if (scoreboard != nullptr) {
                scoreboard->onWin(winner.getName());
            }
        }
    }
};

#endif  // WORLDCUP2022_H