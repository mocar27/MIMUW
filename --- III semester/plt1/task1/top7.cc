// Task 1: Top7
// Authors: Konrad Mocarski & Pawel Dec

#include <iostream>
#include <string>
#include <regex>
#include <unordered_map>
#include <vector>
#include <set>
#include <tuple>
#include <sstream>
#include <algorithm>

using namespace std;

#define MIN_TRACK 1
#define MAX_TRACK 99999999
#define NUMBER_LENGTH 9
#define VECTOR_CAP 7

vector<size_t> extractIntegerWords(string &str) {

    stringstream sstream;
    vector<size_t> v;
    string s;
    size_t val;
    sstream << str;

    while (!sstream.eof()) {

        sstream >> s;
        if (stringstream(s) >> val) {
            if (val >= MIN_TRACK && val <= MAX_TRACK)
                v.push_back(val);
            else {
                v.clear();
                return v;
            }
        } else if (s.length() >= NUMBER_LENGTH) {
            v.clear();
            return v;
        }
        s = "";
    }

    return v;
}

tuple<char, string, vector<size_t>> readLine(regex &b, regex &n,
                                             regex &t, regex &num) {

    string s;
    char c = ' ';
    vector<size_t> v;

    if (getline(cin, s)) {

        if (regex_match(s, b)) {
            c = 'B';
        } else if (regex_match(s, n)) {
            c = 'N';
            v = extractIntegerWords(s);
            if (v.empty()) {
                c = 'E';
            }

        } else if (regex_match(s, t)) {
            c = 'T';

        } else if (regex_match(s, num)) {
            c = 'G';
            v = extractIntegerWords(s);
            if (v.empty()) {
                c = 'E';
            }

        } else {
            c = 'E';
        }

    } else {
        c = 'F';
    }

    return {c, s, v};
}

bool comparator(pair<size_t, size_t> e1, pair<size_t, size_t> e2) {

    if (e1.second == e2.second) {
        return e1.first < e2.first;
    }

    return e1.second > e2.second;
}

void addVote(unordered_map<size_t, size_t> &currentRec, vector<size_t> &v) {

    for (size_t i = 0; i < v.size(); i++) {
        currentRec[v[i]] += 1;
    }
}

// Function that checks the correctness of the cast vote.
bool validG(set<size_t> &t, vector<size_t> &v, size_t maxVal) {

    sort(v.begin(), v.end());

    for (size_t i = 0; i < v.size(); i++) {
        if (v[i] > maxVal || t.count(v[i])) {
            return false;
        }
    }

    for (size_t i = 1; i < v.size(); i++) {
        if (v[i] == v[i - 1]) {
            return false;
        }
    }

    return true;
}

void newRecord(size_t &lastMax, size_t newMax, vector<size_t> &lastN,
               unordered_map<size_t, size_t> &currentRec,
               set<size_t> &t, unordered_map<size_t, size_t> &totalP) {

    if (lastMax == 0) {
        lastMax = newMax;
        return;
    }

    vector<pair<size_t, size_t>> temp(currentRec.begin(), currentRec.end());
    sort(temp.begin(), temp.end(), comparator);

    vector<pair<size_t, int>> result;

    for (size_t i = 0; i < temp.size() && i < VECTOR_CAP; i++) {
        result.push_back(make_pair(temp[i].first, VECTOR_CAP));
    }

    if (!lastN.empty()) {

        for (size_t i = 0; i < result.size() && i < VECTOR_CAP; i++) {

            size_t track = result[i].first;

            for (size_t j = 0; j < lastN.size(); j++) {
                if (lastN[j] == track) {
                    result[i].second = j - i;
                }
            }
        }

        /* If one of the previous tracks was not in the closing Top 7
         * it is deleted from set and cannot be voted again. */
        for (size_t i = 0; i < lastN.size(); i++) {

            size_t track = lastN[i];
            bool occurred = false;

            for (size_t j = 0; j < result.size(); j++) {
                if (result[j].first == track) {
                    occurred = true;
                }
            }

            if (!occurred) {
                t.insert(track);
            }
        }

        lastN.clear();
    }

    int points = 7;
    for (size_t i = 0; i < result.size(); i++) {

        // Summarizing points from the closing record.
        lastN.push_back(result[i].first);
        totalP[result[i].first] += points;
        points--;

        if (result[i].second == VECTOR_CAP) {
            cout << result[i].first << " -\n";
        } else {
            cout << result[i].first << " " << result[i].second << "\n";
        }
    }

    currentRec.clear();
    lastMax = newMax;
}

void printTop(vector<size_t> &lastT, unordered_map<size_t, size_t> &totalP) {

    vector<pair<size_t, size_t>> temp(totalP.begin(), totalP.end());
    sort(temp.begin(), temp.end(), comparator);

    vector<pair<size_t, int>> result;

    for (size_t i = 0; i < temp.size() && i < VECTOR_CAP; i++) {
        result.push_back(make_pair(temp[i].first, VECTOR_CAP));
    }

    if (!lastT.empty()) {

        for (size_t i = 0; i < result.size() && i < VECTOR_CAP; i++) {

            size_t track = result[i].first;

            for (size_t j = 0; j < lastT.size(); j++) {
                if (lastT[j] == track) {
                    result[i].second = j - i;
                }
            }
        }

        lastT.clear();
    }

    for (size_t i = 0; i < result.size(); i++) {

        lastT.push_back(result[i].first);

        if (result[i].second == VECTOR_CAP) {
            cout << result[i].first << " -\n";
        } else {
            cout << result[i].first << " " << result[i].second << "\n";
        }
    }
}

void printError(size_t errorLine, string &s) {
    cerr << "Error in line " << errorLine << ": " << s << "\n";
}

int main() {

    regex b = regex("^\\s{0,}");
    regex n = regex("^\\s{0,}NEW\\s{1,}\\d+\\s{0,}");
    regex t = regex("^\\s{0,}TOP\\s{0,}");
    regex num = regex("^\\s{0,}\\d+(\\s{1,}\\d{0,}){0,}");

    unordered_map<size_t, size_t> totalPoints;
    unordered_map<size_t, size_t> currentRecord;
    vector<size_t> lastTop;
    vector<size_t> lastNew;
    set<size_t> tracksOutOfRecords;

    size_t errorLine = 1;
    size_t currentMax = 0;
    size_t newMax = 0;
    char operation = ' ';

    /* Program reads every line and filter it with regex and then returns
     * a tuple object with char, string containing whole data line
     * and vector of numbers - that are votes for tracks in the current record.
     * Where, for char it is:
     * 'B' - blank line
     * 'F' - end of data | finish program
     * 'N' - NEW MAX operation
     * 'T' - TOP operation
     * 'E' - error in given data
     * 'G' - vector of numbers that need to be validated. */

    while (operation != 'F') {

        tuple<char, string, vector<size_t>> line = readLine(b, n, t, num);
        operation = get<char>(line);
        string s = get<string>(line);
        vector<size_t> vote = get<vector<size_t>>(line);

        if (operation == 'N') {

            newMax = vote[0];
            if (newMax < currentMax) {
                operation = 'E';
            }
        }

        if (operation == 'G') {
            if (!validG(tracksOutOfRecords, vote, currentMax)) {
                operation = 'E';
            }
        }

        switch (operation) {

            case 'B':
            case 'F':
                break;

            case 'E':
                printError(errorLine, s);
                break;

            case 'T':
                printTop(lastTop, totalPoints);
                break;

            case 'N':
                newRecord(currentMax, newMax, lastNew,
                          currentRecord, tracksOutOfRecords, totalPoints);
                break;

            default:
                addVote(currentRecord, vote);
        }

        errorLine++;
    }
}