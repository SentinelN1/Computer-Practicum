#include <iostream>
#include "PasswordGenerator.hpp"
#include <ctime>

int main()
{
    std::srand(std::time(nullptr));

    // digit_generator basic_password_generator(8);
    // std::cout << basic_password_generator.generate() << "\n";

    digit_generator digitGenerator(4);
    std::cout << digitGenerator.generate() << "\n";

    symbol_generator symbolGenerator(4);
    std::cout << symbolGenerator.generate() << "\n";

    upper_letter_generator upperLetterGenerator(4);
    std::cout << upperLetterGenerator.generate() << "\n";

    lower_letter_generator lowerLetterGenerator(4);
    std::cout << lowerLetterGenerator.generate() << "\n";

    composite_password_generator compositeGenerator;
    compositeGenerator.add(&digitGenerator);
    compositeGenerator.add(&symbolGenerator);
    compositeGenerator.add(&upperLetterGenerator);
    compositeGenerator.add(&lowerLetterGenerator);
    std::cout << compositeGenerator.generate() << "\n";

    return 0;
}
