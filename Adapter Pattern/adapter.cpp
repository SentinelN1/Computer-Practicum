#define AvogadroConstant 6.022e23
#define BoltzmanConstant 1.380e-23
#define GasConstant 8.314

#include <iostream>

class Adaptee
{
private:
    double volume;
    double mass;
    double molar;

public:
    Adaptee(const double &vol, const double &mas, const double &mol)
    {
        volume = vol;
        mass = mas;
        molar = mol;
    }

    double AmountOfMatter()
    {
        return mass / molar;
    }

    double GetPressure(const int &temperature)
    {
        return AmountOfMatter() * GasConstant * temperature / volume;
    }

    std::string ToString()
    {
        std::string str;
        str += "volume = " + std::to_string(volume) + " m^3; ";
        str += "mass = " + std::to_string(mass) + " kg; ";
        str += "molar mass = " + std::to_string(molar) + " kg / mol.";
        return str;
    }

    void ModifMass(const double &dm)
    {
        mass += dm;
    }
};

class Target
{
public:
    virtual ~Target() {}
    virtual double CalculateDp(const int &, const int &) = 0;
    virtual void ModifMass(const double &) = 0;
    virtual std::string GetData() = 0;
};

class Adapter : public Target
{
private:
    Adaptee *p_gas;

public:
    Adapter(Adaptee *gas)
    {
        p_gas = gas;
    }

    ~Adapter()
    {
        if (p_gas)
        {
            delete p_gas;
        }
    }

    double CalculateDp(const int &T0, const int &dT)
    {
        return p_gas->GetPressure(T0 + dT) - p_gas->GetPressure(T0);
    }

    void ModifMass(const double &dm)
    {
        p_gas->ModifMass(dm);
    }

    std::string GetData()
    {
        return p_gas->ToString();
    }
};

int main()
{
    Adaptee gas(1, 1, 1);
    Target *t = new Adapter(&gas);

    std::cout << t->CalculateDp(250, 50) << "\n";
    t->ModifMass(99);
    std::cout << t->GetData() << "\n";

    delete t;
}
