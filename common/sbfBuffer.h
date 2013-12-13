#ifndef _SBF_BUFFER_H_
#define _SBF_BUFFER_H_

#include "sbfCommon.h"
#include "sbfPool.h"
#include "sbfRefCount.h"

SBF_BEGIN_DECLS

typedef struct sbfBufferImpl* sbfBuffer;

typedef void (*sbfBufferDestroyCb) (sbfBuffer buffer, void* closure);

sbfPool sbfBuffer_createPool (size_t size);

static SBF_INLINE sbfBuffer sbfBuffer_new (sbfPool pool, size_t size);
static SBF_INLINE sbfBuffer sbfBuffer_newZero (sbfPool pool, size_t size);

static SBF_INLINE sbfBuffer sbfBuffer_wrap (void* data,
                                            size_t size,
                                            sbfBufferDestroyCb cb,
                                            void* closure);

static SBF_INLINE void sbfBuffer_addRef (sbfBuffer buffer);

static SBF_INLINE void sbfBuffer_destroy (sbfBuffer buffer);

static SBF_INLINE void* sbfBuffer_getData (sbfBuffer buffer);
static SBF_INLINE void sbfBuffer_setData (sbfBuffer buffer, void* data);

static SBF_INLINE size_t sbfBuffer_getSize (sbfBuffer buffer);
static SBF_INLINE void sbfBuffer_setSize (sbfBuffer buffer, size_t size);

#include "sbfBufferInline.h"

SBF_END_DECLS

#endif /* _SBF_BUFFER_H_ */
