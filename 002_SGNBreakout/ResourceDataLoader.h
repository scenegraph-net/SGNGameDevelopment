#pragma once

#include "DataLoader.h"

#include <string>


class ResourceDataLoader : public DataLoader
{
   public:
      ResourceDataLoader(const std::string& resourceType, int resourceId);

      bool Load(std::vector<unsigned char>& out_data, size_t trailingSlack = 0) override;

   private:
      const std::string ResourceType;
      const int ResourceId;
};
