#pragma once
#include <algorithm>
#include <string>
#include <memory>
#include <vector>

class password_generator
{
public:
    virtual std::string generate() = 0;
    virtual std::string allowed_chars() = 0;
    virtual size_t length() = 0;
    virtual void add(password_generator *) {}
};

class basic_password_generator : public password_generator
{
protected:
    size_t len;

public:
    size_t length() { return len; }
    basic_password_generator(const size_t &l = 0) { len = l; }
    std::string generate()
    {
        std::string password(len, ' ');
        std::string allowedChars = allowed_chars();
        size_t size = allowedChars.length();
        for (size_t i = 0; i < len; i++)
        {
            password[i] = allowedChars[rand() % size];
        }
        return password;
    }
};

class digit_generator : public basic_password_generator
{
public:
    digit_generator(const size_t &l) { len = l; }
    std::string allowed_chars() { return "0123456789"; }
};

class symbol_generator : public basic_password_generator
{
public:
    symbol_generator(const size_t &l) { len = l; }
    std::string allowed_chars() { return "!@#$%&*?"; }
};

class upper_letter_generator : public basic_password_generator
{
public:
    upper_letter_generator(const size_t &l) { len = l; }
    std::string allowed_chars() { return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; }
};

class lower_letter_generator : public basic_password_generator
{
public:
    lower_letter_generator(const size_t &l) { len = l; }
    std::string allowed_chars() { return "abcdefghijklmnpqrstuvwxyz"; }
};

class composite_password_generator : password_generator
{
private:
    std::vector<password_generator *> generators;

public:
    std::string generate()
    {
        std::string password = "";
        for (password_generator *generator : generators)
        {
            password += generator->generate();
        }
        std::random_shuffle(password.begin(), password.end());
        return password;
    }

    void add(password_generator *ptr)
    {
        generators.push_back(ptr);
    }

    size_t length()
    {
        size_t l = 0;
        for (password_generator *generator : generators)
        {
            l += generator->length();
        }
        return l;
    }

    std::string allowed_chars()
    {
        std::string allowedChars = "";
        for (password_generator *generator : generators)
        {
            allowedChars += generator->allowed_chars();
        }
        return allowedChars;
    }
};
