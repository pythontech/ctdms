/*=======================================================================
 *      TDMS
 *	https://www.ni.com/docs/en-US/bundle/labwindows-cvi/page/cvi/libref/cvitdmslibraryfunctiontree.htm
 *	https://www.ni.com/content/dam/web/product-documentation/c_dll_tdm.zip
 *=======================================================================*/
#ifndef TDMS_H
#define TDMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

enum TDMSFileFormat {
    TDMS_Streaming1_0
};

enum TDMSByteOrder {
    TDMS_ByteOrderLittleEndian,
    TDMS_ByteOrderBigEndian
};

enum TDMSDataType {
    TDMS_Int32 = 4,
    TDMS_Uint64 = 8,
    TDMS_SingleFloat = 9,
    TDMS_DoubleFloat = 10
};

enum TDMSDataLayout {
    TDMS_DataLayoutNonInterleaved,
    TDMS_DataLayoutInterleaved
};

struct _TDMSFile;
struct _TDMSChannelGroup;
struct _TDMSChannel;

typedef struct _TDMSFile         *TDMSFileHandle;
typedef struct _TDMSChannelGroup *TDMSChannelGroupHandle;
typedef struct _TDMSChannel      *TDMSChannelHandle;

int // status: 0=OK, negative=error
TDMS_CreateFile(const char *filePath,
                TDMSFileFormat fileFormat,
                const char *name,
                const char *description,
                const char *title,
                const char *author,
                TDMSFileHandle *file);
int // status: 0=OK, negative=error
TDMS_CreateFileEx(const char *filePath,
                  TDMSFileFormat fileFormat,
                  TDMSByteOrder byteOrder,
                  unsigned int options,
                  const char *name,
                  const char *description,
                  const char *title,
                  const char *author,
                  TDMSFileHandle *file);
int // status: 0=OK, negative=error
TDMS_AddChannelGroup(TDMSFileHandle file,
                     const char *name,
                     const char *description,
                     TDMSChannelGroupHandle *channelGroup);
int // status: 0=OK, negative=error
TDMS_AddChannel(TDMSChannelGroupHandle channelGroup,
                TDMSDataType dataType,
                const char *name,
                const char *description,
                const char *unitString,
                TDMSChannelHandle *channel);
int // status: 0=OK, negative=error
TDMS_SaveFile(TDMSFileHandle file);
int // status: 0=OK, negative=error
TDMS_CloseFile(TDMSFileHandle file);
int // status: 0=OK, negative=error
TDMS_OpenFile(const char *filePath,
              int readOnly,
              TDMSFileHandle *file);
// TDMS_OpenFileEx(...)
int // status: 0=OK, negative=error
TDMS_AppendDataValues(TDMSChannelHandle channel,
                      void *values,
                      size_t numberOfValues,
                      int saveFile);
int // status: 0=OK, negative=error
TDMS_AppendDataValuesMultiChannel(TDMSChannelHandle channels[],
                                  size_t numberOfChannels,
                                  void *values,
                                  size_t numberOfValues,
                                  TDMSDataLayout dataLayout,
                                  int saveFile);

#ifdef __cplusplus
}
#endif

#endif /* TDMS_H */
