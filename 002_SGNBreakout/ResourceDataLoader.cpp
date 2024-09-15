#include "ResourceDataLoader.h"

#include <windows.h>


ResourceDataLoader::ResourceDataLoader(const std::string& resourceType, int resourceId)
   : ResourceType(resourceType), ResourceId(resourceId)
{
}


bool ResourceDataLoader::Load(std::vector<unsigned char>& out_data, size_t trailingSlack)
{
   HMODULE moduleHandle = GetModuleHandle(nullptr);

   HRSRC resourceInfo = FindResource(moduleHandle, MAKEINTRESOURCE(ResourceId), ResourceType.c_str());

   if (nullptr == resourceInfo)
      return false;

   size_t dataSize = static_cast<size_t>(SizeofResource(moduleHandle, resourceInfo));
   HGLOBAL resource = LoadResource(moduleHandle, resourceInfo);

   if (nullptr == resource)
      return false;

   const unsigned char* resourceData = static_cast<const unsigned char*>(LockResource(resource));
   out_data.resize(dataSize + trailingSlack);
   memcpy_s(out_data.data(), dataSize, resourceData, dataSize);

   if (0 != trailingSlack)
      memset(out_data.data() + dataSize, 0, trailingSlack);

   return true;
}
