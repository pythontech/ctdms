#include <stdio.h>
#include "ctdms.h"

#define chk(f,args) do {                                \
        if ((err = f args) != 0) {                      \
            fprintf(stderr, "%s: error %d\n", #f, err); \
            return 1;                                   \
        }                                               \
    } while (0)

int main() {
    int err;
    TDMSFileHandle file;
    TDMSChannelGroupHandle group;
    TDMSChannelHandle chan0, chan1;
    float fvalues[] = {1, 2.5, 4, 6.75};

    chk(TDMS_CreateFile,("t1.tdms",
                         TDMS_Streaming1_0,
                         "test",
                         "desc",
                         "The Test",
                         "me",
                         &file));
    chk(TDMS_AddChannelGroup,(file,
                              "group",
                              "groupdesc",
                              &group));
    chk(TDMS_AddChannel,(group,
                         TDMS_SingleFloat,
                         "chan0",
                         "chan0 desc",
                         "V",
                         &chan0));
    chk(TDMS_AddChannel,(group,
                         TDMS_SingleFloat,
                         "chan1",
                         "chan1 desc",
                         "m/s",
                         &chan1));
    chk(TDMS_AppendDataValues,(chan0,
                               fvalues,
                               sizeof(fvalues) / sizeof(fvalues[0]),
                               0));
    float fvalues2[] = {3.1, 4.1, 5.9,
                        2.7, 1.8, 2.8};
    TDMSChannelHandle chans2[] = {chan0, chan1};
    chk(TDMS_AppendDataValuesMultiChannel,(chans2, 2,
                                           fvalues2, 3,
                                           TDMS_DataLayoutNonInterleaved,
                                           0));
    chk(TDMS_SaveFile,(file));
    chk(TDMS_CloseFile,(file));
    return 0;
}

    
