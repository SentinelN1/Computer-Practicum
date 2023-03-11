#pragma once
#include <string>
#include <memory>
#include <vector>

class password_generator
{
public:
    virtual std::string generate()
    {
    }

    virtual std::string allowed_chars()
    {
    }

    virtual size_t length()
    {
    }

    virtual void add(std::unique_ptr<password_generator>)
    {
    }
};

class composite_password_generator : password_generator
{
private:
    std::vector<std::unique_ptr<password_generator>> generators;

public:
    std::string generate()
    {
    }

    void add(std::unique_ptr<password_generator>)
    {
    }
};

class basic_password_generator : public password_generator
{
private:
    size_t len;

public:
    size_t length()
    {
        return len;
    }
};

class digit_generator : public basic_password_generator
{
public:
    std::string allowed_chars()
    {
        return "0123456789";
    }
};

class symbol_generator : public basic_password_generator
{
public:
    std::string allowed_chars()
    {
        return "!@#$%&*?";
    }
};

class upper_letter_generator : public basic_password_generator
{
public:
    std::string allowed_chars()
    {
        return "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    }
};

class lower_letter_generator : public basic_password_generator
{
public:
    std::string allowed_chars()
    {
        return "abcdefghijklmnpqrstuvwxyz";
    }
};
