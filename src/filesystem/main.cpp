#include <iostream>
#include <cstddef>
#include <zi/utility/for_each.hpp>
#include <vector>
#include "storage_client.hpp"
#include <sstream>

int main()
{
    storage_client<std::string,int> s;


    int size = 10;
    int i;
    std::vector<std::string> keys;
    for(i = 0;i<size;i++)
    {
	std::stringstream ss;
	ss << "key"<<i;

	int x[2000] =  {2,1};
	s.set(ss.str(),x,2000);
	keys.push_back(ss.str());

    }

    std::vector<std::pair<std::string,storage_type<int> > > r = s.multi_get(keys);

    FOR_EACH( it, r )
    {
	std::cout << "Key: " << it->first << " Val: " << it->second.data[0] << "\n";
    }


    //remove mapped file
    //bint::file_mapping::remove("MappedFile");
}
