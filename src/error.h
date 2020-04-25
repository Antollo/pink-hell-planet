#ifndef ERROR_H_
#define ERROR_H_

#include <iostream>
#include <string>

template <class... Args>
void errorAndExit [[noreturn]] (Args... args)
{
    // C++17 fold expression <3
    (std::cerr << ... << args) << std::endl;
    glfwTerminate();
    std::exit(1);
}

template <class... Args>
void error (Args... args)
{
    (std::cerr << ... << args) << std::endl;
}

#endif /* !ERROR_H_ */
