#ifndef PatternTemplatesH
#define PatternTemplatesH

#include <cstddef> // size_t

// Шаблонный класс, реализующий паттерн "Итератор"
// (класс абстрактный, так как содержит функции без реализации)

template<typename T> // T - подставляемый тип данных (элемент контейнера)
class Iterator
{
protected:
    Iterator() {}

public:
    virtual ~Iterator() {}
    virtual void First() = 0; // Спозиционироваться на начале контейнера
    virtual void Next() = 0;  // Перейти к следующему элементу
    virtual bool IsDone() const = 0;  // Проверить, достигнут ли конец
    virtual T GetCurrent() const = 0; // Получить текущий элемент
};

// [НОВОЕ] Общий базовый класс для ВСЕХ наших контейнеров.
// Он говорит: "любой контейнер умеет сообщать свой размер и создавать итератор для себя".
// Благодаря этому с контейнером можно работать через указатель ContainerBase<T>*,
// не зная, стек это или массив.
template<typename T>
class ContainerBase
{
public:
    virtual ~ContainerBase() {}

    // Количество элементов в контейнере
    virtual size_t Size() const = 0;

    // Создать итератор для обхода этого контейнера.
    // Итератор создаётся через new, поэтому вызывающий код должен сделать delete.
    virtual Iterator<T> *GetIterator() = 0;
};

// Реализация паттерна "Итератор", предназначенная для обхода стека
// [ИЗМЕНЕНО] Теперь итератор получает не указатель на контейнер,
// а указатель на начало массива с данными и количество элементов (как у преподавателя).

template<typename T>
class StackIterator : public Iterator<T>
{
private:
    T *StackContainer; // Указатель на массив с данными стека
    size_t Pos;        // Текущее положение в массиве
    size_t Size;       // Количество элементов

public:
    StackIterator(T *container, size_t size)
    : StackContainer(container), Pos(0), Size(size) {}

    void First() override { Pos = 0; }
    void Next() override { Pos++; }
    bool IsDone() const override { return (Pos >= Size); }
    T GetCurrent() const override { return StackContainer[Pos]; }
};

// Шаблонный класс "Стек"

const size_t MaxSize = 100;

template <typename T> // T - подставляемый тип данных
class StackClass : public ContainerBase<T> // [ИЗМЕНЕНО] наследуем от общего базового класса
{
private:

    // Собственно контейнер, содержащий данные типа T
    T Items[MaxSize];

    // Индекс элемента стека, следующего за верхним
    size_t Top;

public:

    // Проверка того, является ли стек пустым
    bool IsEmpty() const { return (Top == 0); }

    // Определение размера стека (теперь это реализация виртуальной функции базового класса)
    size_t Size() const override { return Top; }

    // Поместить в стек новый элемент
    void Push(T newObject) { Items[Top++] = newObject; }

    // Извлечь из списка верхний элемент
    T Pop() { return Items[--Top]; }

    // Получение элемента по индексу
    T GetElementByIndex(size_t index) const { return Items[index]; }

    // Конструктор
    StackClass() : Top(0) {}

    // [НОВОЕ] Создать итератор для обхода стека
    Iterator<T> *GetIterator() override
    {
        return new StackIterator<T>(Items, Top);
    }
};

// Реализация паттерна "Итератор", предназначенная для обхода массива
// [ИЗМЕНЕНО] Так же, как StackIterator: получает указатель на данные и размер.

template<typename T>
class ArrayIterator : public Iterator<T>
{
private:
    const T *ArrayContainer; // Указатель на массив с данными
    size_t Pos;              // Текущее положение в массиве
    size_t Size;             // Количество элементов

public:
    ArrayIterator(T *container, size_t size)
    : ArrayContainer(container), Pos(0), Size(size) {}

    void First() override { Pos = 0; }
    void Next() override { Pos++; }
    bool IsDone() const override { return (Pos >= Size); }
    T GetCurrent() const override { return ArrayContainer[Pos]; }
};

// Шаблонный класс "Массив"

template <typename T>
class ArrayClass : public ContainerBase<T> // [ИЗМЕНЕНО] наследуем от общего базового класса
{
private:

    T Items[MaxSize];
    size_t ArraySize;

public:

    void Add(T newObject) { Items[ArraySize++] = newObject; }

    // Реализация виртуальной функции базового класса
    size_t Size() const override { return ArraySize; }

    // Первый вариант получения элемента по индексу (перегрузка оператора [])
    T operator[](size_t index) const { return Items[index]; }

    // Второй вариант получения элемента по индексу
    T GetElement(size_t index) const { return Items[index]; }

    // [НОВОЕ] Создать итератор для обхода массива
    Iterator<T> *GetIterator() override
    {
        return new ArrayIterator<T>(Items, ArraySize);
    }

    ArrayClass() : ArraySize(0) {}
};

// [НОВОЕ] Паттерн "Декоратор" для итератора (базовый, "пустой" декоратор).
// Сам декоратор - тоже Iterator, но внутри хранит другой итератор (It)
// и по умолчанию просто передаёт ему все вызовы.
// Классы-наследники переопределяют First() и Next(), чтобы пропускать ненужные элементы.

template<typename T>
class IteratorDecorator : public Iterator<T>
{
protected:
    Iterator<T> *It; // Оборачиваемый итератор

public:
    IteratorDecorator(Iterator<T> *it) : It(it) {}

    // Декоратор "владеет" внутренним итератором, поэтому удаляет его вместе с собой
    virtual ~IteratorDecorator() { delete It; }

    void First() override { It->First(); }
    void Next() override { It->Next(); }
    bool IsDone() const override { return It->IsDone(); }
    T GetCurrent() const override { return It->GetCurrent(); }
};

// [НОВОЕ] Паттерн "Адаптер": превращает STL-итераторы (begin(), end(), ++, *)
// в наш интерфейс Iterator (First, Next, IsDone, GetCurrent).
// Работает с любым STL-контейнером: list, vector, forward_list и т. д.

template<typename ContainerType, typename ItemType>
class ConstIteratorAdapter : public Iterator<ItemType>
{
protected:
    ContainerType *Container;                  // Указатель на STL-контейнер
    typename ContainerType::const_iterator It; // Текущая позиция (STL-итератор)

public:
    ConstIteratorAdapter(ContainerType *container)
    : Container(container)
    {
        It = Container->begin();
    }

    virtual ~ConstIteratorAdapter() {}

    void First() override { It = Container->begin(); }
    void Next() override { ++It; }
    bool IsDone() const override { return (It == Container->end()); }
    ItemType GetCurrent() const override { return *It; }
};

#endif // PatternTemplatesH
