// take two tsv files representing spreadsheets
// and produce a differential report

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

using namespace std;

void abort(const std::string& msg) {
    cerr << "abort: " << msg << endl;
    exit(1);
}

bool is_number(const std::string& token) {
    return std::regex_match(token, std::regex(("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?")));
}

double to_double(const string& s) {
    stringstream ss;
    ss << s;
    double d;
    ss >> d;
    return d;
}

int main(int argc, char** argv) {
    if (argc != 3)
        abort("usage: bench-compare tsv-file tsv-file");

    // open 0
    fstream s_in_0;
    s_in_0.open(argv[1], fstream::in);
    if (s_in_0.fail())
        abort("could not open first file");

    // open 1
    fstream s_in_1;
    s_in_1.open(argv[2], fstream::in);
    if (s_in_1.fail())
        abort("could not open second file");

    // read loop
    char buf_0[1024];
    char buf_1[1024];
    while (!s_in_0.eof()) {
        if (s_in_0.fail())
            abort("first file failure");
        if (s_in_1.fail())
            abort("second file failure");
        if (s_in_1.eof())
            abort("file formats do not match");

        // read 0
        s_in_0.getline(buf_0, sizeof(buf_0));
        if (s_in_0.fail())
            break;

        // read 1
        s_in_1.getline(buf_1, sizeof(buf_1));
        if (s_in_1.fail())
            break;

        if (s_in_0.eof() != s_in_1.eof())
            abort("file formats do not match");

        // splitting regex
        regex reg_tab("\\t+");

        // tokenize 0
        string line_0(buf_0);
        sregex_token_iterator iter_0(line_0.begin(), line_0.end(), reg_tab, -1);
        sregex_token_iterator end_0;
        vector<string> vec_0(iter_0, end_0);

        // tokenize 1
        string line_1(buf_1);
        sregex_token_iterator iter_1(line_1.begin(), line_1.end(), reg_tab, -1);
        sregex_token_iterator end_1;
        vector<string> vec_1(iter_1, end_1);

        // cursor over tokens
        vector<string>::iterator cursor_0 = vec_0.begin();
        vector<string>::iterator cursor_1 = vec_1.begin();
        while (cursor_0 != vec_0.end()) {
            if (cursor_1 == vec_1.end())
                abort("file formats do not match");

            bool is_num_0 = is_number(*cursor_0);
            bool is_num_1 = is_number(*cursor_1);

            if (is_num_0 != is_num_1)
                abort("file formats do not match");

            if (!is_num_0) {
                // we have strings here
                if (*cursor_0 == *cursor_1) {
                    // and they are the same
                    cout << *cursor_0 << "\t";
                }
                else {
                    // they have differed
                    cout << *cursor_0 << " -> " << *cursor_1;
                }
            }
            else {
                // parse numbers into doubles
                double d_0 = to_double(*cursor_0);
                double d_1 = to_double(*cursor_1);

                // don't divide by zero
                if (d_1 == 0.0) {
                    if (d_0 == 0.0) {
                        cout << "0\t";
                    }
                    else {
                        cout << "NAN\t";
                    }
                }
                else {
                    // divide for differential
                    cout << d_0 / d_1 << "\t";
                }
            }

            // move on
            cursor_0++;
            cursor_1++;
        }
        // next line
        cout << endl;
    }
}
