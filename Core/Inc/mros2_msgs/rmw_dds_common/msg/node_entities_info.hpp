#ifndef _RMW_DDS_COMMON_MSG_NODEENTITIESINFO_H
#define _RMW_DDS_COMMON_MSG_NODEENTITIESINFO_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "mros2_msgs/rmw_dds_common/msg/gid.hpp"

using namespace std;

namespace rmw_dds_common
{
namespace msg
  {
    class NodeEntitiesInfo
    {
    public:
      uint32_t cntPub = 0;
      uint32_t cntSub = 0;
      uint32_t idxSerialized = 0;

      typedef std::pair<bool, uint32_t> FragCopyReturnType;

      template <class T>
      uint32_t copyPrimToFragBufLocal(uint8_t *&addrPtr,
                                      const uint32_t cntPub,
                                      const uint32_t size,
                                      const T &data)
      {
        uint32_t lenPad = (0 == (cntPub % sizeof(T))) ? 0 : (sizeof(T) - (cntPub % sizeof(T))); // this doesn't get along with float128.
        if (size < sizeof(T))
        {
          // There are no enough space.
          return 0;
        }
        // Put padding space
        for (int i = 0; i < lenPad; i++)
        {
          *addrPtr = 0;
          addrPtr += 1;
        }
        // Store serialzed value.
        memcpy(addrPtr, &data, sizeof(T));
        addrPtr += sizeof(T);

        return sizeof(T) + lenPad;
      }

      template <class T>
      FragCopyReturnType copyArrayToFragBufLocal(uint8_t *&addrPtr,
                                                const uint32_t size,
                                                T &data,
                                                uint32_t &cntPubMemberLocal)
      {
        uint32_t pubDataSize = data.size();
        uint32_t cntLocalFrag = 0;

        if (cntPubMemberLocal < sizeof(uint32_t))
        {
          if (size < sizeof(uint32_t))
          {
            return {false, 0};
          }
          memcpy(addrPtr, &pubDataSize, sizeof(uint32_t));
          addrPtr += sizeof(uint32_t);
          cntPubMemberLocal += sizeof(uint32_t);
          cntLocalFrag += sizeof(uint32_t);
        }

        uint32_t cntFrag = (cntPubMemberLocal - sizeof(uint32_t)); // cntPubMemberLocal > 4 here
        uint32_t tmp = std::min(pubDataSize - cntFrag, size - cntLocalFrag);
        if (0 < tmp)
        {
          memcpy(addrPtr, data.data() + cntFrag, tmp);
          addrPtr += tmp;
          cntPubMemberLocal += tmp;
          cntLocalFrag += tmp;
        }

        return {(cntPubMemberLocal - sizeof(uint32_t)) >= pubDataSize, cntLocalFrag};
      }

      
      string node_namespace;
      
      string node_name;
      
      std::vector<Gid> reader_gid_seq;
      
      std::vector<Gid> writer_gid_seq;
      

      uint32_t copyToBuf(uint8_t *addrPtr)
      {
        uint32_t tmpPub = 0;
        uint32_t arraySize;
        uint32_t stringSize;
        
        if (cntPub % 4 >0)
        {
          for (uint32_t i = 0; i < (4 - (cntPub % 4)); i++)
          {
            *addrPtr = 0;
            addrPtr += 1;
          }
          cntPub += 4 - (cntPub % 4);
        }

        stringSize = node_namespace.size();
        memcpy(addrPtr, &stringSize, 4);
        addrPtr += 4;
        cntPub += 4;
        memcpy(addrPtr, node_namespace.c_str(), stringSize);
        addrPtr += stringSize;
        cntPub += stringSize;

        if (cntPub % 4 >0)
        {
          for (uint32_t i = 0; i < (4 - (cntPub % 4)); i++)
          {
            *addrPtr = 0;
            addrPtr += 1;
          }
          cntPub += 4 - (cntPub % 4);
        }
        stringSize = node_name.size();
        memcpy(addrPtr, &stringSize, 4);
        addrPtr += 4;
        cntPub += 4;
        memcpy(addrPtr, node_name.c_str(), stringSize);
        addrPtr += stringSize;
        cntPub += stringSize;

        if (cntPub % 4 > 0)
        {
          for (uint32_t i = 0; i < (4 - (cntPub % 4)); i++)
          {
            *addrPtr = 0;
            addrPtr += 1;
          }
          cntPub += 4 - (cntPub % 4);
        }
        arraySize = reader_gid_seq.size();
        memcpy(addrPtr, &arraySize, 4);
        addrPtr += 4;
        cntPub += 4;

        for (int i = 0; i < arraySize; i++)
        {
          tmpPub = reader_gid_seq[i].copyToBuf(addrPtr);
          cntPub += tmpPub;
          addrPtr += tmpPub;
        }

        if (cntPub % 4 > 0)
        {
          for (uint32_t i = 0; i < (4 - (cntPub % 4)); i++)
          {
            *addrPtr = 0;
            addrPtr += 1;
          }
          cntPub += 4 - (cntPub % 4);
        }
        arraySize = writer_gid_seq.size();
        memcpy(addrPtr, &arraySize, 4);
        addrPtr += 4;
        cntPub += 4;

        for (int i = 0; i < arraySize; i++)
        {
          tmpPub = writer_gid_seq[i].copyToBuf(addrPtr);
          cntPub += tmpPub;
          addrPtr += tmpPub;
        }
        return cntPub;
      }

      uint32_t copyFromBuf(const uint8_t *addrPtr)
      {
        uint32_t tmpSub = 0;
        uint32_t arraySize;
        uint32_t stringSize;

        if (cntSub % 4 >0)
        {
          for (uint32_t i = 0; i < (4 - (cntSub % 4)); i++)
          {
            addrPtr += 1;
          }
          cntSub += 4 - (cntSub % 4);
        }
        memcpy(&stringSize, addrPtr, 4);
        addrPtr += 4;
        cntSub += 4;
        node_namespace.resize(stringSize);
        memcpy(&node_namespace[0], addrPtr, stringSize);
        addrPtr += stringSize;
        cntSub += stringSize;

        if (cntSub % 4 >0)
        {
          for (uint32_t i = 0; i < (4 - (cntSub % 4)); i++)
          {
            addrPtr += 1;
          }
          cntSub += 4 - (cntSub % 4);
        }
        memcpy(&stringSize, addrPtr, 4);
        addrPtr += 4;
        cntSub += 4;
        node_name.resize(stringSize);
        memcpy(&node_name[0], addrPtr, stringSize);
        addrPtr += stringSize;
        cntSub += stringSize;

        if (cntSub % 4 >0)
        {
          for (uint32_t i = 0; i < (4 - (cntSub % 4)); i++)
          {
            addrPtr += 1;
          }
          cntSub += 4 - (cntSub % 4);
        }
        memcpy(&arraySize, addrPtr, 4);
        addrPtr += 4;
        cntSub += 4;

        for (int i = 0; i < arraySize; i++)
        {
          Gid buf;
          tmpSub = buf.copyFromBuf(addrPtr);
          reader_gid_seq.push_back(buf);
          addrPtr += tmpSub;
          cntSub += tmpSub;
        }    
        
        if (cntSub % 4 >0)
        {
          for (uint32_t i = 0; i < (4 - (cntSub % 4)); i++)
          {
            addrPtr += 1;
          }
          cntSub += 4 - (cntSub % 4);
        }
        memcpy(&arraySize, addrPtr, 4);
        addrPtr += 4;
        cntSub += 4;
        for (int i = 0; i < arraySize; i++)
        {
          Gid buf;
          tmpSub = buf.copyFromBuf(addrPtr);
          writer_gid_seq.push_back(buf);
          addrPtr += tmpSub;
          cntSub += tmpSub;
        }
        return cntSub;
      }

      void memAlign(uint8_t *addrPtr)
      {
        if (cntPub % 4 > 0)
        {
          addrPtr += cntPub;
          for (uint32_t i = 0; i < (4 - (cntPub % 4)); i++)
          {
            *addrPtr = 0;
            addrPtr += 1;
          }
          cntPub += 4 - (cntPub % 4);
        }
        return;
      }

      uint32_t getTotalSize()
      {
        uint32_t tmpCntPub = cntPub;
        cntPub = 0;
        return tmpCntPub;
      }

      uint32_t getPubCnt()
      {
        return cntPub;
      }

      uint32_t calcRawTotalSize()
      {
        // TODO: store template code here
        return 0;
      }

      uint32_t calcTotalSize()
      {
        uint32_t tmp;
        tmp = 4 + calcRawTotalSize();                  // CDR encoding version.
        tmp += (0 == (tmp % 4) ? 0 : (4 - (tmp % 4))); // Padding
        return tmp;
      }

      void resetCount()
      {
        cntPub = 0;
        cntSub = 0;
        idxSerialized = 0;
        // TODO: store template code here
        return;
      }

      FragCopyReturnType copyToFragBuf(uint8_t *addrPtr, uint32_t size)
      {
        // TODO: store template code here
        return {false, 0};
      }

      private:
        std::string type_name = "dds_::NodeEntitiesInfo";
    };
  };
}

namespace message_traits
{
  template<>
  struct TypeName<rmw_dds_common::msg::NodeEntitiesInfo*> {
    static const char* value()
    {
      return "rmw_dds_common::msg::dds_::NodeEntitiesInfo_";
    }
  };
};

#endif