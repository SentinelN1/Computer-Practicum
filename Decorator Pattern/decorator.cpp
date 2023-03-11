#include <iostream>
#include <string>
#include <algorithm>

// Abstract component
class PasswordInterface
{
protected:
    std::string password;

public:
    PasswordInterface() {}

    ~PasswordInterface() {}

    virtual std::string GetPassword()
    {
        return password;
    }

    virtual bool CheckPassword() = 0;
};

class Password : public PasswordInterface
{
public:
    Password(std::string password_)
    {
        password = password_;
    }

    ~Password() {}

    bool CheckPassword()
    {
        // Каждый пароль, как минимум, должен соответствовать требованию длины (не менее 10 символов).
        return (password.length() >= 10);
    }
};

// Abstract decorator
class PasswordChecker : public PasswordInterface
{
protected:
    PasswordInterface *passwordInterface;

public:
    PasswordChecker() {}

    ~PasswordChecker()
    {
        delete passwordInterface;
    }

    std::string GetPassword()
    {
        return passwordInterface->GetPassword();
    }
};

// Наличие хотя бы одного неалфавитного символа
class SymbolChecker : public PasswordChecker
{
public:
    SymbolChecker(PasswordInterface *passwordInteerface_)
    {
        passwordInterface = passwordInteerface_;
    }

    ~SymbolChecker() {}

    bool CheckPassword()
    {
        bool symbol = true;
        for (auto i = passwordInterface->GetPassword().begin(); i < passwordInterface->GetPassword().end(); i++)
        {
            symbol = symbol && isalpha(*i);
        }

        return passwordInterface->CheckPassword() && !symbol;
    }
};

// Наличие хотя бы одной цифры
class DigitChecker : public PasswordChecker
{
public:
    DigitChecker(PasswordInterface *passwordInteerface_)
    {
        passwordInterface = passwordInteerface_;
    }

    ~DigitChecker() {}

    bool CheckPassword()
    {
        bool digit = false;
        for (auto i = passwordInterface->GetPassword().begin(); i < passwordInterface->GetPassword().end(); i++)
        {
            digit = digit || isdigit(*i);
        }

        return passwordInterface->CheckPassword() && digit;
    }
};

// Наличие букв верхнего и нижнего регистров
class CapitalChecker : public PasswordChecker
{
public:
    CapitalChecker(PasswordInterface *passwordInteerface_)
    {
        passwordInterface = passwordInteerface_;
    }

    ~CapitalChecker() {}

    bool CheckPassword()
    {
        bool lower = false;
        bool upper = false;

        for (auto i = passwordInterface->GetPassword().begin(); i < passwordInterface->GetPassword().end(); i++)
        {
            lower = lower || islower(*i);
            upper = upper || isupper(*i);
        }

        return passwordInterface->CheckPassword() && lower && upper;
    }
};

int main()
{
    // Length check (<10)
    PasswordInterface *password1 = new Password("qwerty");
    std::cout << password1->GetPassword() << " -- " << password1->CheckPassword() << std::endl;
    delete password1;

    // Length check (>=10)
    PasswordInterface *password2 = new Password("qwertyuiop");
    std::cout << password2->GetPassword() << " -- " << password2->CheckPassword() << std::endl;

    // Symbol check (only alphabet characters)
    password2 = new SymbolChecker(password2);
    std::cout << password2->GetPassword() << " -- " << password2->CheckPassword() << std::endl;

    // Symbol check (not only alphabet characters)
    PasswordInterface *password3 = new Password("2&Tx@2M656GG");
    password3 = new SymbolChecker(password3);
    std::cout << password3->GetPassword() << " -- " << password3->CheckPassword() << std::endl;
    delete password3;

    // Digit check (without digits)
    password2 = new DigitChecker(password2);
    std::cout << password2->GetPassword() << " -- " << password2->CheckPassword() << std::endl;

    // Digit check (with digits)
    password3 = new Password("0123456789");
    password3 = new DigitChecker(password3);
    std::cout << password3->GetPassword() << " -- " << password3->CheckPassword() << std::endl;
    delete password3;

    // Lowercase and Uppercase letters check (without letters)
    password2 = new CapitalChecker(password2);
    std::cout << password2->GetPassword() << " -- " << password2->CheckPassword() << std::endl;
    delete password2;

    // Lowercase and Uppercase letters check (with letters)
    PasswordInterface *password4 = new Password("aAbBcCdDeF");
    password4 = new CapitalChecker(password4);
    std::cout << password4->GetPassword() << " -- " << password4->CheckPassword() << std::endl;
    delete password4;
}
