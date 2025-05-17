#include "Library.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cmath>

int main() {
    try {
        // Инициализация интерпретатора Octave
        auto total_start = std::chrono::high_resolution_clock::now();
        auto init_start = std::chrono::high_resolution_clock::now();
        octave::interpreter interpreter;
        interpreter.initialize();
        auto init_end = std::chrono::high_resolution_clock::now();
        
        std::cout << "==============================================\n"
                  << " Матричные операции и оптимизация\n"
                  << "==============================================\n\n";
        
        std::cout << "Инициализация Octave: " 
                  << std::chrono::duration<double>(init_end - init_start).count() 
                  << " сек.\n\n";

        // Создание экземпляров классов
        MatrixOperations matrixOps(interpreter);
        const int size = 1000;

        // --- Матричные операции ---
        std::cout << "МАТРИЧНЫЕ ОПЕРАЦИИ (размер матриц: " << size << "x" << size << ")\n";
        std::cout << "------------------------------------------------\n";

        // Инициализация данных
        Matrix matA(size, size);
        Matrix matB(size, size);
        ColumnVector b(size);
        auto start = std::chrono::high_resolution_clock::now();
        
        // 1. Заполнение матриц и вектора
	for (int i = 0; i < size; ++i) {
	    for (int j = 0; j < size; ++j) {
		// Уменьшаем диагональные элементы для предотвращения переполнения определителя
		matA(i, j) = (i == j) ? 1.0 + static_cast<double>(std::rand() % 10) / 10.0 : 0.1;
		matB(i, j) = static_cast<double>(std::rand())/RAND_MAX;
	    }
	    b(i) = static_cast<double>(std::rand())/RAND_MAX;
	}
        auto end = std::chrono::high_resolution_clock::now();
        //printf("1. Заполнение матриц A, B и вектора b: %.4f сек\n", 
              //std::chrono::duration<double>(end - start).count());

        // 2. Сложение матриц
        start = std::chrono::high_resolution_clock::now();
        Matrix sumMat = matrixOps.addMatrices(matA, matB);
        end = std::chrono::high_resolution_clock::now();
        printf("1. Сложение матриц A + B: %.4f сек\n",
              std::chrono::duration<double>(end - start).count());

        // 3. Вычитание матриц
        start = std::chrono::high_resolution_clock::now();
        Matrix diffMat = matrixOps.subtractMatrices(matA, matB);
        end = std::chrono::high_resolution_clock::now();
        printf("2. Вычитание матриц A - B: %.4f сек\n",
              std::chrono::duration<double>(end - start).count());

        // 4. Умножение матриц
        start = std::chrono::high_resolution_clock::now();
        Matrix prodMat = matrixOps.multiplyMatrices(matA, matB);
        end = std::chrono::high_resolution_clock::now();
        printf("3. Умножение матриц A * B: %.4f сек\n",
              std::chrono::duration<double>(end - start).count());

        // 5. Определитель матрицы
        start = std::chrono::high_resolution_clock::now();
        double det = matrixOps.computeDeterminant(matA);
        end = std::chrono::high_resolution_clock::now();
        printf("4. Определитель матрицы A: %.4e\t%.4f сек\n",
              det, std::chrono::duration<double>(end - start).count());

        // 6. Решение СЛАУ
        start = std::chrono::high_resolution_clock::now();
        ColumnVector x = matrixOps.solveLinearSystem(matA, b);
        end = std::chrono::high_resolution_clock::now();
        printf("5. Решение СЛАУ Ax = b: %.4f сек\n",
              std::chrono::duration<double>(end - start).count());

        // 7. Обратная матрица
        start = std::chrono::high_resolution_clock::now();
        Matrix invA = matrixOps.computeInverse(matA);
        end = std::chrono::high_resolution_clock::now();
        printf("6. Обратная матрица A⁻¹: %.4f сек\n",
              std::chrono::duration<double>(end - start).count());

        // 8. Проверка точности обратной матрицы
        start = std::chrono::high_resolution_clock::now();
        Matrix identity = matrixOps.multiplyMatrices(matA, invA);
        
        double max_error = 0.0;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                double target = (i == j) ? 1.0 : 0.0;
                max_error = std::max(max_error, std::abs(identity(i,j) - target));
            }
        }
        end = std::chrono::high_resolution_clock::now();
        //printf("8. Проверка A*A⁻¹ ≈ I: ошибка=%.4e\t%.4f сек\n",
              //max_error, std::chrono::duration<double>(end - start).count());

        std::cout << "\n";

        // --- Оптимизационные задачи ---
        std::cout << "ОПТИМИЗАЦИОННЫЕ ЗАДАЧИ\n";
        std::cout << "------------------------------------------------\n";

        // --- Пример 1: Оптимизация с ограничениями ---
        std::cout << "ЗАДАЧА 1: Оптимизация с ограничениями\n";
        std::cout << "Минимизировать: f(x) = 4x₁ + x₁² + 8x₂ + x₂² + x₃²\n";
        std::cout << "При ограничении: x₁ + x₂ + x₃ = 120\n";
        std::cout << "Начальная точка: (0, 0, 0)\n";
        std::cout << "Метод: SQP (Sequential Quadratic Programming)\n";

        auto task1_start = std::chrono::high_resolution_clock::now();
        
        // Определение функций f, g и fi в Octave
        interpreter.eval("function y = f(x) y = 4*x(1) + x(1)^2 + 8*x(2) + x(2)^2 + x(3)^2; endfunction", 0);
        interpreter.eval("function z = g(x) z = x(1) + x(2) + x(3) - 120; endfunction", 0);
        interpreter.eval("function u = fi(x) u = [x(1); x(2); x(3)]; endfunction", 0);

        // Начальное предположение
        ColumnVector x0(3);
        x0(0) = 0.0;
        x0(1) = 0.0;
        x0(2) = 0.0;

        // Вызов функции sqp
        octave_value_list in_args;
        in_args(0) = x0;
        in_args(1) = octave_value("f");
        in_args(2) = octave_value("g");
        in_args(3) = octave_value("fi");

        auto solve_start = std::chrono::high_resolution_clock::now();
        octave_value_list out_args = interpreter.feval("sqp", in_args, 4);
        auto solve_end = std::chrono::high_resolution_clock::now();

        // Получение результатов
        if (out_args.length() >= 4) {
            ColumnVector x_opt = out_args(0).column_vector_value();
            double obj = out_args(1).double_value();
            int info = out_args(2).int_value();
            int iter = out_args(3).int_value();

            // Вывод результатов
            std::cout << "\nРЕЗУЛЬТАТЫ:\n";
            std::cout << "Оптимальная точка: x = \n" << x_opt;
            std::cout << "Значение функции: " << obj << std::endl;
            std::cout << "Статус решения: " << info << std::endl;
            std::cout << "Число итераций: " << iter << std::endl;
        } else {
            std::cerr << "Ошибка: функция sqp вернула меньше аргументов, чем ожидалось." << std::endl;
        }

        auto task1_end = std::chrono::high_resolution_clock::now();
        std::cout << "Время решения задачи: " 
                  << std::chrono::duration<double>(solve_end - solve_start).count() 
                  << " сек.\n";
        std::cout << "Общее время выполнения (с подготовкой): " 
                  << std::chrono::duration<double>(task1_end - task1_start).count() 
                  << " сек.\n\n";

        // --- Пример 2: Минимизация полинома 4-й степени ---
        std::cout << "ЗАДАЧА 2: Минимизация полинома\n";
        std::cout << "Функция: phi(x) = x⁴ + 3x³ - 13x² - 6x + 26\n";
        std::cout << "Начальная точка: x = -3.0\n";
        std::cout << "Метод: SQP (Sequential Quadratic Programming)\n";

        auto task2_start = std::chrono::high_resolution_clock::now();
        
        // Определение функции phi в Octave
        interpreter.eval("function obj = phi(x) obj = x^4 + 3*x^3 - 13*x^2 - 6*x + 26; endfunction", 0);

        // Начальное предположение
        double x0_phi = -3.0;

        // Вызов функции sqp
        octave_value_list in_args_phi;
        in_args_phi(0) = x0_phi;
        in_args_phi(1) = octave_value("phi");

        auto solve2_start = std::chrono::high_resolution_clock::now();
        octave_value_list out_args_phi = interpreter.feval("sqp", in_args_phi, 4);
        auto solve2_end = std::chrono::high_resolution_clock::now();

        // Получение результатов
        if (out_args_phi.length() >= 4) {
            double x_phi = out_args_phi(0).double_value();
            double obj_phi = out_args_phi(1).double_value();
            int info_phi = out_args_phi(2).int_value();
            int iter_phi = out_args_phi(3).int_value();

            // Вывод результатов
            std::cout << "\nРЕЗУЛЬТАТЫ:\n";
            std::cout << "Оптимальная точка: x = " << x_phi << std::endl;
            std::cout << "Значение функции: " << obj_phi << std::endl;
            std::cout << "Статус решения: " << info_phi << std::endl;
            std::cout << "Число итераций: " << iter_phi << std::endl;
        } else {
            std::cerr << "Ошибка: функция sqp вернула меньше аргументов, чем ожидалось." << std::endl;
        }

        auto task2_end = std::chrono::high_resolution_clock::now();
        std::cout << "Время решения задачи: " 
                  << std::chrono::duration<double>(solve2_end - solve2_start).count() 
                  << " сек.\n";
        std::cout << "Общее время выполнения (с подготовкой): " 
                  << std::chrono::duration<double>(task2_end - task2_start).count() 
                  << " сек.\n";

        // Общее время работы
        auto total_end = std::chrono::high_resolution_clock::now();
        std::cout << "\n==============================================\n";
        std::cout << "Общее время выполнения программы: " 
                  << std::chrono::duration<double>(total_end - total_start).count() 
                  << " сек.\n";
        std::cout << "==============================================\n";

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
