#ifndef _YEDIS_FORMATER_H_
#define _YEDIS_FORMATER_H_
#include <string>
#include <vector>
#include "../base/ReplyBuffer.h"

namespace Yedis
{
class YedisFormate
{
public:
   static  std::size_t formatInt(long value, ReplyBuffer* reply);
   static  std::size_t formatSingle(const char* str, std::size_t len, ReplyBuffer* reply);
   static  std::size_t formatSingle(const std::string& str, ReplyBuffer* reply);
   static  std::size_t formatBulk(const char* str, std::size_t len, ReplyBuffer* reply);
   static  std::size_t formatBulk(const std::string& str, ReplyBuffer* reply);
   static  std::size_t preFormatMultiBulk(std::size_t nBulk, ReplyBuffer* reply);
   static  std::size_t formatMultiBulk(const std::vector<std::string> vs, ReplyBuffer* reply);

   static  std::size_t formatEmptyBulk(ReplyBuffer* reply);
   static  std::size_t formatNull(ReplyBuffer* reply);
   static  std::size_t formatNullArray(ReplyBuffer* reply);
   static  std::size_t formatOK(ReplyBuffer* reply);
   static  std::size_t format1(ReplyBuffer* reply);
   static  std::size_t format0(ReplyBuffer* reply);
};
}
#endif //!_YEDIS_FORMATER_H_
