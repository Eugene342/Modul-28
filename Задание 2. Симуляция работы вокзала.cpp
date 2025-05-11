#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
std::mutex mtx;
std::condition_variable cv;
bool isStationOccupied = false;
void train(int travelTime, const std::string& trainName)
{
	std::this_thread::sleep_for(std::chrono::seconds(travelTime)); // поезд движется к вокзалу
	std::unique_lock<std::mutex> lock(mtx);
	std::cout << "The train " << trainName << " arrived at the station." << std::endl; // поезд прибывает на вокзал
	cv.wait(lock, [] { return !isStationOccupied; }); // ожидание свободного места на вокзале
	isStationOccupied = true; // занимаем вокзал
	std::cout << "The train " << trainName << " occupied the station." << std::endl;
	std::string command;
	while (true)
	{
		std::cout << "Enter 'depart' for sending the train " << trainName << ": ";
		std::cin >> command; // ожидаем команды на отправку с вокзала
		if (command == "depart")
		{
			break;
		}
	}
	std::cout << "The train " << trainName << " leaves the station." << std::endl; // поезд покидает вокзал
	isStationOccupied = false;
	cv.notify_all(); // уведомляем другие поезда, что вокзал свободен
}
int main()
{
	int travelTimes[3];
	std::string trainNames[3] = { "A", "B", "C" };
	std::thread trains[3];
	for(int i = 0; i < 3; i++) // ввод времени в пути для каждого поезда
	{
		std::cout << "Enter the travel time for the train " << trainNames[i] << " in seconds: ";
		std::cin >> travelTimes[i];
	}
	for (int i = 0; i < 3; i++) // запускаем потоки для каждого поезда
	{
		trains[i] = std::thread(train, travelTimes[i], trainNames[i]);
	}
	for (int i = 0; i < 3; i++) // ждём завершения всех потоков
	{
		trains[i].join();
	}
	std::cout << "All trains visited the station. The program is completed." << std::endl;
	return 0;
}

