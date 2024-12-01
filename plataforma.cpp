#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <memory>
#include <algorithm>
#include <unordered_set>

using namespace std;

struct Movie {
    string imdb_id;
    string title;
    string plot_synopsis;
    string tags;
    mutable int relevance_score = 0;
    string split;
    string synopsis_source;
    bool liked = false;           // Marca si la película recibió un "Like"
    bool watch_later = false;     // Marca si la película fue añadida a "Ver más tarde"
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

    void insert(const shared_ptr<Movie> &movie) {
        vector<string> words = splitWords(movie->title + " " + movie->plot_synopsis);
        for (const string &word : words) {
            insertWord(word, movie, 1);
        }
    }

    // Búsqueda por palabras y frases
    vector<shared_ptr<Movie>> search(const string &query) {
        for (auto &movie : movies) {
            movie->relevance_score = 0; // Reseteamos siempre el relevance para cada película
        }

        vector<string> words = splitWords(query);
        vector<shared_ptr<Movie>> result;

        set<string> uniqueMovies; // Usamos el imdb_id para evitar duplicados
        for (const string &word : words) {
            vector<shared_ptr<Movie>> movies_with_word = searchWord(word);
            for (auto &movie : movies_with_word) {
                if (uniqueMovies.find(movie->imdb_id) == uniqueMovies.end()) {
                    result.push_back(movie);
                    uniqueMovies.insert(movie->imdb_id);
                }
                movie->relevance_score++; // Incrementamos el relevance por cada coincidencia
            }
        }

        sort(result.begin(), result.end(), [](const shared_ptr<Movie> &a, const shared_ptr<Movie> &b) {
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

    void insertWord(const string &word, const shared_ptr<Movie> &movie, int score) {
        shared_ptr<TrieNode> node = root;
        for (char ch : word) {
            ch = tolower(ch);
            if (!node->children[ch]) {
                node->children[ch] = make_shared<TrieNode>();
            }
            node = node->children[ch];
        }
        node->movies_with_word.push_back(movie);
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
};

// Algoritmo de relevancia para filtrar y ordenar resultados
vector<shared_ptr<Movie>> getTopRelevantMovies(const vector<shared_ptr<Movie>> &movies, int topN = 5) {
    vector<shared_ptr<Movie>> sortedMovies = movies;
    sort(sortedMovies.begin(), sortedMovies.end(), [](const shared_ptr<Movie> &a, const shared_ptr<Movie> &b) {
        return a->relevance_score > b->relevance_score;
    });

    if (sortedMovies.size() > topN) {
        sortedMovies.resize(topN);
    }
    return sortedMovies;
}

class PlataformaStreaming {
private:
    vector<shared_ptr<Movie>> movies;      // Todas las películas cargadas
    vector<shared_ptr<Movie>> watchLater; // Películas marcadas como "Ver más tarde"
    vector<shared_ptr<Movie>> likedMovies; // Películas marcadas con "Like"

public:
    void agregarPelicula(const shared_ptr<Movie> &movie) {
        movies.push_back(movie);
    }

    void mostrarPeliculasGuardadas() {
        cout << "Peliculas añadidas a 'Ver más tarde':\n";
        for (const auto &movie : watchLater) {
            cout << "Titulo: " << movie->title << "\n";
            cout << "Sinopsis: " << movie->plot_synopsis << "\n\n";
        }
    }

    void mostrarPeliculasSimilares() {
        cout << "Películas similares a las que diste 'Like':\n";
        for (const auto &likedMovie : likedMovies) {
            cout << "Titulo: " << likedMovie->title << " (Pelicula similar)\n";
            cout << "Sinopsis: " << likedMovie->plot_synopsis << "\n\n";
        }
    }

    void mostrarResultadosBusqueda(const vector<shared_ptr<Movie>> &results, int offset) {
        int limite = 5;
        int start = offset * limite;
        int end = min(start + limite, (int)results.size());

        cout << "Mostrando peliculas " << start + 1 << " a " << end << ":\n";
        for (int i = start; i < end; i++) {
            cout << i + 1 << ". Título: " << results[i]->title << "\n";
            cout << "Sinopsis: " << results[i]->plot_synopsis << "\n";
            cout << "Relevance Score: " << results[i]->relevance_score << "\n";
            cout << "-----------------------" << "\n\n";
        }
    }

    void marcarLike(const shared_ptr<Movie> &movie) {
        movie->liked = true;
        likedMovies.push_back(movie);
        cout << "Pelicula marcada con 'Like'.\n";
    }

    void marcarVerMasTarde(const shared_ptr<Movie> &movie) {
        movie->watch_later = true;
        watchLater.push_back(movie);
        cout << "Pelicula añadida a 'Ver más tarde'.\n";
    }

    void mostrarResultadosBusquedaSinopsis(const vector<shared_ptr<Movie>> &results, int offset) {
        int limite = 5;
        int start = offset * limite;
        int end = min(start + limite, (int)results.size());

        cout << "Mostrando peliculas " << start + 1 << " a " << end << ":\n";
        for (int i = start; i < end; i++) {
            cout << i + 1 << ". Titulo: " << results[i]->title << "\n";
        }
    }

};

vector<shared_ptr<Movie>> readMoviesFromCSV(const string &filename) {
    vector<shared_ptr<Movie>> movies;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        return movies;
    }

    string line;
    getline(file, line); // Saltar la cabecera

    while (getline(file, line)) {
        stringstream ss(line);
        shared_ptr<Movie> movie = make_shared<Movie>();

        getline(ss, movie->imdb_id, ',');
        getline(ss, movie->title, ',');
        getline(ss, movie->plot_synopsis, ',');
        getline(ss, movie->tags, ',');
        getline(ss, movie->split, ',');
        getline(ss, movie->synopsis_source, ',');

        if (movie->imdb_id.empty() || movie->title.empty() || movie->plot_synopsis.empty()) {
            continue;
        }

        movies.push_back(movie);
    }

    file.close();
    return movies;
}

int main() {
    string filename = "../mpst_full_data.csv";
    vector<shared_ptr<Movie>> movies = readMoviesFromCSV(filename);

    Trie movieTrie;
    for (const auto &movie : movies) {
        movieTrie.insert(movie);
    }

    PlataformaStreaming plataforma;

    string search_query;
    cout << "Enter a word, phrase, or tag to search: ";
    getline(cin, search_query);

    vector<shared_ptr<Movie>> results = movieTrie.search(search_query);
    int offset = 0;

    while (true) {
        plataforma.mostrarResultadosBusquedaSinopsis(results, offset);

        cout << "\nOpciones: \n";
        cout << "1. Siguiente pagina\n";
        cout << "2. Elegir pelicula\n";
        cout << "3. Salir\n";

        int opcion;
        cin >> opcion;

        if (opcion == 1) {
            offset++;
        } else if (opcion == 2) {
            int index;
            cout << "Selecciona el número de la película: ";
            cin >> index;

            if (index > 0 && index <= results.size()) {
                auto movie = results[index - 1];
                cout << "\nTítulo: " << movie->title << "\n";
                cout << "Sinopsis: " << movie->plot_synopsis << "\n";
                cout << "Relevance Score: " << movie->relevance_score << "\n";
                cout << "-----------------------\n";

                cout << "Opciones para esta película:\n";
                cout << "1. Marcar Like\n";
                cout << "2. Marcar Ver más tarde\n";
                cout << "3. Volver\n";

                int sub_opcion;
                cin >> sub_opcion;

                if (sub_opcion == 1) {
                    plataforma.marcarLike(movie);
                } else if (sub_opcion == 2) {
                    plataforma.marcarVerMasTarde(movie);
                }
            } else {
                cout << "Número de película inválido.\n";
            }
        } else if (opcion == 3) {
            break;
        }
    }

    cout << "\nResumen al inicio del programa:\n";
    plataforma.mostrarPeliculasGuardadas();
    plataforma.mostrarPeliculasSimilares();

    return 0;
}