#include <iostream>
#include <codecvt>
#include <fstream>
#include <locale>
#include <string>
#include <vector>
#include <thread>
#include <future>

const int N_THREADS = 3;

int grep(std::string filename, std::wstring word) {
    std::wfstream file(filename);
    std::locale loc("pl_PL.UTF-8");
    file.imbue(loc);
    file.exceptions(std::wfstream::badbit);

    std::wstring line;
    int count = 0;
    while (getline(file, line)) {
        for (auto pos = line.find(word, 0);
            pos != std::string::npos;
            pos = line.find(word, pos + 1)) {
            count++;
        }
    }
    return count;
}

void set_promise_to_grep(std::promise<int>& p, std::vector<std::string> filenames, std::wstring word) {
    int result = 0;
    for (auto& f : filenames) {
        result += grep(f, word);
    }
    p.set_value(result);
}

int main() {

    std::locale loc("pl_PL.UTF-8");
    std::wcout.imbue(loc);
    std::wcin.imbue(loc);
    std::wcout.exceptions(std::wfstream::badbit);
    std::wcin.exceptions(std::wfstream::badbit);

    std::wstring word;
    std::getline(std::wcin, word);

    std::wstring s_file_count;
    std::getline(std::wcin, s_file_count);
    int file_count = std::stoi(s_file_count);

    std::vector<std::vector<std::string>> filenames(N_THREADS);
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

    for (int file_num = 0; file_num < file_count; file_num++) {
        std::wstring w_filename;
        std::getline(std::wcin, w_filename);
        std::string s_filename = converter.to_bytes(w_filename);
        filenames[file_num % N_THREADS].push_back(s_filename);
    }

    std::vector<std::promise<int>> promises;
    std::vector<std::future<int>> futures;
    std::vector<std::thread> threads;

    for (int i = 0; i < N_THREADS; i++) {
        promises.push_back(std::promise<int>());
        futures.push_back(promises[i].get_future());
    }

    for (int i = 0; i < N_THREADS; i++) {
        std::vector<std::string>& f = filenames[i];
        std::promise<int>& p = promises[i];
        std::thread t(set_promise_to_grep, std::ref(p), std::ref(f), std::ref(word));
        threads.push_back(std::move(t));
    }

    int sum = 0;
    for (auto& f : futures) {
        sum += f.get();
        std::cout << sum << std::endl;
    }

    std::wcout << sum << std::endl;

    for (auto& t : threads) {
        t.join();
    }
}