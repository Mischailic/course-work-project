#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include <string>
#include <cmath>

using namespace std;

// Константы задачи
const int NUM_APTS = 100;
const int NUM_HOURS = 24;
const int NUM_EXPERIMENTS = 1000;
const double BASE_LOAD = 0.3;        // кВт на квартиру
const double WASH_POWER = 2.0;       // кВт стиральной машины
const double OVERLOAD_LIMIT = 100.0; // предел сети

// Структура для хранения результатов одного режима
struct ModeResult {
    string name;
    vector<double> avg_hourly_power; // Средняя мощность по часам (0-23)
    double avg_max_power;            // Средняя максимальная мощность за сутки
    double avg_overload_hours;       // Среднее число часов перегрузки
    double overload_percent;         // Процент суток с перегрузкой
    double total_energy_kwh;         // Общее суточное потребление
};

ModeResult simulate_mode(const string& name, const vector<int>& start_hours) {
    ModeResult res;
    res.name = name;
    res.avg_hourly_power.resize(NUM_HOURS, 0.0);

ModeResult simulate_mode(const string& name, const vector<int>& start_hours) {
    ModeResult res;
    res.name = name;
    res.avg_hourly_power.resize(NUM_HOURS, 0.0);

    mt19937 rng(42); // Фиксированный seed для воспроизводимости
    uniform_int_distribution<> dist(0, start_hours.size() - 1);

    vector<double> sum_power(NUM_HOURS, 0.0);
    double sum_max_power = 0.0;
    double sum_overload_hours = 0.0;
    int days_with_overload = 0;
    double total_energy = 0.0;

     for (int exp = 0; exp < NUM_EXPERIMENTS; ++exp) {
         vector<double> hourly_power(NUM_HOURS, 0.0);

    // 1. Базовая нагрузка
    for (int h = 0; h < NUM_HOURS; ++h) {
        hourly_power[h] = NUM_APTS * BASE_LOAD;
    }

    // 2. Добавляем нагрузку от стиральных машин
    for (int apt = 0; apt < NUM_APTS; ++apt) {
        int start_h = start_hours[dist(rng)];

        // Машина работает 2 часа подряд
        // Проверка границ, чтобы не выйти за пределы массива (по условию старт <= 22, значит конец в 24/00)
        if (start_h < NUM_HOURS) hourly_power[start_h] += WASH_POWER;
        if (start_h + 1 < NUM_HOURS) hourly_power[start_h + 1] += WASH_POWER;
    }
    // 3. Сбор статистики за сутки
    double max_p = *max_element(hourly_power.begin(), hourly_power.end());
    int overload_count = 0;

    for (int h = 0; h < NUM_HOURS; ++h) {
        sum_power[h] += hourly_power[h];
        total_energy += hourly_power[h]; // Суммируем мощность как энергию (т.к. шаг 1 час)
            
            if (hourly_power[h] > OVERLOAD_LIMIT) {
                overload_count++;
            }
        }
        sum_max_power += max_p;
        sum_overload_hours += overload_count;
        if (overload_count > 0) days_with_overload++;
    }
// 4. Усреднение результатов
    for (int h = 0; h < NUM_HOURS; ++h) {
        res.avg_hourly_power[h] = sum_power[h] / NUM_EXPERIMENTS;
    }
    res.avg_max_power = sum_max_power / NUM_EXPERIMENTS;
    res.avg_overload_hours = sum_overload_hours / NUM_EXPERIMENTS;
    res.overload_percent = (static_cast<double>(days_with_overload) / NUM_EXPERIMENTS) * 100.0;
    res.total_energy_kwh = total_energy / NUM_EXPERIMENTS;

    return res;
}

//Вывод таблицы сравнения режимов

void print_comparison_table(const vector<ModeResult>& results) {
    cout << "\n=== ТАБЛИЦА СРАВНЕНИЯ РЕЖИМОВ ===" << endl;
    cout << left << setw(35) << "Параметр" 
         << setw(20) << "Режим 1 (18-20)" 
         << setw(20) << "Режим 2 (19-21)" 
         << setw(20) << "Режим 3 (20-22)" << endl;
    cout << string(95, '-') << endl;

    auto fmt = [](double val) {
        stringstream ss;
        ss << fixed << setprecision(2) << val;
        return ss.str();
    };
    cout << left << setw(35) << "Средняя макс. мощность (кВт):" 
         << setw(20) << fmt(results[0].avg_max_power) 
         << setw(20) << fmt(results[1].avg_max_power) 
         << setw(20) << fmt(results[2].avg_max_power) << endl;

    cout << left << setw(35) << "Ср. число часов перегрузки:" 
         << setw(20) << fmt(results[0].avg_overload_hours) 
         << setw(20) << fmt(results[1].avg_overload_hours) 
         << setw(20) << fmt(results[2].avg_overload_hours) << endl;

    cout << left << setw(35) << "% суток с перегрузкой:" 
         << setw(20) << (fmt(results[0].overload_percent) + "%") 
         << setw(20) << (fmt(results[1].overload_percent) + "%") 
         << setw(20) << (fmt(results[2].overload_percent) + "%") << endl;

    cout << left << setw(35) << "Суточное потребление (кВт*ч):" 
         << setw(20) << fmt(results[0].total_energy_kwh) 
         << setw(20) << fmt(results[1].total_energy_kwh) 
}

//Проверка корректности расчетов мощности
void verify_calculations() {
    cout << "\n=== ПРОВЕРКА РАСЧЕТА МОЩНОСТИ ===" << endl;
    cout << "Базовая нагрузка (100 кв. * 0.3 кВт): " << NUM_APTS * BASE_LOAD << " кВт" << endl;
    cout << "Работает 1 машина (База + 2 кВт):     " << NUM_APTS * BASE_LOAD + WASH_POWER << " кВт" << endl;
    cout << "Работает 10 машин (База + 20 кВт):    " << NUM_APTS * BASE_LOAD + 10 * WASH_POWER << " кВт" << endl;
    cout << "Предел перегрузки сети:               " << OVERLOAD_LIMIT << " кВт" << endl;
}

int main() {
    // 1. Проверка расчетов
    verify_calculations(); 

    // 2. Определение режимов (разрешенные часы старта)

    // Режим 1: Запуск с 18:00 до 20:00
    vector<int> mode1_starts = {18, 19, 20};
    // Режим 2: Запуск с 19:00 до 21:00
    vector<int> mode2_starts = {19, 20, 21};
    // Режим 3: Запуск с 20:00 до 22:00
    vector<int> mode3_starts = {20, 21, 22};

    // 3. Запуск симуляций
    cout << "\nЗапуск симуляций (по " << NUM_EXPERIMENTS << " экспериментов)..." << endl;

    ModeResult res1 = simulate_mode("Режим 1 (Старт 18-20)", mode1_starts);
    ModeResult res2 = simulate_mode("Режим 2 (Старт 19-21)", mode2_starts);
    ModeResult res3 = simulate_mode("Режим 3 (Старт 20-22)", mode3_starts);

   vector<ModeResult> all_results = {res1, res2, res3};

   // 4. Вывод таблицы сравнения
   print_comparison_table(all_results);

   // 5. Вывод графиков для первого и последнего режима для сравнения сдвига пика print_graph(res1);
    print_graph(res1);
    print_graph(res3);

    // 6. Итоговый вывод
    cout << endl;
    cout << "1. Общая сумма нагрузки не меняется" << endl;
    cout << "2. "Пиковая нагрузка одинакова" << endl;
    cout << "3. Суточное потребление энергии во всех режимах одинакова 
         << fixed << setprecision(0) << res1.total_energy_kwh << " кВт*ч)." << endl;
    cout << "4. Чтобы уменьшить риск перегрузки, необходимо значительно расширить окно запуска" << endl;
    return 0;
}
