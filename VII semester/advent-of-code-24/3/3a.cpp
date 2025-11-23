#include <iostream>
#include <string>
#include <regex>

using namespace std;

int main() {
    string text;
    regex r("mul\\(\\d{1,3},\\d{1,3}\\)");
    long long result = 0;

    while (getline(cin, text)) {
        string::const_iterator searchStart(text.cbegin());
        smatch matches;

        while (regex_search(searchStart, text.cend(), matches, r)) {
            string match = matches[0];

            regex numberRegex("\\d{1,3}");
            smatch numberMatch;
            string::const_iterator numberStart(match.cbegin());

            int x = 0, y = 0, index = 0;

            // search for numbers to extract from mul(x, y) match
            while (regex_search(numberStart, match.cend(), numberMatch, numberRegex)) {
                if (index == 0) { x = stoi(numberMatch[0]); }
                else if (index == 1) { y = stoi(numberMatch[0]); }
                index++;

                numberStart = numberMatch.suffix().first; // Move to the next number match in the mul(x, y) match
            }

            result += x * y;
            searchStart = matches.suffix().first; // Move past this match and search for another mul(x, y)
        }
    }

    cout << result << endl;
    return 0;
}
