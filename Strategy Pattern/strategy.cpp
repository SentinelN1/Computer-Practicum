#include <iostream>
#include <string>
#include <vector>

enum class article_unit
{
    piece,
    kg,
    meter,
    sqmeter,
    cbmeter,
    liter
};

class discount_type
{
public:
    virtual double discount_percent(const double &, const double &) = 0;
};

// Фиксированная скидка
class fixed_discount : public discount_type
{
private:
    double discount;

public:
    fixed_discount(const double &discount_)
    {
        discount = discount_;
    }

    double discount_percent(const double &, const double &)
    {
        std::cout << "Fixed discount is used.\n";
        return discount;
    }
};

// Оптовоя скидка
class volume_discount : public discount_type
{
private:
    double discount;
    double volume;

public:
    volume_discount(const double &volume_, const double &discount_)
    {
        volume = volume_;
        discount = discount_;
    }

    double discount_percent(const double &volume_, const double &)
    {
        if (volume_ >= volume)
        {
            std::cout << "Volume discount is used.\n";
            return discount;
        }

        return 0;
    }
};

// Скидка 15% при приобретении одного вида товара на общую сумму больше 100.000 рублей
class price_discount : public discount_type
{
private:
    double discount;
    double price;

public:
    price_discount(const double &price_, const double &discount_)
    {
        price = price_;
        discount = discount_;
    }

    double discount_percent(const double &price_, const double &)
    {
        if (price_ <= price)
        {
            std::cout << "Price discount is used.\n";
            return discount;
        }

        return 0;
    }
};

// Скидка с общей стоимости заказа (независимо от вида товаров, вошедших в заказ) от 200 000 – 7%.
class amount_discount : public discount_type
{
private:
    double discount;
    double amount;

public:
    amount_discount(const double &amount_, const double &discount_)
    {
        amount = amount_;
        discount = discount_;
    }

    double discount_percent(const double &amount_, const double &)
    {
        if (amount_ >= amount)
        {
            std::cout << "Amount discount is used.\n";
            return discount;
        }
        return 0;
    }
};

class article
{
public:
    int id;
    std::string name;
    double price;
    article_unit unit;
    discount_type *discount;
};

struct order_line
{
    article product;
    int quantity;
    discount_type *discount;
};

class customer
{
public:
    std::string name;
    discount_type *discount;

    customer(std::string name_, discount_type *discount_)
    {
        name = name_;
        discount = discount_;
    }
};

class order
{
public:
    int id;
    customer *buyer;
    std::vector<order_line> lines;
    discount_type *discount;
};

int main()
{
    // Я ничего не понимаю...

    fixed_discount fixed(0.05);
    volume_discount opt(50, 0.1);
    price_discount uni(100000, 0.15);
    amount_discount all(200000, 0.07);

    customer c1("Jason", &opt);
    customer c2("John ", &fixed);
    customer c3("Joane", &uni);

    std::cout << c1.discount->discount_percent(20, 0) << "\n";
    std::cout << c1.discount->discount_percent(50, 0) << "\n";

    std::cout << c2.discount->discount_percent(10, 0) << "\n";
    std::cout << c2.discount->discount_percent(100, 0) << "\n";

    std::cout << c3.discount->discount_percent(50000, 0) << "\n";
    std::cout << c3.discount->discount_percent(200000, 0) << "\n";

    // article a1{1, "pen", 5, article_unit::piece, nullptr};
    // article a2{2, "expensive pen", 15, article_unit::piece, &opt};
    // article a3{3, "scissors", 10, article_unit::piece, &fixed};

    // cumulative_price_calculator calc;
    // order o1{101, &c1, {{a1, 1, nullptr}}, nullptr};
}