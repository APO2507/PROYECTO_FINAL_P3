#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>

using namespace std;

struct Movie {
    string imdb_id;
    string title;
    string plot_synopsis;
    string tags;
    string split;
    string synopsis_source;
};

// Nodo del Trie
struct TrieNode {
    unordered_map<char, shared_ptr<TrieNode>> children;
    vector<shared_ptr<Movie>> movies_with_word; // Lista de películas que contienen este prefijo
};


// Clase Trie para insertar y buscar palabras en títulos y sinopsis
class Trie {
public:
    Trie() : root(make_shared<TrieNode>()) {}

    // Método para insertar palabras del título y sinopsis en el Trie
    void insert(const Movie &movie) {
        vector<string> words = splitWords(movie.title + " " + movie.plot_synopsis);
        for (const string &word : words) {
            insertWord(word, movie);
        }
    }

    // Búsqueda de películas que contienen todas las palabras de la consulta en el título o sinopsis
    vector<shared_ptr<Movie>> search(const string &query) const {
        vector<string> words = splitWords(query);
        vector<shared_ptr<Movie>> result;

        for (const string &word : words) {
            vector<shared_ptr<Movie>> movies_with_word = searchWord(word);
            if (result.empty()) {
                result = movies_with_word; // Inicializar con los resultados de la primera palabra
            } else {
                result = intersect(result, movies_with_word); // Mantener solo las películas que cumplen con todas las palabras
            }
            if (result.empty()) break; // Si no quedan coincidencias, no continua buscando
        }

        return result;
    }

private:
    shared_ptr<TrieNode> root;

    // Insertar una sola palabra en el Trie y enlazarla con la película
    void insertWord(const string &word, const Movie &movie) {
        shared_ptr<TrieNode> node = root;
        for (char ch : word) {
            if (!node->children[ch]) {
                node->children[ch] = make_shared<TrieNode>();
            }
            node = node->children[ch];
        }
        node->movies_with_word.push_back(make_shared<Movie>(movie));
    }

    // Buscar películas que contienen una palabra específica en el Trie
    vector<shared_ptr<Movie>> searchWord(const string &word) const {
        shared_ptr<TrieNode> node = root;
        for (char ch : word) {
            if (!node->children.count(ch)) {
                return {}; // No se encontró la palabra
            }
            node = node->children[ch];
        }
        return node->movies_with_word; // Devolver películas que contienen esta palabra
    }

    // Dividir una cadena en palabras
    vector<string> splitWords(const string &text) const {
        vector<string> words;
        string word;
        istringstream stream(text);
        while (stream >> word) {
            words.push_back(word);
        }
        return words;
    }

    // Encontrar la intersección de dos vectores de punteros a películas
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


// Función para leer el archivo CSV y cargar los datos en un vector de películas
vector<Movie> readMoviesFromCSV(const string &filename) {
    vector<Movie> movies;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        return movies;
    }

    string line;
    // Saltar la primera línea con encabezados
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        Movie movie;

        getline(ss, movie.imdb_id, ',');
        getline(ss, movie.title, ',');
        getline(ss, movie.plot_synopsis, ',');
        getline(ss, movie.tags, ',');
        getline(ss, movie.split, ',');
        getline(ss, movie.synopsis_source, ',');

        movies.push_back(movie);
    }

    file.close();
    return movies;
}

int main() {
    //ajusten la ruta a su computadora
    string filename = "../mpst_full_data.csv";
    vector<Movie> movies = readMoviesFromCSV(filename);

    // Crear el Trie e insertar todas las películas
    Trie movieTrie;
    for (const auto &movie : movies) {
        movieTrie.insert(movie);
    }

    //busqueda
    string search_query;
    cout << "Enter a word or phrase to search: ";
    getline(cin, search_query);

    vector<shared_ptr<Movie>> found_movies = movieTrie.search(search_query);
    if (!found_movies.empty()) {
        cout << "Movies found:" << endl;
        for (const auto &movie : found_movies) {
            cout << "Title: " << movie->title << endl;
            cout << "Plot Synopsis: " << movie->plot_synopsis << endl;
            cout << "-----------------------" << endl;
        }
    } else {
        cout << "No movies found matching the query." << endl;
    }
    return 0;
}