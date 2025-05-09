#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <mutex>
#include <iomanip>
#include <chrono>
class Swimmer 
{
public:
    std::string name; // имя
    double speed; // скорость в м/с
    double distance; // пройденная дистанция
    int time_taken; // результат заплыва
    Swimmer(std::string n, double s) : name(n), speed(s), distance(0), time_taken(0) 
    {
    }
    void wait_for_finish(std::mutex& mtx) 
    {
        while (distance < 100) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Ждем 1 секунду
            distance += speed;
            time_taken++;
            mtx.lock();
            std::cout << name << " sailed through " << std::fixed << std::setprecision(2) << distance << " meters." << std::endl;
            mtx.unlock();
        }
        mtx.lock();
        std::cout << name << " finished in " << time_taken << " seconds." << std::endl;
        mtx.unlock();
    }
};
int main() 
{
    std::vector<Swimmer> swimmers;
    std::mutex mtx;
    for (int i = 0; i < 6; ++i) 
    {
        std::string name;
        double speed;
        std::cout << "Enter the name of swimmer # " << (i + 1) << ": ";
        std::cin >> name;
        std::cout << "Enter the speed of swimmer # " << (i + 1) << " in meters per second: ";
        std::cin >> speed;
        swimmers.emplace_back(name, speed);
    }
    std::vector<std::thread> threads;
    for (auto& swimmer : swimmers) 
    {
        threads.emplace_back(&Swimmer::wait_for_finish, &swimmer, std::ref(mtx));
    }
    for (auto& thread : threads) 
    {
        thread.join();
    }
    std::sort(swimmers.begin(), swimmers.end(), [](const Swimmer& a, const Swimmer& b) // Сортируем пловцов по времени
        {
        return a.time_taken < b.time_taken;
        });
    std::cout << "\nResults:" << std::endl; // выводим результаты
    for (const auto& swimmer : swimmers) 
    {
        std::cout << std::setw(12) << swimmer.name << "   " << swimmer.time_taken << std::endl;
    }
    return 0;
}
