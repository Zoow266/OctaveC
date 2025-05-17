#ifndef LIBRARY_H
#define LIBRARY_H

#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <functional>

// Вспомогательные функции для преобразования octave_value в типы C++
namespace OctaveUtils {
    // Преобразование octave_value в double
    double toDouble(const octave_value& value) {
        if (!value.is_scalar_type()) {
            throw std::runtime_error("Ожидалось скалярное значение.");
        }
        return value.double_value();
    }

    // Преобразование octave_value в Matrix
    Matrix toMatrix(const octave_value& value) {
        if (!value.is_matrix_type()) {
            throw std::runtime_error("Ожидалась матрица.");
        }
        return value.matrix_value();
    }

    // Преобразование octave_value в ComplexMatrix
    ComplexMatrix toComplexMatrix(const octave_value& value) {
        if (!value.is_complex_matrix()) { // Исправлено: is_complex_matrix
            throw std::runtime_error("Ожидалась комплексная матрица.");
        }
        return value.complex_matrix_value();
    }

    // Преобразование octave_value в std::vector<double>
    std::vector<double> toVectorDouble(const octave_value& value) {
        if (!value.is_matrix_type() || value.rows() != 1) {
            throw std::runtime_error("Ожидался вектор-строка.");
        }
        Matrix mat = value.matrix_value();
        std::vector<double> result(mat.numel());
        for (int i = 0; i < mat.numel(); ++i) {
            result[i] = mat(i);
        }
        return result;
    }
}

// Класс для операций с матрицами
class MatrixOperations {
public:
    MatrixOperations(octave::interpreter& interp) : interpreter(interp) {}

    // Обратная матрица
    Matrix computeInverse(const Matrix& mat) {
        return OctaveUtils::toMatrix(callOctaveFunction("inv", mat));
    }

    // Транспонированная матрица
    Matrix computeTranspose(const Matrix& mat) {
        return OctaveUtils::toMatrix(callOctaveFunction("transpose", mat));
    }

    // Определитель матрицы
    double computeDeterminant(const Matrix& mat) {
        return OctaveUtils::toDouble(callOctaveFunction("det", mat));
    }

    // Сложение матриц
    Matrix addMatrices(const Matrix& matA, const Matrix& matB) {
        if (matA.rows() != matB.rows() || matA.columns() != matB.columns()) {
            throw std::runtime_error("Ошибка: размеры матриц не совпадают для сложения.");
        }
        return matA + matB;
    }

    // Вычитание матриц
    Matrix subtractMatrices(const Matrix& matA, const Matrix& matB) {
        if (matA.rows() != matB.rows() || matA.columns() != matB.columns()) {
            throw std::runtime_error("Ошибка: размеры матриц не совпадают для вычитания.");
        }
        return matA - matB;
    }

    // Умножение матриц
    Matrix multiplyMatrices(const Matrix& matA, const Matrix& matB) {
        if (matA.columns() != matB.rows()) {
            throw std::runtime_error("Ошибка: количество столбцов первой матрицы не равно количеству строк второй матрицы для умножения.");
        }
        return matA * matB;
    }

    // Решение СЛАУ
    Matrix solveLinearSystem(const Matrix& A, const ColumnVector& b) {
        octave_value_list in;
        in(0) = octave_value(A);
        in(1) = octave_value(b);
        return OctaveUtils::toMatrix(callOctaveFunction("linsolve", in));
    }

private:
    octave::interpreter& interpreter;

    // Вспомогательная функция для вызова Octave
    octave_value callOctaveFunction(const std::string& function_name, const octave_value_list& args) {
        octave_value_list out_args = octave::feval(function_name, args);
        if (out_args.length() == 0) {
            throw std::runtime_error("Ошибка при вызове функции: " + function_name);
        }
        return out_args(0);
    }

    octave_value callOctaveFunction(const std::string& function_name, const octave_value& arg) {
        octave_value_list args;
        args(0) = arg;
        return callOctaveFunction(function_name, args);
    }
};

// Класс для решения уравнений
class EquationSolver {
public:
    EquationSolver(octave::interpreter& interp) : interpreter(interp) {}

    // Вычисление корней полинома
    ComplexMatrix computePolynomialRoots(const std::vector<double>& coefficients) {
        std::ostringstream oss;
        oss << "roots([";
        for (size_t i = 0; i < coefficients.size(); ++i) {
            oss << coefficients[i];
            if (i < coefficients.size() - 1) oss << ", ";
        }
        oss << "])";

        octave_value_list out = interpreter.eval(oss.str(), 1);
        if (out.length() == 0) {
            throw std::runtime_error("Ошибка при вычислении корней полинома.");
        }
        return OctaveUtils::toComplexMatrix(out(0));
    }

    // Решение системы нелинейных уравнений
    std::vector<Matrix> solveNonlinearSystem(const std::string& function_str, const Matrix& initial_guesses) {
        std::vector<Matrix> solutions;
        solutions.reserve(initial_guesses.columns());

        for (octave_idx_type i = 0; i < initial_guesses.columns(); ++i) {
            octave_value_list fsolve_in, fsolve_out;
            fsolve_in(0) = octave_value(function_str);
            fsolve_in(1) = octave_value(initial_guesses.column(i));
            fsolve_out = octave::feval("fsolve", fsolve_in, 1);

            if (fsolve_out.length() > 0 && fsolve_out(0).is_matrix_type()) {
                solutions.push_back(OctaveUtils::toMatrix(fsolve_out(0)));
            } else {
                throw std::runtime_error("Не удалось решить систему уравнений для начального предположения.");
            }
        }
        return solutions;
    }

    // Решение уравнения
    double solveEquation(const std::string& function_str, double initial_guess) {
        octave_value_list fsolve_in, fsolve_out;
        fsolve_in(0) = octave_value(function_str);
        fsolve_in(1) = octave_value(initial_guess);
        fsolve_out = octave::feval("fsolve", fsolve_in, 1);

        if (fsolve_out.length() > 0 && fsolve_out(0).is_scalar_type()) {
            return OctaveUtils::toDouble(fsolve_out(0));
        } else {
            throw std::runtime_error("Не удалось решить уравнение.");
        }
    }

private:
    octave::interpreter& interpreter;
};

#endif // LIBRARY_H
