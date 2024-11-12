#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <cctype>

using namespace std;

struct Movie {
    string imdb_id;
    string title;
    string plot_synopsis;
    string tags;
    int relevance_score = 0;
};

// Nodo del Trie
struct TrieNode {
    unordered_map<char, shared_ptr<TrieNode>> children;
    vector<shared_ptr<Movie>> movies_with_word;
};

// Clase Trie para insertar y buscar palabras en títulos y sinopsis
class Trie {
public:
    Trie() : root(make_shared<TrieNode>()) {}

    void insert(const Movie &movie) {
        vector<string> words = splitWords(movie.title + " " + movie.plot_synopsis);
        for (const string &word : words) {
            insertWord(word, movie);
        }
    }

    // Búsqueda por palabras y frases
    vector<shared_ptr<Movie>> search(const string &query) const {
        vector<string> words = splitWords(query);
        vector<shared_ptr<Movie>> result;

        for (const string &word : words) {
            vector<shared_ptr<Movie>> movies_with_word = searchWord(word);
            if (result.empty()) {
                result = movies_with_word;
            } else {
                result = intersect(result, movies_with_word);
            }
            if (result.empty()) break;
        }

        sort(result.begin(), result.end(), [](const shared_ptr<Movie>& a, const shared_ptr<Movie>& b) {
            return a->relevance_score > b->relevance_score;
        });

        return result;
    }

    // Búsqueda por tags
    vector<shared_ptr<Movie>> searchByTag(const string &tag) const {
        vector<shared_ptr<Movie>> result;
        for (const auto &movie : movies) {
            if (movie->tags.find(tag) != string::npos) {
                result.push_back(movie);
                movie->relevance_score++; // Incrementar relevancia para tag coincidente
            }
        }
        return result;
    }

private:
    shared_ptr<TrieNode> root;
    vector<shared_ptr<Movie>> movies;

    void insertWord(const string &word, const Movie &movie) {
        shared_ptr<TrieNode> node = root;
        for (char ch : word) {
            ch = tolower(ch);
            if (!node->children[ch]) {
                node->children[ch] = make_shared<TrieNode>();
            }
            node = node->children[ch];
        }
        node->movies_with_word.push_back(make_shared<Movie>(movie));
    }

    vector<shared_ptr<Movie>> searchWord(const string &word) const {
        shared_ptr<TrieNode> node = root;
        for (char ch : word) {
            ch = tolower(ch);
            if (!node->children.count(ch)) {
                return {};
            }
            node = node->children[ch];
        }
        return node->movies_with_word;
    }

    vector<string> splitWords(const string &text) const {
        vector<string> words;
        string word;
        for (char ch : text) {
            if (isalnum(ch)) {
                word += tolower(ch);
            } else if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        if (!word.empty()) {
            words.push_back(word);
        }
        return words;
    }

    vector<shared_ptr<Movie>> intersect(const vector<shared_ptr<Movie>> &v1, const vector<shared_ptr<Movie>> &v2) const {
        vector<shared_ptr<Movie>> intersection;
        for (const auto &movie : v1) {
            if (find_if(v2.begin(), v2.end(), [&](const shared_ptr<Movie> &m) { return m->imdb_id == movie->imdb_id; }) != v2.end()) {
                intersection.push_back(movie);
            }
        }
        return intersection;
    }
};

// Algoritmo de relevancia para filtrar y ordenar resultados
vector<shared_ptr<Movie>> getTopRelevantMovies(const vector<shared_ptr<Movie>>& movies, int topN = 5) {
    vector<shared_ptr<Movie>> sortedMovies = movies;
    sort(sortedMovies.begin(), sortedMovies.end(), [](const shared_ptr<Movie>& a, const shared_ptr<Movie>& b) {
        return a->relevance_score > b->relevance_score;
    });

    if (sortedMovies.size() > topN) {
        sortedMovies.resize(topN);
    }
    return sortedMovies;
}

// Función para leer el archivo CSV y cargar los datos en un vector de películas
vector<Movie> readMoviesFromCSV(const string &filename) {
    vector<Movie> movies;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        return movies;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        Movie movie;

        getline(ss, movie.imdb_id, ',');
        getline(ss, movie.title, ',');
        getline(ss, movie.plot_synopsis, ',');
        getline(ss, movie.tags, ',');

        movies.push_back(movie);
    }

    file.close();
    return movies;
}

int main() {
    string filename = "../mpst_full_data.csv";
    vector<Movie> movies = readMoviesFromCSV(filename);

    Trie movieTrie;
    for (const auto &movie : movies) {
        movieTrie.insert(movie);
    }

    string search_query;
    cout << "Enter a word, phrase, or tag to search: ";
    getline(cin, search_query);

    vector<shared_ptr<Movie>> found_movies = movieTrie.search(search_query);

    if (!found_movies.empty()) {
        auto topMovies = getTopRelevantMovies(found_movies);
        cout << "Top Movies Found:" << endl;
        for (const auto &movie : topMovies) {
            cout << "Title: " << movie->title << endl;
            cout << "Plot Synopsis: " << movie->plot_synopsis << endl;
            cout << "Relevance Score: " << movie->relevance_score << endl;
            cout << "-----------------------" << endl;
        }
    } else {
        cout << "No movies found matching the query." << endl;
    }

    return 0;
}
