#include <pybind11/pybind11.h>
#include <pybind11/stl.h> 
#include "EngineBackend.h"

namespace py = pybind11;

PYBIND11_MODULE(trading_engine, m) {
    m.doc() = "C++ High-Frequency Trading Engine";

    // 1. MarketBar
    py::class_<MarketBar>(m, "MarketBar")
        .def(py::init<>())
        .def_readwrite("date", &MarketBar::date)
        .def_readwrite("price", &MarketBar::price);

    // 2. Strategy (Trampoline for Inheritance)
    class PyStrategy : public Strategy {
    public:
        using Strategy::Strategy;
        std::string check_signal(double price) override {
            PYBIND11_OVERRIDE(std::string, Strategy, check_signal, price);
        }
    };

    py::class_<Strategy, PyStrategy>(m, "Strategy")
        .def(py::init<>())
        .def("check_signal", &Strategy::check_signal);

    // 3. BacktestEngine
    py::class_<BacktestEngine>(m, "BacktestEngine")
        .def(py::init<std::vector<MarketBar>&, double, Strategy*>())
        .def("run", &BacktestEngine::run)
        // This connects the Python name "get_final_balance" to the C++ function
        .def("get_final_balance", &BacktestEngine::get_final_balance)
        .def("get_total_trades", &BacktestEngine::get_total_trades);
}