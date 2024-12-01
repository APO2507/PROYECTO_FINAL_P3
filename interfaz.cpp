#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>


using namespace std;

// Clase Pelicula
struct Pelicula {
    int id;
    string titulo;
    string sinopsis;
    unordered_set<string> tags;

    Pelicula(int id, const string& titulo, const string& sinopsis, const unordered_set<string>& tags)
        : id(id), titulo(titulo), sinopsis(sinopsis), tags(tags) {}
};

// Clase PlataformaStreaming
class PlataformaStreaming {
private:
    vector<Pelicula> peliculas;             // Lista de películas

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
            getline(ss, idStr, ',');
            getline(ss, titulo, ',');
            getline(ss, sinopsis, ',');
            getline(ss, tagsStr, ',');

            // Convertir ID a entero
            int id = stoi(idStr);

            // Procesar tags (separados por "|")
            unordered_set<string> tags;
            stringstream tagsStream(tagsStr);
            string tag;
            while (getline(tagsStream, tag, '|')) {
                tags.insert(tag);
            }

            // Crear la película y agregarla
            agregarPelicula(Pelicula(id, titulo, sinopsis, tags));
        }

        archivo.close();
        cout << "Películas cargadas desde el archivo: " << rutaArchivo << "\n";
    }
};

// Función principal
int main() {
    PlataformaStreaming plataforma;

    // Cargar películas desde el archivo CSV
    string rutaArchivo = "peliculas.csv";
    plataforma.cargarDesdeCSV(rutaArchivo);

    // Mostrar películas guardadas
    plataforma.mostrarPeliculasGuardadas();

    return 0;
}
