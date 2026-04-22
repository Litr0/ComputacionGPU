# Tarea 1 - Matrix (C++23)

La clase `Matrix` almacena y manipula valores de tipo `double`.

## Compilar y ejecutar

Este proyecto usa C++23, Google Test por CMake y desarrollado en Ubuntu.

Ejecución de comandos para compilar y correr tests:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Ejecución de ejemplo:

```bash
./build/example
```

## Decisiones de implementación

- Operaciones azules elegidas: suma/resta de matrices y multiplicación por constante.

## Respuestas:

1. **Afecta el tipo de dato de la matriz? Qué pasa si se multiplica con `int` en vez de `double`?**
   Sí, afecta directamente precisión y rango. Con `int`, toda parte fraccionaria se pierde por truncamiento y aparecen más desbordamientos en productos acumulados. Con `double` se preservan decimales y se obtiene mejor representación para cálculo numérico.

2. **Qué ocurre con números muy pequeños, muy grandes o primos en términos de precisión?**
   En `double`, los números se representan en punto flotante binario con precisión finita. Números muy grandes pueden perder resolución relativa y números muy pequeños pueden subdesbordar a cero. Muchos racionales (incluyendo combinaciones con primos) no tienen representación binaria exacta, por lo que aparecen errores de redondeo acumulados.

3. **Puede haber problemas al comparar matrices idénticas pero con distinto tipo?**
   Sí. Comparaciones exactas entre tipos distintos suelen fallar por conversiones y redondeo. Por eso en esta implementación la igualdad usa una tolerancia (`epsilon`) para comparar `double` de manera robusta.
