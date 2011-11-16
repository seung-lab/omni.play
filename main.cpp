#include <iostream>
#include <cstddef>
#include <zi/utility/for_each.hpp>
#include <vector>
#include "storage_client.hpp"

int main()
{
  storage_client<int,int> s;

    
  int size = 10;
  int i;
  std::vector<int> keys;
  for(i = 0;i<size;i++)
    {
      int x[2000] =  {2,1};
      s.set(i,x,2000);
      keys.push_back(i);
      
    }
  
  std::vector<std::pair<int,storage_type<int> > > r = s.multi_get(keys);

  FOR_EACH( it, r )
    {
      std::cout << "Key: " << it->first << " Val: " << it->second.data[0] << "\n";
    }
  //remove mapped file
  //bint::file_mapping::remove("MappedFile");
}
