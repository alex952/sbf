#ifndef _SBF_CACHE_FILE_H_
#define _SBF_CACHE_FILE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef struct sbfCacheFileImpl* sbfCacheFile;
typedef struct sbfCacheFileItemImpl* sbfCacheFileItem;

typedef sbfError (*sbfCacheFileItemCb) (sbfCacheFile file,
                                        sbfCacheFileItem item,
                                        void* itemData,
                                        size_t itemSize,
                                        void* closure);

sbfCacheFile sbfCacheFile_open (const char* path,
                                size_t itemSize,
                                int always_create,
                                int* created,
                                sbfCacheFileItemCb cb,
                                void* closure);
void sbfCacheFile_close (sbfCacheFile file);

sbfCacheFileItem sbfCacheFile_add (sbfCacheFile file);
sbfError sbfCacheFile_flush (sbfCacheFileItem item, void* itemData);
sbfError sbfCacheFile_flushOffset (sbfCacheFileItem item,
                                   size_t offset,
                                   void* data,
                                   size_t size);

SBF_END_DECLS

#endif /* _SBF_CACHE_FILE_H_ */