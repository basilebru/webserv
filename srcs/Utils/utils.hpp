#ifndef UTILS_H
# define UTILS_H

# include <string>
# include <vector>
# include <iostream>
# include <sstream>
# include <cstdlib>
# include <algorithm>

/* utils.cpp */
bool			is_whitespace(char c);
void			trim_whitespace(std::string &s);
int				ft_isdigit(int c);
int				ft_isdigit_str(const char *str);
int				ft_isxdigit(int c);
int				ft_isxdigit_str(const char *str);
bool			ft_isdigit_string(const std::string& str);
std::string 	iToString(size_t size);
std::string&	to_lower(std::string& str);
std::string&	to_upper(std::string& str);


/* ip_manipulation.cpp */
unsigned int	stringToIp(std::string ip);
std::string		ipToString(unsigned int ip);
unsigned short	ft_htons(unsigned short port);
unsigned short	ft_ntohs(unsigned short port);

/* Base 64 encoding */
typedef unsigned char BYTE;
std::string base64_encode(BYTE const* buf, unsigned int bufLen);
std::vector<BYTE> base64_decode(std::string const&);

template<typename T>
void displayVec(std::vector<T> const& v, const char* separator = NULL)
{
	typename std::vector<T>::const_iterator it = v.begin();
	while (it != v.end())
	{
		std::cout << *it << separator;
		++it;
	}
	std::cout << std::endl;
}

/* Displays every container which iterator can be dereferenced */
template<typename Container>
void displayContainer(Container const& v, char const separator = ' ')
{
	typename Container::const_iterator it = v.begin();
	while (it != v.end())
	{
		std::cout << *it << separator;
		++it;
	}
	std::cout << std::endl;
}

template<typename T>
bool is_higher (const T& value1, const T& value2)
{
	return (value2 > value1);
}

template<typename T>
bool is_lower (const T& value1, const T& value2)
{
	return (value2 < value1);
}

template<typename T>
bool same_as (const T& value1, const T& value2)
{
	return (value2 == value1);
}

template <class UnaryOperator>
std::string& changeCaseString(std::string& str, UnaryOperator op)
{
	std::transform(str.begin(), str.end(),str.begin(), op);
	return str;
}

// Put vector values to ostream
template<typename InputIterator>
void putVecToOstream(std::ostream& o, InputIterator first, InputIterator last)
{
	while(first != last) {
		o << *first << ' ';
		++first;
	}
	o << std::endl;	
}

// Put map values to ostream
template<typename InputIterator>
void putMapToOstream(std::ostream& o, InputIterator first, InputIterator last)
{
	while(first != last) {
		o << first->first << "->" << first->second << ' ';
		++first;
	}
	o << std::endl;	
}

// Put map values to ostream
template<typename InputIterator>
void putLocToOstream(std::ostream& o, InputIterator first, InputIterator last)
{
	while(first != last) {
		o << std::endl << "  --> LOCATION " << first->first << std::endl;
		o << "    " << "------------" << std::endl;
		o << first->second;
		++first;
	}
}

// Print thes "state" of a value accordind to int value
template<typename Val>
void putState(std::ostream& o, Val state)
{
	if (state == 0)
		o << "off";	
	else if (state == 1)
		o << "on";	
	else if (state == -1)
		o << "NOT_SET";
	else
		o << "ERROR";

	o << std::endl;
}

#endif
