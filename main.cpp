#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>  // rand, srand
#include <clocale>  // setlocale
#include <list>     // std::list - для демонстрации адаптера
#include "PatternTemplates.h"

using namespace std;


// ===================== Паттерн "Стратегия": способы запуска ракеты =====================

// 1. Перечень способов запуска (можно расширять)
enum class LaunchMannerEnum : int
{
    Vertical,    // вертикальный старт с наземной площадки
    AirDrop,     // воздушный старт: ракета сбрасывается с самолёта-носителя
    SeaPlatform, // старт с морской платформы

    None
};

// 2. Абстрактная стратегия: общий интерфейс для всех способов запуска
class LaunchStrategy
{
public:
    virtual ~LaunchStrategy() {}
    virtual void Launch() = 0; // "как именно стартуем"
};

// 3. Конкретные стратегии (каждая - один способ запуска)
class VerticalLaunchStrategy : public LaunchStrategy
{
public:
    void Launch() override { cout << "Vertical launch from the ground pad..."; }
};

class AirDropLaunchStrategy : public LaunchStrategy
{
public:
    void Launch() override { cout << "Air launch: dropped from a carrier aircraft..."; }
};

class SeaPlatformLaunchStrategy : public LaunchStrategy
{
public:
    void Launch() override { cout << "Launch from a floating sea platform..."; }
};

// 4. Фабричный метод для создания стратегий
LaunchStrategy* CreateLaunchStrategy(LaunchMannerEnum manner)
{
    switch (manner)
    {
        case LaunchMannerEnum::Vertical:    return new VerticalLaunchStrategy;
        case LaunchMannerEnum::AirDrop:     return new AirDropLaunchStrategy;
        case LaunchMannerEnum::SeaPlatform: return new SeaPlatformLaunchStrategy;

        default: return nullptr;
    }
}

// ===================== Семейство ракет =====================

class Rockets // Родительский (базовый) класс "ракеты"
{
private: // "Закрытые" компоненты (не доступны в унаследованных классах)
    double Mass;
    double Capacity;
    string FuelType;
    double MaxRange;

    // [НОВОЕ] Стратегия запуска. Ракета владеет этим объектом и удаляет его в деструкторе.
    LaunchStrategy* LaunchManner;

    // [НОВОЕ] Один из шагов алгоритма Launch(): запуск выбранным способом (общий код)
    void DoLaunchUsingStrategy();

protected: // "Защищенные" компоненты (доступны в унаследованных классах, но не доступны внешнему наблюдателю)
    bool isReady;

    // [НОВОЕ] Паттерн "Шаблонный метод": шаги алгоритмов Launch() и DisplayInfo().
    // Общую структуру алгоритмов задаёт базовый класс, а эти шаги
    // каждая ракета реализует по-своему (поэтому они чисто виртуальные).
    virtual void PrintType() const = 0;     // вывести название типа ракеты
    virtual void PrepareFlightPlan() = 0;   // шаг Launch(): вывести план полёта
    virtual void PrintSpecificInfo() = 0;   // шаг DisplayInfo(): вывести данные, специфичные для типа

    // [НОВОЕ] "Хук" (необязательный шаг): по умолчанию ничего не делает,
    // наследник может переопределить, если ему нужно что-то сделать после запуска
    virtual void AfterLaunch() {}

public: // "Открытые" компоненты, определяющие интерфейс класса

    Rockets(double M, double C, const string& fuel, double range); // Конструктор
    virtual ~Rockets(); // Деструктор (объявлен виртуальным, чтобы обеспечить корректное уничтожение унаследованных классов)

    // Функция с реализацией
    double GetMass() const { return Mass; }
    double GetCapacity() const { return Capacity; }
    string GetFuelType() const { return FuelType; }
    double GetMaxRange() const { return MaxRange; }

    // [НОВОЕ] Установить/снять признак "готова к запуску"
    void SetReady(bool ready) { isReady = ready; }

    // [НОВОЕ] Задать способ запуска: можно передать готовую стратегию...
    void SetLaunchManner(LaunchStrategy* manner);
    // ...а можно просто указать значение enum (стратегия создаётся фабричным методом)
    void SetLaunchManner(LaunchMannerEnum manner);

    // [ИЗМЕНЕНО] Launch() и DisplayInfo() больше НЕ виртуальные!
    // Это шаблонные методы: структура алгоритма зафиксирована здесь, в базовом классе.
    // Сигнатуры остались прежними, поэтому остальной код (ProcessAllRockets и др.) не менялся.
    void Launch();      // запуск ракеты
    void DisplayInfo(); // вывод информации

    virtual double CalculateEfficiency() = 0; // эффективность
};

// Реализация конструктора
Rockets::Rockets(double M, double C, const string& fuel, double range) : Mass(M), Capacity(C), FuelType(fuel), MaxRange(range), LaunchManner(nullptr), isReady(true)
{
    cout << "Creating rocket..." << endl;
}

// Реализация деструктора
Rockets::~Rockets()
{
    cout << "Deleting rocket..." << endl;
    delete LaunchManner; // стратегия принадлежит ракете, поэтому удаляем её здесь
}

void Rockets::SetLaunchManner(LaunchStrategy* manner)
{
    if (manner == LaunchManner) return;

    delete LaunchManner; // старую стратегию удаляем, чтобы не было утечки памяти
    LaunchManner = manner;
}

void Rockets::SetLaunchManner(LaunchMannerEnum manner)
{
    SetLaunchManner(CreateLaunchStrategy(manner));
}

void Rockets::DoLaunchUsingStrategy()
{
    if (LaunchManner == nullptr)
    {
        // Способ запуска не задан, ничего не делаем
        cout << "No launch method selected!";
        return;
    }

    LaunchManner->Launch(); // запуск выполняет стратегия (какая именно - базовый класс не знает)
}

// [НОВОЕ] ШАБЛОННЫЙ МЕТОД Launch(): общий "скелет" запуска для всех ракет
void Rockets::Launch()
{
    // 1. Вывести название ракеты (шаг реализуется в наследнике)
    PrintType();
    cout << " : ";

    // 2. Проверить готовность (общий код, одинаковый для всех ракет)
    if (!isReady)
    {
        cout << "NOT READY, launch cancelled" << endl;
        return;
    }
    cout << "READY : ";

    // 3. Вывести план полёта (шаг реализуется в наследнике)
    PrepareFlightPlan();
    cout << " : ";

    // 4. Запустить выбранным способом (стратегия)
    DoLaunchUsingStrategy();

    // 5. Хук: наследник может что-то сделать после запуска
    AfterLaunch();

    // 6. Конец алгоритма
    cout << endl;
}

// [НОВОЕ] ШАБЛОННЫЙ МЕТОД DisplayInfo(): общий "скелет" вывода информации.
// Раньше четыре одинаковые строки (масса, грузоподъёмность, топливо, дальность)
// дублировались в каждом классе-наследнике, теперь они выводятся здесь один раз.
void Rockets::DisplayInfo()
{
    // 1. Заголовок с названием типа (шаг реализуется в наследнике)
    cout << "--- ";
    PrintType();
    cout << " ---" << endl;

    // 2. Общие данные (одинаковые для всех ракет)
    cout << "Mass: " << GetMass() << " t" << endl;
    cout << "Capacity: " << GetCapacity() << " t" << endl;
    cout << "Fuel: " << GetFuelType() << endl;
    cout << "Range: " << GetMaxRange() << " ths. km" << endl;

    // 3. Данные, специфичные для типа ракеты (шаг реализуется в наследнике)
    PrintSpecificInfo();

    // 4. Эффективность (тоже считается по-разному, но выводится одинаково)
    cout << "Efficiency: " << CalculateEfficiency() << endl;
}

class OrbitalRocket : public Rockets // Класс-наследник "Орбитальная ракета"
{
private:
    double OrbitAltitude;

protected:
    // [ИЗМЕНЕНО] Вместо Launch() и DisplayInfo() реализуем только "шаги" алгоритмов
    void PrintType() const override;
    void PrepareFlightPlan() override;
    void PrintSpecificInfo() override;

public:
    OrbitalRocket(double M, double C, const string& fuel, double range, double alt);
    ~OrbitalRocket();

    double CalculateEfficiency() override;
};

// Реализация конструктора
OrbitalRocket::OrbitalRocket(double M, double C, const string& fuel, double range, double alt) : Rockets(M, C, fuel, range), OrbitAltitude(alt)
{
    isReady = true; // Инициализация isReady через присваивание

    // [НОВОЕ] Способ запуска по умолчанию для орбитальной ракеты
    SetLaunchManner(LaunchMannerEnum::AirDrop);

    cout << "Creating OrbitalRocket..." << endl;
}

// Реализация деструктора
OrbitalRocket::~OrbitalRocket()
{
    cout << "Deleting OrbitalRocket..." << endl;
}

void OrbitalRocket::PrintType() const
{
    cout << "OrbitalRocket";
}

void OrbitalRocket::PrepareFlightPlan()
{
    cout << "Target orbit altitude " << OrbitAltitude << " km";
}

void OrbitalRocket::PrintSpecificInfo()
{
    cout << "Orbit altitude: " << OrbitAltitude << " km" << endl;
}

double OrbitalRocket::CalculateEfficiency()
{
    // Эффективность = (грузоподъёмность / масса) * (1 + высота/1000)
    return (GetCapacity() / GetMass()) * (1.0 + OrbitAltitude / 1000.0);
}

class InterPlanetaryRocket : public Rockets
{
private:
    string DestinationPlanet;
    double RequiredV;

protected:
    void PrintType() const override;
    void PrepareFlightPlan() override;
    void PrintSpecificInfo() override;

public:
    InterPlanetaryRocket(double M, double C, const string& fuel, double range, const string& planet, double V);
    ~InterPlanetaryRocket();

    double CalculateEfficiency() override;
};

InterPlanetaryRocket::InterPlanetaryRocket(double M, double C, const string& fuel, double range, const string& planet, double V) : Rockets(M, C, fuel, range), DestinationPlanet(planet), RequiredV(V)
{
    isReady = true;

    // [НОВОЕ] Способ запуска по умолчанию для межпланетной ракеты
    SetLaunchManner(LaunchMannerEnum::Vertical);

    cout << "Creating InterPlanetaryRocket..." << endl;
}

InterPlanetaryRocket::~InterPlanetaryRocket()
{
    cout << "Deleting InterPlanetaryRocket..." << endl;
}

void InterPlanetaryRocket::PrintType() const
{
    cout << "InterPlanetaryRocket";
}

void InterPlanetaryRocket::PrepareFlightPlan()
{
    cout << "Destination " << DestinationPlanet << ", required speed " << RequiredV << " km/s";
}

void InterPlanetaryRocket::PrintSpecificInfo()
{
    cout << "Planet: " << DestinationPlanet << endl;
    cout << "Speed: " << RequiredV << " km/s" << endl;
}

double InterPlanetaryRocket::CalculateEfficiency()
{
    // Эффективность = грузоподъёмность / (масса * требуемая скорость)
    return GetCapacity() / (GetMass() * RequiredV);
}

class ReusableRocket : public Rockets
{
private:
    int FlightDone;
    int MaxFlights;

protected:
    void PrintType() const override;
    void PrepareFlightPlan() override;
    void PrintSpecificInfo() override;

    // [НОВОЕ] Переопределяем хук: после каждого запуска увеличиваем счётчик полётов
    void AfterLaunch() override;

public:
    ReusableRocket(double M, double C, const string& fuel, double range, int maxFl);
    ~ReusableRocket();

    double CalculateEfficiency() override;

    // Метод для увеличения счетчика полётов
    void IncrementFlights();
};

ReusableRocket::ReusableRocket(double M, double C, const string& fuel, double range, int maxFl) : Rockets(M, C, fuel, range), FlightDone(0), MaxFlights(maxFl)
{
    isReady = true;

    // [НОВОЕ] Способ запуска по умолчанию для многоразовой ракеты
    SetLaunchManner(LaunchMannerEnum::SeaPlatform);

    cout << "Creating ReusableRocket..." << endl;
}

ReusableRocket::~ReusableRocket()
{
    cout << "Deleting ReusableRocket..." << endl;
}

void ReusableRocket::PrintType() const
{
    cout << "ReusableRocket";
}

void ReusableRocket::PrepareFlightPlan()
{
    cout << "Flight #" << (FlightDone + 1) << " of " << MaxFlights;
}

void ReusableRocket::PrintSpecificInfo()
{
    cout << "Flights: " << FlightDone << endl;
    cout << "Max Flights: " << MaxFlights << endl;
}

void ReusableRocket::AfterLaunch()
{
    IncrementFlights();
}

double ReusableRocket::CalculateEfficiency()
{
    // Эффективность повышается с каждым полётом
    double ReuseBonus = 1.0 + (static_cast<double>(FlightDone) / MaxFlights);
    return (GetCapacity() / GetMass()) * ReuseBonus;
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

// Запуск всех ракет из любого контейнера (аналог EatEmAll из примера преподавателя)
void LaunchAllRockets(Iterator<Rockets*>* it)
{
    for (it->First(); !it->IsDone(); it->Next())
    {
        Rockets* current = it->GetCurrent();
        current->Launch();
    }
}

// Задать всем ракетам из контейнера один и тот же способ запуска
void SetLaunchMannerForAll(Iterator<Rockets*>* it, LaunchMannerEnum manner)
{
    for (it->First(); !it->IsDone(); it->Next())
        it->GetCurrent()->SetLaunchManner(manner);
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

        // ---------- Задание 4: паттерны "Стратегия" и "Шаблонный метод" ----------
    cout << endl << "=== Task 4: Strategy + Template Method ===" << endl;

    // 4.1 Запуск всех ракет: у каждого типа свой способ запуска по умолчанию (стратегия)
    cout << "-- Launch all (default strategies) --" << endl;
    Iterator<Rockets*>* launchIt = rocketArray.GetIterator();
    LaunchAllRockets(launchIt);
    delete launchIt;

    // 4.2 Смена стратегии во время работы программы: интерфейс ракет не менялся
    cout << "-- Strategy changed at runtime: all rockets -> sea platform --" << endl;
    Iterator<Rockets*>* setIt = rocketArray.GetIterator();
    SetLaunchMannerForAll(setIt, LaunchMannerEnum::SeaPlatform);
    delete setIt;

    Iterator<Rockets*>* launchIt2 = rocketArray.GetIterator();
    LaunchAllRockets(launchIt2);
    delete launchIt2;

    // 4.3 Шаблонный метод: общая проверка готовности работает для любой ракеты
    cout << "-- Rocket that is not ready --" << endl;
    rocketArray.GetElement(0)->SetReady(false);
    rocketArray.GetElement(0)->Launch();

    // 4.4 Шаблонный метод DisplayInfo(): у многоразовых ракет счётчик полётов вырос
    // (хук AfterLaunch), а значит, выросла и эффективность
    cout << "-- DisplayInfo() of reusable rockets after 2 launches --" << endl;
    Iterator<Rockets*>* reusableIt = new RocketFuelDecorator(rocketArray.GetIterator(), "RP-1 + O2");
    ProcessAllRockets(reusableIt);
    delete reusableIt;

    // Очистка памяти
    DeleteRocketsFromContainer(&rocketStack);
    DeleteRocketsFromContainer(&rocketArray);
    for (Rockets* r : rocketList)
        delete r;

    return 0;
}
