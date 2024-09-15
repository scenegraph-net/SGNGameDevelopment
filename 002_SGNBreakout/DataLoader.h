#pragma once

#include <vector>


class DataLoader
{
   public:
      virtual bool Load(std::vector<unsigned char>& out_data, size_t trailingSlack = 0) = 0;
};
