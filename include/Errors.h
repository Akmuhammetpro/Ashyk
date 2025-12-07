#pragma once
#include <stdexcept>
#include <string>

class GameError : public std::runtime_error {
public:
    explicit GameError(const std::string& msg)
        : std::runtime_error(msg) {}
};

// Ошибка: плохое количество игроков
class InvalidPlayerCountError : public GameError {
public:
    InvalidPlayerCountError()
        : GameError("Trebuie cel putin un jucator") {}
};

// Ошибка: плохая конфигурация (например, некорректное число раундов)
class InvalidConfigError : public GameError {
public:
    explicit InvalidConfigError(const std::string& msg)
        : GameError("Configuratie invalida: " + msg) {}
};

// Ошибка: странная дистанция броска (пример отдельной категории)
class InvalidThrowDistanceError : public GameError {
public:
    InvalidThrowDistanceError()
        : GameError("Distanta aruncarii nu poate fi negativa") {}
};
