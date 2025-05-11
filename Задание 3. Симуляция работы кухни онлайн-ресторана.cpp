#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <random>
#include <string>
#include <atomic>
class Order // определение класса Order
{
public:
    std::string dish;
    Order(const std::string& dish) : dish(dish) 
    {
    }
};
class Kitchen // определение класса Kitchen
{
private:
    std::queue<Order> orders;
    std::mutex mtx;
    std::atomic<int> deliveredOrders;
public:
    Kitchen() : deliveredOrders(0) 
    {
    }
    void acceptOrder(const Order& order) 
    {
        std::lock_guard<std::mutex> lock(mtx);
        orders.push(order);
        std::cout << "Order accepted: " << order.dish << std::endl; // заказ принят
        cookOrder();
    }
    void cookOrder() 
    {
        if (orders.empty()) return;
        std::thread([this]() 
            {
            std::this_thread::sleep_for(std::chrono::seconds(rand() % 11 + 5)); // 5-15 секунд
            std::lock_guard<std::mutex> lock(mtx);
            if (!orders.empty()) {
                Order order = orders.front();
                orders.pop();
                std::cout << "The dish is ready: " << order.dish << std::endl; // блюдо готово
                deliveredOrders++;
            }
            }).detach();
    }
    bool hasOrders() 
    {
        std::lock_guard<std::mutex> lock(mtx);
        return !orders.empty();
    }
    int getDeliveredOrders() 
    {
        return deliveredOrders.load();
    }
};
void courier(Kitchen& kitchen) // определение функции для курьера 
{
    while (kitchen.getDeliveredOrders() < 10) 
    {
        std::this_thread::sleep_for(std::chrono::seconds(30)); // каждые 30 секунд
        if (kitchen.hasOrders()) 
        {
            std::cout << "The courier picked up the ready meals." << std::endl; // курьер забрал готовые блюда
        }
        else 
        {
            std::cout << "There are no ready meals for the courier." << std::endl; // нет готовых блюд для курьера
        }
    }
}
void generateOrders(Kitchen& kitchen) // определение функции для генерации заказов 
{
    std::string dishes[] = { "Pizza", "Soup", "Steak", "Salad", "Sushi" };
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 4);
    for (int i = 0; i < 10; ++i) 
    {
        std::this_thread::sleep_for(std::chrono::seconds(rand() % 6 + 5)); // 5-10 секунд
        Order order(dishes[distribution(generator)]);
        kitchen.acceptOrder(order);
    }
}
int main() // основная функция
{
    srand(static_cast<unsigned int>(time(0))); // Инициализация генератора случайных чисел
    Kitchen kitchen;
    std::thread orderThread(generateOrders, std::ref(kitchen));
    std::thread courierThread(courier, std::ref(kitchen));
    orderThread.join();
    courierThread.join();
    return 0;
}



