#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <memory>
#include <algorithm>
#include <cctype>
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
            insertWord(word, movie,1);
        }
    }

    // Búsqueda por palabras y frases
    vector<shared_ptr<Movie>> search(const string &query) {

        for (auto &movie : movies) {
            movie->relevance_score = 0;                // -> Reseteamos siempre el relevance para c/u de las pel
        }

        vector<string> words = splitWords(query);
        vector<shared_ptr<Movie>> result;

        set<string> uniqueMovies;   // usamos el imdb_id para evita duplicados
        for (const string &word : words) {
            vector<shared_ptr<Movie>> movies_with_word = searchWord(word);
            for (auto &movie : movies_with_word) {
                if (uniqueMovies.find(movie->imdb_id) == uniqueMovies.end()) {
                    result.push_back(movie);
                    uniqueMovies.insert(movie->imdb_id);
                }
                movie->relevance_score++; // va aumetndo el relevance por cada coincidencia
            }
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
vector<shared_ptr<Movie>> readMoviesFromCSV(const string &filename) {
    vector<shared_ptr<Movie>> movies;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        return movies;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        shared_ptr<Movie> movie = make_shared<Movie>();

        getline(ss, movie->imdb_id, ',');
        getline(ss, movie->title, ',');
        getline(ss, movie->plot_synopsis, ',');
        getline(ss, movie->tags, ',');
        getline(ss, movie->split, ',');
        getline(ss, movie->synopsis_source, ',');

        movies.push_back(movie);
    }

    file.close();
    return movies;
}

struct Pelicula {
    string id;
    string titulo;
    string sinopsis;
    unordered_set<string> tags;

    Pelicula(const string& id, const string& titulo, const string& sinopsis, const unordered_set<string>& tags)
        : id(id), titulo(titulo), sinopsis(sinopsis), tags(tags) {}
};

// Clase PlataformaStreaming
class PlataformaStreaming {
private:
    vector<Pelicula> peliculas; // Lista de películas

public:
    // Agrega una película a la plataforma
    void agregarPelicula(const Pelicula& pelicula) {
        peliculas.push_back(pelicula);
    }

    // Muestra todas las películas guardadas
    void mostrarPeliculasGuardadas() {
        cout << "Películas guardadas:\n";
        for (const auto& pelicula : peliculas) {
            cout << "ID: " << pelicula.id << "\n";
            cout << "Título: " << pelicula.titulo << "\n";
            cout << "Sinopsis: " << pelicula.sinopsis << "\n";
            cout << "Tags: ";
            for (const auto& tag : pelicula.tags) {
                cout << tag << " ";
            }
            cout << "\n\n";
        }
    }

    // Cargar películas desde un archivo CSV
    void cargarDesdeCSV(const string& rutaArchivo) {
        ifstream archivo(rutaArchivo);
        if (!archivo.is_open()) {
            cerr << "Error al abrir el archivo: " << rutaArchivo << "\n";
            return;
        }

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            stringstream ss(linea);
            string idStr, titulo, sinopsis, tagsStr;

            // Leer los campos separados por comas
            getline(ss, idStr, ',');  // Usamos idStr directamente como string
            getline(ss, titulo, ',');
            getline(ss, sinopsis, ',');
            getline(ss, tagsStr, ',');

            // Procesar tags (separados por "|")
            unordered_set<string> tags;
            stringstream tagsStream(tagsStr);
            string tag;
            while (getline(tagsStream, tag, '|')) {
                tags.insert(tag);
            }

            // Crear la película y agregarla
            agregarPelicula(Pelicula(idStr, titulo, sinopsis, tags));
        }

        archivo.close();
        cout << "Películas cargadas desde el archivo: " << rutaArchivo << "\n";
    }
};


int main() {
    string filename = "C:/Users/GTX 1660 TI/Documents/C-Lion Proyects/POO II/mpst_full_data.csv";
    vector<shared_ptr<Movie>> movies = readMoviesFromCSV(filename);

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
        cout << "\nTop Movies found:\n" << endl;
        for (const auto &movie : topMovies) {
            cout << "Title: " << movie->title << endl;
            cout << "Plot Synopsis: " << movie->plot_synopsis << endl;
            cout << "Relevance Score: " << movie->relevance_score << endl;
            cout << "-----------------------" << endl;
        }

        cout << "Peliculas total ----->" << found_movies.size() << ", \n" << endl;  // aca solo lo puse para ver cuantas pelis encontraba
    } else {
        cout << "No movies found matching the query." << endl;
    }
    PlataformaStreaming plataforma;

    // Cargar películas desde el archivo CSV
    string rutaArchivo = "peliculas.csv";
    plataforma.cargarDesdeCSV(rutaArchivo);

    // Mostrar películas guardadas
    plataforma.mostrarPeliculasGuardadas();

    return 0;
}
