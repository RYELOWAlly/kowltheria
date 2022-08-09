#ifndef TEMPLATES_HPP
#define TEMPLATES_HPP

template <typename T>
bool vector_contains(std::vector<T> vec, const T &elem)
{
	if(find(vec.begin(), vec.end(), elem) != vec.end())
	{
		return true;
	}
	return false;
}

template <typename T>
void remove_from_vector(std::vector<T> vec, const T &elem)
{
	vec.erase(std::remove(
			  vec.begin(),
			  vec.end(),
			  elem),
		  vec.end());
}

#endif
