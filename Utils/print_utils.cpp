template<typename T>
void displayVec(std::vector<T>* v)
{
	typename std::vector<T>::iterator it = v->begin();
	while (it != v->end())
	{
		std::cout << *it << ' ';
		++it;
	}
	std::cout << std::endl;
}
