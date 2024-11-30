#include <iostream>
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
    unordered_set<int> likes;               // IDs de películas que el usuario le gusta
    unordered_set<string> tagsGustados;     // Tags relacionados con las películas que le gustan al usuario

public:
    // Agrega una película a la plataforma
    void agregarPelicula(const Pelicula& pelicula) {
        peliculas.push_back(pelicula);
    }

    // Marca una película como "like" y actualiza los tags gustados
    void darLike(int peliculaId) {
        for (const auto& pelicula : peliculas) {
            if (pelicula.id == peliculaId) {
                likes.insert(peliculaId);
                tagsGustados.insert(pelicula.tags.begin(), pelicula.tags.end());
                break;
            }
        }
    }

    // Muestra información de una película
    void mostrarPelicula(const Pelicula& pelicula) {
        cout << "ID: " << pelicula.id << "\n";
        cout << "Título: " << pelicula.titulo << "\n";
        cout << "Sinopsis: " << pelicula.sinopsis << "\n";
        cout << "Tags: ";
        for (const auto& tag : pelicula.tags) {
            cout << tag << " ";
        }
        cout << "\n\n";
    }

    // Busca películas por título y las muestra
    void buscarPelicula(const string& titulo) {
        cout << "Resultados de búsqueda para: " << titulo << "\n";
        for (const auto& pelicula : peliculas) {
            if (pelicula.titulo.find(titulo) != string::npos) {
                mostrarPelicula(pelicula);
            }
        }
    }

    // Muestra todas las películas guardadas
    void mostrarPeliculasGuardadas() {
        cout << "Películas guardadas:\n";
        for (const auto& pelicula : peliculas) {
            mostrarPelicula(pelicula);
        }
    }

    // Muestra recomendaciones basadas en los tags gustados
    void mostrarSimilares() {
        cout << "Recomendaciones basadas en tus gustos:\n";
        for (const auto& pelicula : peliculas) {
            if (likes.count(pelicula.id) == 0) { // Evitar recomendar películas ya en likes
                for (const auto& tag : pelicula.tags) {
                    if (tagsGustados.count(tag) > 0) {
                        mostrarPelicula(pelicula);
                        break; // Mostrar cada película solo una vez
                    }
                }
            }
        }
    }
};

// Función principal
int main() {
    PlataformaStreaming plataforma;

    // Agregar películas a la plataforma
    plataforma.agregarPelicula(Pelicula(1, "El origen", "Un ladrón que roba secretos del subconsciente.", {"ciencia ficción", "acción", "sueños"}));
    plataforma.agregarPelicula(Pelicula(2, "La red social", "La historia detrás de Facebook.", {"drama", "tecnología"}));
    plataforma.agregarPelicula(Pelicula(3, "Interestelar", "Viajes espaciales para salvar a la humanidad.", {"ciencia ficción", "drama", "espacio"}));

    // Dar like a una película
    plataforma.darLike(1);

    // Buscar películas
    cout << "Búsqueda: ";
    string busqueda;
    cin >> busqueda;
    plataforma.buscarPelicula(busqueda);

    // Mostrar películas guardadas
    plataforma.mostrarPeliculasGuardadas();

    // Mostrar recomendaciones
    plataforma.mostrarSimilares();

    return 0;
}
