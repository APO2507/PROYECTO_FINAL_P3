# PROYECTO_FINAL_P3

# Programación III: Proyecto Final (2024-2)

## Integrantes
* Marco Esau Zinedine Apolinario Lainez
* Luis Yucef Julio Mendez Carazas
* Gabriel Raymi Vaccaro Ortiz
* Francisco Jose Lira Francia

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

### Flujo del programa
1. **Carga de datos:** Se leen los datos de un archivo CSV con información de películas.
2. **Construcción del Trie:** Se insertan las palabras de títulos y sinopsis en el Trie.
3. **Búsqueda:**
  - Por palabra, frase o string.
  - Por tags específicos.
4. **Ordenación:** Se filtran las películas más relevantes y se muestran al usuario.
