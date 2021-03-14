#include <bits/stdc++.h>
using namespace std;

unordered_set<string> dictionary;

void readDictionary(string filename) {
    ifstream file(filename);
    string word;
    while (file >> word) {
        transform(word.begin(), word.end(), word.begin(), ::tolower); 
        dictionary.insert(word);
    }
    file.close();
}

double validateEnglishText(vector<string> txt, bool printUnknownWords) {
    int validWords = 0;
    for (string word : txt) {
        if (dictionary.find(word) != dictionary.end())
            validWords++;
        else if (printUnknownWords)
            cout << "unknown: '" << word << "'\n";
    }
    return (double)validWords / txt.size();
}


vector<string> twoLetterWords = { "of", "to", "in", "it", "is", "be", "as", "at", "so", "we", "he", "by", "or", "on", "do", "if", "me", "my", "up", "an", "go", "no", "us", "am" };
vector<string> threeLetterWords = {"the","and","for","are","but","not","you","all","any","can","had","her","was","one","our","out","day","get","has","him","his","how","man","new","now","old","see","two","way","who","boy","did","its","let","put","say","she","too","use"};

template<typename T>
vector<T> sortMapByFreq(map<T, int>& freq) {
    vector<T> res;
    for (auto pair : freq)
        res.push_back(pair.first);
    sort(res.begin(), res.end(), [&](auto& x, auto& y) {
        return freq[x] > freq[y];
    });
    return res;
}

void substitute(vector<string>& cipher, vector<string> plain, unordered_map<char,char> mapping) {
    static double maxsf = 0;
    for (int i = 0; i < (int)cipher.size(); i++) {
        for (int j = 0; j < (int)cipher[i].length(); j++)
            if (mapping.find(cipher[i][j]) != mapping.end())
                plain[i][j] = mapping[cipher[i][j]];
    }
    double valid = validateEnglishText(plain, false);
    if (valid > maxsf) {
        maxsf = valid;
        cout << valid << '\n';
        //for (string word : plain)
        //    cout << word << '\n';
    }
}

void solve(vector<string> &cipher) {
    vector<string> result;
    unordered_map<char, char> mapping;
    map<char, int> freq;
    vector<map<string, int>> shortWords(4);
    vector<string> plain;

    for (string word : cipher) {
        for (char c : word)
            ++freq[c];    
        if (word.length() <= 3)
            ++shortWords[word.length()][word];
        plain.push_back(string(word.length(), '*'));
    }

    vector<string> words3 = sortMapByFreq(shortWords[3]);
    vector<string> words2 = sortMapByFreq(shortWords[2]);
    vector<string> words1 = sortMapByFreq(shortWords[1]);
    vector<char> guessedLetters = sortMapByFreq(freq);

    for (int i = 0; i < (int)words3.size(); i++) {
        for (int j = 0; j < (int)words3.size(); j++) {
            unordered_map<char,char> mapping;
            mapping['p']='a'; 
            for (int k = 0; k < min((int)words3.size(), (int)threeLetterWords.size()); k++) {
                int l = 0;
                for (char c : words3[(i+j) % words3.size()])
                    if (mapping.find(c) == mapping.end())
                        mapping[c] = threeLetterWords[k][l++];
            }
            for (int k = 0; k < (int)words2.size(); k++) {
                for (int l = 0; l < (int)words2.size(); l++) {
                    vector<char> mapping2;
                    for (int p = 0; p < min((int)words2.size(), (int)twoLetterWords.size()); p++) {
                        int q = 0;
                        for (char c : words2[(k+l) % words2.size()])
                            if (mapping.find(c) == mapping.end()) {
                                mapping[c] = twoLetterWords[p][q];
                                mapping2.push_back(twoLetterWords[p][q]);
                                q++;
                            }
                    }
                    substitute(cipher, plain, mapping);
                    for (char c : mapping2)
                        mapping.erase(c);
                }
            }
        }
    }
}

void run(string inputFile) {
    vector<string> cipher;
    ifstream file(inputFile);
    string word = "";
    char c;

    while (file.get(c)) {
        if (c == '\n' || c == '\r')
            continue;
        if (c == '#') {
            solve(cipher);
            break;
        }
        if (isspace(c)) {
            if (word != " " && word != "")
                cipher.push_back(word);
            word = "";
        } else
            word += c;
    }
}

int main() {
    readDictionary("words/01.txt");
    run("test.txt");
    return 0;
}