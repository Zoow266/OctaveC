# Документация

## Общая информация
**OctaveC** — это C++ библиотека, обеспечивающая удобный доступ к мощным возможностям GNU Octave: матричные операции, вычисление определителей, транспонирование, решение СЛАУ и уравнений. Библиотека ориентирована на интеграцию с `liboctave`, предоставляя объектно-ориентированный интерфейс поверх Octave API.

## Зависимости
Перед использованием библиотеки убедитесь, что у вас установлены следующие пакеты:
- GNU Octave (>= 6.0.0) с `liboctave-dev`
- GCC или G++ (>= 9.0)

### Установка зависимостей (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install liboctave-dev octave g++
```

Проверьте, что Octave установлен:
```bash
octave --version
```

## Структура проекта
```
project/
├── Library.h            # Заголовочный файл библиотеки
├── main.cpp             # Пример использования
```

## Компиляция
Библиотека компилируется без использования CMake. Используйте следующую команду:
```bash
g++ -o main main.cpp -I/usr/include/octave-X.Y.Z -I/usr/include/octave-X.Y.Z/octave -I/usr/include/scilab -L/usr/lib/x86_64-linux-gnu/octave/X.Y.Z -L/usr/lib/scilab -loctave -loctinterp -lscilab -Wl,-rpath=/usr/lib/x86_64-linux-gnu/octave/X.Y.Z -Wl,-rpath=/usr/lib/scilab
```
где `X.Y.Z` – версия Octave.

## Инициализация Octave
Перед вызовом любых функций необходимо инициализировать интерпретатор:
```cpp
octave::interpreter interpreter;
interpreter.initialize();
```

## Использование

### MatrixOperations
Класс для базовых линейных операций.

**Примеры:**
```cpp
MatrixOperations ops(interpreter);

// Сложение
Matrix result = ops.addMatrices(matA, matB);

// Обратная матрица
Matrix inv = ops.computeInverse(matA);

// Определитель
double det = ops.computeDeterminant(matA);

// Решение СЛАУ
ColumnVector b(n);
// ... заполнение b
Matrix x = ops.solveLinearSystem(matA, b);
```

### EquationSolver
Класс для решения уравнений.

```cpp
EquationSolver solver(interpreter);

// Нахождение корней полинома
std::vector<double> coeffs = {1, -3, 2}; // x^2 - 3x + 2
ComplexMatrix roots = solver.computePolynomialRoots(coeffs);

// Решение уравнения
double root = solver.solveEquation("x^2 - 2", 1.0);

// Система нелинейных уравнений
std::string f = "@(x) [x(1)^2 + x(2) - 37; x(1) - x(2)^2 - 5]";
Matrix guesses(2, 1); guesses(0,0) = 1.0; guesses(1,0) = 1.0;
auto solutions = solver.solveNonlinearSystem(f, guesses);
```

## Поддерживаемые функции Octave

| Функция Octave | Описание               |
|----------------|------------------------|
| `inv`          | Обратная матрица       |
| `transpose`    | Транспонирование       |
| `det`          | Определитель           |
| `linsolve`     | Решение СЛАУ           |
| `fsolve`       | Решение уравнений      |
| `roots`        | Корни полинома         |

## Ошибки и исключения
Все ошибки преобразований и вызовов Octave функций выбрасываются как `std::runtime_error`. Оберните вызовы в try-catch:

```cpp
try {
    auto inv = ops.computeInverse(mat);
} catch (const std::runtime_error& e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
}
```

## Переменные и типы

| Тип               | Описание                                             |
|------------------|------------------------------------------------------|
| `Matrix`         | Двумерная вещественная матрица (`octave::Matrix`)   |
| `ComplexMatrix`  | Комплексная матрица                                  |
| `ColumnVector`   | Вектор-столбец                                       |
| `octave_value`   | Объект Octave (универсальное представление)         |
| `std::vector<double>` | Используется для передачи коэффициентов полиномов |

## Очистка
Уничтожение интерпретатора происходит автоматически, однако при использовании в большом приложении можно явно завершить:
```cpp
interpreter.reset();
```

## Расширение
Вы можете расширить функциональность, добавляя вызовы других Octave-функций через `octave::feval()` или `interpreter.eval()`.
