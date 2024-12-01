# PROYECTO_FINAL_P3

# Programación III: Proyecto Final (2024-2)

## Integrantes
* Marco Esau Zinedine Apolinario Lainez
* Luis Yucef Julio Mendez Carazas
* Gabriel Raymi Vaccaro Ortiz
* Francisco Jose Lira Francia
### Implementación de Lectura de Archivo CSV

Este módulo implementa la funcionalidad para cargar datos desde un archivo CSV en un vector de punteros compartidos que representan películas. Es una solución eficiente que utiliza la biblioteca estándar de C++ para el manejo de archivos y cadenas.

### Funcionalidades Implementadas

#### 1. Lectura de Archivo CSV
La función `readMoviesFromCSV` procesa un archivo CSV que contiene información de películas y carga cada entrada como un objeto `Movie` en un vector.

**Características**
- Apertura de Archivo: Abre el archivo especificado y verifica que esté disponible antes de procesarlo.
- Procesamiento de Cabecera: Ignora la primera línea del archivo, que generalmente contiene los nombres de las columnas.
- Extracción de Datos: Utiliza stringstream para separar y extraer los valores de cada columna.
- Creación de Objetos Dinámicos: Los datos de cada fila se almacenan en un objeto Movie creado dinámicamente usando shared_ptr.
- Manejo Automático de Memoria: Utiliza punteros compartidos para evitar fugas de memoria.

###### Código Relevante
``` cpp
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
```
#### Requisitos del Archivo CSV

El archivo debe tener las columnas en el siguiente formato:
- Cabecera: La primera línea del archivo contiene los nombres de las columnas.
- Filas: Cada fila representa una película con los siguientes campos:
  - imdb_id: Identificador único en IMDb.
  - title: Título de la película.
  - plot_synopsis: Resumen del argumento.
  - tags: Etiquetas relacionadas.
  - split: Clasificación (entrenamiento/prueba).
  - synopsis_source: Fuente de la sinopsis.

###### Ejemplo de Formato CSV
```
imdb_id,title,plot_synopsis,tags,split,synopsis_source
tt0111161,The Shawshank Redemption,"Two imprisoned men bond...",Drama,train,IMDb
tt0068646,The Godfather,"The aging patriarch of an organized crime...",Crime,train,IMDb
```

#### Beneficios de la Implementación
- Eficiencia: Procesa grandes archivos CSV de manera rápida.
- Escalabilidad: El uso de punteros compartidos permite manejar grandes cantidades de datos sin problemas de memoria.
- Flexibilidad: Puede adaptarse fácilmente a otros formatos de archivo añadiendo más columnas o cambiando la estructura del objeto `Movie`.



### Implementación de Búsqueda y Algoritmo de Relevancia

Este módulo implementa las funcionalidades de búsqueda en un dataset de películas utilizando una estructura eficiente para manejar palabras clave y tags, y un algoritmo de relevancia para ordenar los resultados.

#### Funcionalidades Implementadas

##### 1. Búsqueda por Palabra, Frase o String
La búsqueda se implementa mediante un **Trie**, permitiendo indexar y buscar palabras de manera eficiente.

###### Características
- **Inserción**: Se indexan palabras provenientes de títulos y sinopsis de las películas.
- **Búsqueda**:
  - Divide la entrada de búsqueda en palabras individuales.
  - Busca cada palabra en el `Trie`.
  - Identifica las películas asociadas a las palabras buscadas.
  - Incrementa un puntaje de relevancia (`relevance_score`) basado en las coincidencias encontradas.

###### Código Relevante
``` cpp
vector<shared_ptr<Movie>> Trie::search(const string &query);
```

##### 2. Búsqueda por Tags
La búsqueda por tags permite filtrar películas utilizando palabras clave relacionadas con categorías o géneros específicos, como "cult", "horror", entre otros.

###### Características
- Busca coincidencias exactas en el campo de tags de cada película.
- Incrementa el puntaje de relevancia (relevance_score) de las películas con tags coincidentes.
- Es útil para identificar películas dentro de un género o tema específico.

###### Flujo de Implementación
1. Itera sobre todas las películas en el dataset.
2. Verifica si el tag buscado está presente en el campo tags.
3. Si hay coincidencias, agrega la película a la lista de resultados y ajusta su relevancia.

###### Código Relevante
```cpp
vector<shared_ptr<Movie>> Trie::searchByTag(const string &tag) const;
```

##### 3. Algoritmo de Relevancia
El algoritmo de relevancia organiza y filtra las películas encontradas basándose en la cantidad de coincidencias con la consulta de búsqueda.

###### Características
- Ordena las películas según el puntaje de relevancia (relevance_score) en orden descendente.
- Filtra las N películas más relevantes (por defecto, 5).
- Mejora la experiencia del usuario al presentar primero los resultados más relevantes.

###### Flujo de Implementación
1. Calcula el puntaje de relevancia de cada película basándose en:
  - Coincidencias de palabras clave en título y sinopsis.
  - Coincidencias de tags, si se utiliza búsqueda por tags. 
2. Ordena las películas utilizando una función de comparación personalizada.
3. Retorna un subconjunto de las películas más relevantes.

###### Código Relevante
```cpp
vector<shared_ptr<Movie>> getTopRelevantMovies(const vector<shared_ptr<Movie>>& movies, int topN = 5);
```

##### 4. Clase película
Representa una película con los siguientes atributos:

Características
ID: Identificador único de la película.
Título: Nombre de la película.
Sinopsis: Descripción o resumen de la película.
Tags: Etiquetas relacionadas con la película.

Código Relevante cpp

Copiar código
struct Pelicula {
    int id;
    string titulo;
    string sinopsis;
    unordered_set<string> tags;

    Pelicula(int id, const string& titulo, const string& sinopsis, const unordered_set<string>& tags)
        : id(id), titulo(titulo), sinopsis(sinopsis), tags(tags) {}
};
##### 5. Clase PlataformaStreaming
Es una plataforma de streaming que gestiona las películas cargadas.

Atributos
peliculas: Una lista de películas.
Métodos
agregarPelicula: Agrega una película a la plataforma.
mostrarPeliculasGuardadas: Muestra todas las películas almacenadas.
cargarDesdeCSV: Carga películas desde un archivo CSV.
Características
Carga de Datos:

Procesa un archivo CSV con las siguientes columnas: id, titulo, sinopsis, tags.
Separa las etiquetas (tags) utilizando el delimitador |.
Visualización:

Muestra el ID, título, sinopsis y etiquetas de cada película almacenada.
Código Relevante
cpp
Copiar código
class PlataformaStreaming {
private:
    vector<Pelicula> peliculas;

public:
    void agregarPelicula(const Pelicula& pelicula) {
        peliculas.push_back(pelicula);
    }

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

            getline(ss, idStr, ',');
            getline(ss, titulo, ',');
            getline(ss, sinopsis, ',');
            getline(ss, tagsStr, ',');

            int id = stoi(idStr);

            unordered_set<string> tags;
            stringstream tagsStream(tagsStr);
            string tag;
            while (getline(tagsStream, tag, '|')) {
                tags.insert(tag);
            }

            agregarPelicula(Pelicula(id, titulo, sinopsis, tags));
        }

        archivo.close();
        cout << "Películas cargadas desde el archivo: " << rutaArchivo << "\n";
    }
};
### Flujo del programa
1. **Carga de datos:** Se leen los datos de un archivo CSV con información de películas.
2. **Construcción del Trie:** Se insertan las palabras de títulos y sinopsis en el Trie.
3. **Búsqueda:**
  - Por palabra, frase o string.
  - Por tags específicos.
4. **Ordenación:** Se filtran las películas más relevantes y se muestran al usuario.
