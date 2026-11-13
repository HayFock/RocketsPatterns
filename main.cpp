#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>  // rand, srand
#include <clocale>  // setlocale
#include <list>     // std::list - для демонстрации адаптера
#include "PatternTemplates.h"

using namespace std;


class Rockets // Родительский (базовый) класс "ракеты"
{
private: // "Закрытые" компоненты (не доступны в унаследованных классах)
    double Mass;
    double Capacity;
    string FuelType;
    double MaxRange;

protected: // "Защищенные" компоненты (доступны в унаследованных классах, но не доступны внешнему наблюдателю)
    bool isReady;

public: // "Открытые" компоненты, определяющие интерфейс класса

    Rockets(double M, double C, const string& fuel, double range); // Конструктор
    virtual ~Rockets(); // Деструктор (объявлен виртуальным, чтобы обеспечить корректное уничтожение унаследованных классов)

    // Функция с реализацией
    double GetMass() const { return Mass; }
    double GetCapacity() const { return Capacity; }
    string GetFuelType() const { return FuelType; }
    double GetMaxRange() const { return MaxRange; }

    // Функция с реализацией, которая может быть переопределена в унаследованном классе
    virtual void Launch() = 0; // запуск ракеты
    virtual double CalculateEfficiency() = 0; // эффективность
    virtual void DisplayInfo() = 0; // вывод информации

};

// Реализация конструктора
Rockets::Rockets(double M, double C, const string& fuel, double range) : Mass(M), Capacity(C), FuelType(fuel), MaxRange(range), isReady(true)
{
    cout << "Creating rocket..." << endl;
}

// Реализация деструктора
Rockets::~Rockets()
{
    cout << "Deleting rocket..." << endl;
}

class OrbitalRocket : public Rockets // Класс-наследник "Орбитальная ракета"
{
private:
    double OrbitAltitude;

public:
    OrbitalRocket(double M, double C, const string& fuel, double range, double alt);
    ~OrbitalRocket();

    // Переопределение функции в унаследованном классе (функция не виртуальная!)
    void Launch() override;
    double CalculateEfficiency() override;
    void DisplayInfo() override;


};

// Реализация конструктора
OrbitalRocket::OrbitalRocket(double M, double C, const string& fuel, double range, double alt) : Rockets(M, C, fuel, range), OrbitAltitude(alt) //
{

    isReady = true; // Инициализация isReady через присваивание

    cout << "Creating OrbitalRocket..." << endl;
}

// Реализация деструктора
OrbitalRocket::~OrbitalRocket()
{
    cout << "Deleting OrbitalRocket..." << endl;
}

void OrbitalRocket::Launch()
{
    cout << "Orbital rocket is launching! Into orbit altitude " << OrbitAltitude << " км." << endl;
}

double OrbitalRocket::CalculateEfficiency()
{
    // Эффективность = (грузоподъёмность / масса) * (1 + высота/1000)
    return (GetCapacity() / GetMass()) * (1.0 + OrbitAltitude / 1000.0);
}

void OrbitalRocket::DisplayInfo()
{
    cout << "--- OrbitalRocket---" << endl;
    cout << "Mass: " << GetMass() << " t"<< endl;
    cout << "Capacity: " << GetCapacity() << " t"<< endl;
    cout << "Fuel: " << GetFuelType() << endl;
    cout << "Range: " << GetMaxRange() << " ths. km"<< endl;
    cout << "Orbit altitude: " << OrbitAltitude << " km"<< endl;
    cout << "Efficiency: " << CalculateEfficiency() << endl;
}

class InterPlanetaryRocket : public Rockets
{
private:
    string DestinationPlanet;
    double RequiredV;

public:
    InterPlanetaryRocket(double M, double C, const string& fuel, double range, const string& planet, double V);
    ~InterPlanetaryRocket();

    void Launch() override;
    double CalculateEfficiency() override;
    void DisplayInfo() override;
};

InterPlanetaryRocket::InterPlanetaryRocket(double M, double C, const string& fuel, double range, const string& planet, double V) : Rockets(M, C, fuel, range), DestinationPlanet(planet), RequiredV(V)
{
    isReady = true;

    cout << "Creating InterPlanetaryRocket..." << endl;
}

InterPlanetaryRocket::~InterPlanetaryRocket()
{
    cout << "Deleting InterPlanetaryRocket..." << endl;
}

void InterPlanetaryRocket::Launch()
{
    cout << "Planetary rocket is launching! Destination planet " << DestinationPlanet << " Required speed " << RequiredV << " км/с." << endl;
}

double InterPlanetaryRocket::CalculateEfficiency()
{
    // Эффективность = грузоподъёмность / (масса * требуемая скорость)
    return GetCapacity() / (GetMass() * RequiredV);
}

void InterPlanetaryRocket::DisplayInfo()
{
    cout << "--- InterPlanetaryRocket---" << endl;
    cout << "Mass: " << GetMass() << " t"<< endl;
    cout << "Capacity: " << GetCapacity() << " t"<< endl;
    cout << "Fuel: " << GetFuelType() << endl;
    cout << "Range: " << GetMaxRange() << " ths. km"<< endl;
    cout << "Planet: " << DestinationPlanet << endl;
    cout << "Speed: " << RequiredV << " km/s" << endl;
    cout << "Efficiency: " << CalculateEfficiency() << endl;
}

class ReusableRocket : public Rockets
{
private:
    int FlightDone;
    int MaxFlights;

public:
    ReusableRocket(double M, double C, const string& fuel, double range, int maxFl);
    ~ReusableRocket();

    void Launch() override;
    double CalculateEfficiency() override;
    void DisplayInfo() override;

    // Метод для увеличения счетчика полётов
    void IncrementFlights();
};

ReusableRocket::ReusableRocket(double M, double C, const string& fuel, double range, int maxFl) : Rockets(M, C, fuel, range), FlightDone(0), MaxFlights(maxFl)
{
    isReady = true;

    cout << "Creating ReusableRocket..." << endl;
}

ReusableRocket::~ReusableRocket()
{
    cout << "Deleting ReusableRocket..." << endl;
}

void ReusableRocket::Launch()
{
    cout << "Reusable rocket is launching! Flight № " << (FlightDone + 1) << ". Max flights " << MaxFlights << endl;
}

double ReusableRocket::CalculateEfficiency()
{
    // Эффективность повышается с каждым полётом
    double ReuseBonus = 1.0 + (static_cast<double>(FlightDone) / MaxFlights);
    return (GetCapacity() / GetMass()) * ReuseBonus;
}

void ReusableRocket::DisplayInfo()
{
    cout << "--- ReusableRocket---" << endl;
    cout << "Mass: " << GetMass() << " t"<< endl;
    cout << "Capacity: " << GetCapacity() << " t"<< endl;
    cout << "Fuel: " << GetFuelType() << endl;
    cout << "Range: " << GetMaxRange() << " ths. km"<< endl;
    cout << "Flights: " << FlightDone << endl;
    cout << "Max Flights: " << MaxFlights << endl;
    cout << "Efficiency: " << CalculateEfficiency() << endl;
}

void ReusableRocket::IncrementFlights()
{
    if (FlightDone < MaxFlights)
        FlightDone++;
}

// Фабричный метод
enum class RocketType : int
{
    Orbital = 1,
    InterPlanetary = 2,
    Reusable = 3,
    Undefined = 0
};

// Фабрика: создаёт ракету заданного типа с характерными параметрами
Rockets* CreateRocket(RocketType type)
{
    Rockets* newRocket = nullptr;

    if (type == RocketType::Orbital)
    {
        newRocket = new OrbitalRocket(300.0 + rand() % 50,
                                      5.0 + rand() % 5,
                                      "kerosene + O2", 15.0,
                                      500.0 + rand() % 400);
    }
    else if (type == RocketType::InterPlanetary)
    {
        newRocket = new InterPlanetaryRocket(4000.0 + rand() % 2000,
                                             80.0 + rand() % 40,
                                             "methane + O2", 800.0,
                                             "Mars", 6.0 + (rand() % 10) / 10.0);
    }
    else if (type == RocketType::Reusable)
    {
        newRocket = new ReusableRocket(500.0 + rand() % 100,
                                       20.0 + rand() % 10,
                                       "RP-1 + O2", 20.0,
                                       15 + rand() % 10);
    }

    return newRocket;
}

// ===================== Декораторы итераторов =====================
// Каждый декоратор "оборачивает" другой итератор и пропускает ракеты,
// которые не подходят под условие. Для кода, который обходит контейнер,
// ничего не меняется: он по-прежнему видит обычный Iterator<Rockets*>.

// Декоратор 1: оставляет только ракеты с заданным типом топлива
class RocketFuelDecorator : public IteratorDecorator<Rockets*>
{
private:
    string TargetFuel;

    // Подходит ли текущая ракета под условие?
    bool IsSuitable() const { return It->GetCurrent()->GetFuelType() == TargetFuel; }

public:
    RocketFuelDecorator(Iterator<Rockets*>* it, const string& fuel)
    : IteratorDecorator<Rockets*>(it), TargetFuel(fuel) {}

    void First() override
    {
        It->First();                           // встали в начало внутреннего итератора
        while (!It->IsDone() && !IsSuitable()) // пока не конец и ракета не подходит...
            It->Next();                        // ...пропускаем её
    }

    void Next() override
    {
        do
        {
            It->Next();                           // шаг вперёд (обязательно хотя бы один)
        } while (!It->IsDone() && !IsSuitable()); // и дальше пропускаем неподходящие
    }
};

// Декоратор 2: оставляет только ракеты с массой не меньше заданной
class RocketMassDecorator : public IteratorDecorator<Rockets*>
{
private:
    double MinMass;

    bool IsSuitable() const { return It->GetCurrent()->GetMass() >= MinMass; }

public:
    RocketMassDecorator(Iterator<Rockets*>* it, double minMass)
    : IteratorDecorator<Rockets*>(it), MinMass(minMass) {}

    void First() override
    {
        It->First();
        while (!It->IsDone() && !IsSuitable())
            It->Next();
    }

    void Next() override
    {
        do
        {
            It->Next();
        } while (!It->IsDone() && !IsSuitable());
    }
};

// Декоратор 3: оставляет только ракеты с эффективностью не меньше заданной
class RocketEfficiencyDecorator : public IteratorDecorator<Rockets*>
{
private:
    double MinEfficiency;

    bool IsSuitable() const { return It->GetCurrent()->CalculateEfficiency() >= MinEfficiency; }

public:
    RocketEfficiencyDecorator(Iterator<Rockets*>* it, double minEfficiency)
    : IteratorDecorator<Rockets*>(it), MinEfficiency(minEfficiency) {}

    void First() override
    {
        It->First();
        while (!It->IsDone() && !IsSuitable())
            It->Next();
    }

    void Next() override
    {
        do
        {
            It->Next();
        } while (!It->IsDone() && !IsSuitable());
    }
};

// Функция для обработки любого контейнера через итератор
void ProcessAllRockets(Iterator<Rockets*>* it)
{
    for (it->First(); !it->IsDone(); it->Next())
    {
        Rockets* current = it->GetCurrent();
        current->DisplayInfo();
        cout << "------------------------" << endl;
    }
}

// Краткий вывод: одна строка на ракету (чтобы результаты фильтрации было удобно читать)
void PrintRocketsBrief(Iterator<Rockets*>* it)
{
    int count = 0;
    for (it->First(); !it->IsDone(); it->Next())
    {
        Rockets* r = it->GetCurrent();
        cout << "  " << r->GetFuelType() << " | mass " << r->GetMass()
             << " t | efficiency " << r->CalculateEfficiency() << endl;
        ++count;
    }
    cout << "  Total: " << count << endl;
}

// Работает с ЛЮБЫМ контейнером, унаследованным от ContainerBase:
// стек это, массив или что-то ещё - функция этого не знает и ей это не нужно
void ShowContainer(ContainerBase<Rockets*>* cont)
{
    Iterator<Rockets*>* it = cont->GetIterator(); // контейнер сам создаёт подходящий итератор
    PrintRocketsBrief(it);
    delete it; // итератор создан через new - удаляем
}

// Освобождение памяти: удаляет все ракеты из любого нашего контейнера
void DeleteRocketsFromContainer(ContainerBase<Rockets*>* cont)
{
    Iterator<Rockets*>* it = cont->GetIterator();
    for (it->First(); !it->IsDone(); it->Next())
        delete it->GetCurrent();
    delete it;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    srand(static_cast<unsigned>(time(nullptr)));

    // Демонстрация фабричного метода (создадим 3 случайные ракеты)
    cout << "=== Fabric method ===" << endl;
    for (int i = 0; i < 3; ++i)
    {
        RocketType randType = static_cast<RocketType>(1 + rand() % 3);
        Rockets* r = CreateRocket(randType);
        r->DisplayInfo();
        cout << endl;
        delete r;
    }

    // Создание контейнеров: два наших и один стандартный STL-список
    const size_t NUM_ROCKETS = 10;
    StackClass<Rockets*> rocketStack;
    ArrayClass<Rockets*> rocketArray;
    list<Rockets*> rocketList;

    // Наполнение случайными ракетами.
    // У каждого контейнера СВОИ объекты, чтобы потом не удалить один объект дважды.
    cout << endl;
    for (size_t i = 0; i < NUM_ROCKETS; ++i)
    {
        rocketStack.Push(CreateRocket(static_cast<RocketType>(1 + rand() % 3)));
        rocketArray.Add(CreateRocket(static_cast<RocketType>(1 + rand() % 3)));
        rocketList.push_back(CreateRocket(static_cast<RocketType>(1 + rand() % 3)));
    }

    // ---------- Задание 1: общий базовый класс и GetIterator ----------
    cout << endl << "=== Task 1: containers share ContainerBase, GetIterator() ===" << endl;
    ContainerBase<Rockets*>* containers[2] = { &rocketStack, &rocketArray };
    const char* names[2] = { "Stack", "Array" };
    for (int i = 0; i < 2; ++i)
    {
        cout << names[i] << " (" << containers[i]->Size() << " rockets):" << endl;
        ShowContainer(containers[i]); // одна и та же функция для разных контейнеров
    }

    // ---------- Задание 2: декораторы итераторов (на массиве) ----------
    cout << endl << "=== Task 2: iterator decorators ===" << endl;

    cout << "-- Decorator 1: only methane + O2 --" << endl;
    Iterator<Rockets*>* fuelIt = new RocketFuelDecorator(rocketArray.GetIterator(), "methane + O2");
    PrintRocketsBrief(fuelIt);
    delete fuelIt;

    cout << "-- Decorator 2: mass >= 500 t --" << endl;
    Iterator<Rockets*>* massIt = new RocketMassDecorator(rocketArray.GetIterator(), 500.0);
    PrintRocketsBrief(massIt);
    delete massIt;

    cout << "-- Decorator 3: efficiency >= 0.03 --" << endl;
    Iterator<Rockets*>* effIt = new RocketEfficiencyDecorator(rocketArray.GetIterator(), 0.03);
    PrintRocketsBrief(effIt);
    delete effIt;

    // Декораторы можно вкладывать друг в друга: условия работают одновременно ("И")
    cout << "-- All three at once: RP-1 + O2, mass >= 550 t, efficiency >= 0.04 --" << endl;
    Iterator<Rockets*>* chainIt =
        new RocketEfficiencyDecorator(
            new RocketMassDecorator(
                new RocketFuelDecorator(rocketArray.GetIterator(), "RP-1 + O2"),
                550.0),
            0.04);
    PrintRocketsBrief(chainIt);
    delete chainIt; // удалит всю цепочку, включая внутренний итератор массива

    // ---------- Задание 3: адаптер STL-итератора (на std::list) ----------
    cout << endl << "=== Task 3: STL iterator adapter (std::list) ===" << endl;

    cout << "-- Plain traversal of std::list via adapter --" << endl;
    Iterator<Rockets*>* listIt = new ConstIteratorAdapter<list<Rockets*>, Rockets*>(&rocketList);
    PrintRocketsBrief(listIt);
    delete listIt;

    // Адаптер + декораторы: список умеет всё то же, что и наши контейнеры
    cout << "-- Adapter + decorators: mass >= 500 t and efficiency >= 0.03 --" << endl;
    Iterator<Rockets*>* adaptedIt = new ConstIteratorAdapter<list<Rockets*>, Rockets*>(&rocketList);
    Iterator<Rockets*>* adaptedChainIt =
        new RocketEfficiencyDecorator(new RocketMassDecorator(adaptedIt, 500.0), 0.03);
    ProcessAllRockets(adaptedChainIt);
    delete adaptedChainIt; // adaptedIt удалять не надо: его удалит декоратор

    // Очистка памяти
    DeleteRocketsFromContainer(&rocketStack);
    DeleteRocketsFromContainer(&rocketArray);
    for (Rockets* r : rocketList)
        delete r;

    return 0;
}
