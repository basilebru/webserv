#ifndef UTILS_H
# define UTILS_H

#include <string>

bool is_whitespace(char c);
void trim_whitespace(std::string &s);
int	ft_isdigit(int c);
int	ft_isdigit_str(const char *str);
int	ft_isxdigit(int c);
int	ft_isxdigit_str(const char *str);

#endif