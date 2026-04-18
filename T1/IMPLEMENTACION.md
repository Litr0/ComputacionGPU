# Reporte de aprendizaje de implementacion - Tarea 1 Matrix

## 1) Relectura de requisitos (Tarea1_GPU.pdf)

Resumen del enunciado revisado:

- Implementar una clase `Matrix` con almacenamiento en `double`.
- Implementar operaciones base:
  - Guardar matriz a archivo.
  - Crear matriz desde archivo.
  - Imprimir en consola.
  - Calculo de traspuesta.
- De las 3 operaciones marcadas en azul, elegir 2:
  1. Suma y resta.
  2. Multiplicacion matricial.
  3. Multiplicacion por constante.
- Testear con Google Test (se espera test de operaciones y de errores).
- Lanzar excepciones en operaciones invalidas.
- Usar C++23.

Decision de alcance aplicada en este proyecto:

- Implementadas de las azules: suma/resta y multiplicacion por constante.
- No implementada: multiplicacion matricial (`Matrix *= Matrix`).

## 2) Mapa rapido: requisito -> codigo

- Tipo `double`:
  - `Matrix.h`: `std::unique_ptr<double[]> mat`.
- Constructores y estado inicial:
  - `Matrix.cpp`: `Matrix()`, `Matrix(int)`, `Matrix(int,int)`.
  - Inicializan en cero y validan dimensiones negativas.
- Acceso y dimension:
  - `operator[](x,y)` con verificacion de rango (`index_of`).
  - `size()` retorna `(filas,columnas)`.
- I/O:
  - `Matrix(const std::string &filename)` lee desde archivo.
  - `save_to_file(...)` guarda en texto.
  - `operator<<` imprime por filas.
- Copia/comparacion:
  - Copy constructor profundo.
  - `operator=` con copy-swap.
  - `operator==` y `operator!=` con tolerancia `epsilon`.
- Algebra elegida:
  - `operator+=`, `operator-=`, `operator+`, `operator-`.
  - `operator*=(double)`.
  - `transpose()`.
- Errores:
  - `std::invalid_argument` para dimensiones invalidas.
  - `std::out_of_range` para indices invalidos.
  - `std::logic_error` para dimensiones incompatibles en suma/resta.
  - `std::runtime_error` para problemas de archivo.

## 3) Como entender la implementacion paso a paso

### 3.1 Estructura interna y layout en memoria

La matriz se guarda en un arreglo lineal de `double`.
Indice lineal usado:

`index = fila * columnas + columna`

Eso permite:

- Acceso O(1) a cada celda.
- Buen comportamiento de cache para recorrido por filas.

### 3.2 Ayudante central: `index_of(...)`

`index_of` hace dos cosas:

1. Valida rango de fila y columna.
2. Convierte `(x,y)` a indice lineal.

Este helper centraliza validacion y evita duplicar chequeos en getters/setters.

### 3.3 Reglas de construccion

- `Matrix()` -> matriz vacia `0x0`.
- `Matrix(n)` -> vector columna `n x 1`.
- `Matrix(n,m)` -> matriz general `n x m`.
- Si `n` o `m` es negativo, se lanza `invalid_argument`.
- Toda matriz nueva se inicializa en `0.0`.

### 3.4 Lectura/escritura de archivos

Formato usado en `save_to_file`:

- Primera linea: `n m`
- Luego `n*m` valores por filas.

`Matrix(filename)` espera ese formato y valida:

- Que el archivo exista.
- Que dimensiones sean validas.
- Que haya suficientes datos.

### 3.5 Igualdad con punto flotante

`operator==` usa `kEpsilon = 1e-9`.

En floating-point, comparar con `==` exacto suele fallar por redondeo.
Por eso la implementacion usa diferencia absoluta y tolerancia.

### 3.6 Suma y resta: version mutante y no mutante

- Mutantes:
  - `operator+=`
  - `operator-=`
- No mutantes:
  - `operator+`
  - `operator-`

Patron usado:

- `operator+` crea copia y delega a `+=`.
- `operator-` crea copia y delega a `-=`.

Esto evita duplicar logica y mantiene consistencia.

### 3.7 Multiplicacion por constante

`operator*=(double a)` recorre el arreglo lineal y escala cada valor.
Complejidad temporal O(n*m), sin memoria extra.

### 3.8 Traspuesta

`transpose()` crea una matriz temporal de tamano invertido `m x n`.
Luego copia `result[j,i] = this[i,j]` y reasigna `*this = result`.

## 4) Complejidad (alto nivel)

Para una matriz `n x m`:

- `operator[]`: O(1)
- `fill`: O(n*m)
- `+=`, `-=`, `*=(double)`: O(n*m)
- `transpose`: O(n*m)
- `save_to_file` / cargar desde archivo: O(n*m)
- `==`: O(n*m)

## 5) Fase de testing (Google Test)

Cobertura actual observada en `tests/test_matrix.cpp`:

- Constructores y dimensiones.
- Rechazo de dimensiones negativas.
- Lectura/escritura de valores `double`.
- `fill`.
- Out-of-range en indexacion.
- Suma/resta (mutante y no mutante) + mismatch de dimensiones.
- Multiplicacion por constante.
- Traspuesta.
- Copia profunda y asignacion (incluye self-assignment).
- Igualdad con tolerancia.
- Roundtrip de archivo y fallas de archivo.
- Formato de salida por stream.

Observacion del enunciado:

- No era obligatorio testear impresion por pantalla, pero aqui se incluye igual.

## 6) Que aprender de este diseno

- Separar API publica y detalles internos.
- Centralizar validaciones (`index_of`) reduce bugs.
- Reusar operadores compuestos para construir operadores no mutantes.
- Manejo explicito de excepciones mejora robustez y testabilidad.
- Usar tolerancia para comparar `double` evita falsos negativos.

## 7) Posibles mejoras futuras (opcionales)

- Permitir parseo mas flexible de formatos de archivo.
- Soportar tolerancia configurable para `operator==`.
- Agregar benchmarks simples para matrices grandes.
- Si el alcance cambia, reintroducir multiplicacion matricial con tests dedicados.

## 8) Conclusiones

La implementacion cumple el alcance acordado para esta entrega:

- Matriz en `double`.
- Operaciones base y de algebra elegidas.
- Excepciones para casos invalidos.
- Suite de pruebas amplia para aprender y validar comportamiento.

Este reporte esta orientado a estudio: conviene leerlo junto a `Matrix.h`, `Matrix.cpp` y `tests/test_matrix.cpp` para seguir cada decision en codigo real.

