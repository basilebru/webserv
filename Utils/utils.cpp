#include "utils.hpp"

bool is_whitespace(char c)
{
    if (c == 32 || c == 9) // space or HTAB (cf RFC 7230 3.2.3)
        return true;
    else
        return false;
}

void trim_whitespace(std::string &s)
{
    size_t pos = s.find_first_not_of("\t ");
    s.erase(0, pos);
    pos = s.find_last_not_of("\t ");
    s.erase(pos + 1);
}

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= 57)
		return (1);
	else
		return (0);
}

int	ft_isdigit_str(const char *str)
{
	int i;

	if (str == 0)
		return (0);
	i = 0;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}