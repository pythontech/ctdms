#include "ctdms.h"

int main() {
    TDMSFileHandle file;
    TDMSChannelGroupHandle group;
    TDMSChannelHandle chan0, chan1;
    float fvalues[] = {1, 2.5, 4, 6.75};

    TDMS_CreateFile("t1.tdms",
                    TDMS_Streaming1_0,
                    "test",
                    "desc",
                    "The Test",
                    "me",
                    &file);
    TDMS_AddChannelGroup(file,
                         "group",
                         "groupdesc",
                         &group);
    TDMS_AddChannel(group,
                    TDMS_SingleFloat,
                    "chan0",
                    "chan0 desc",
                    "V",
                    &chan0);
    TDMS_AddChannel(group,
                    TDMS_SingleFloat,
                    "chan1",
                    "chan1 desc",
                    "m/s",
                    &chan1);
    TDMS_AppendDataValues(chan0,
                          fvalues,
                          sizeof(fvalues) / sizeof(fvalues[0]),
                          0);
    TDMS_SaveFile(file);
    TDMS_CloseFile(file);
    return 0;
}

    
