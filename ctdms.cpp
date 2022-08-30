#include "ctdms.h"
#include <string>
#include <vector>
#include <cstring>
#include <fstream>

#define OK 0

static inline size_t
lstrSize(const std::string &str) {
    return 4 + str.size();
}

struct TDMSChunk {
    _TDMSChannel *channel;
    size_t numValues;
    char *dataCopy;
    size_t numBytes;

    TDMSChunk(_TDMSChannel *_channel, size_t _numValues, char *_data, size_t numBytes);
    size_t indexSize();
    void writeIndex(_TDMSFile &);
};

struct _TDMSFile {
    std::string path;
    std::string name;
    std::string description;
    std::string title;
    std::string author;
    std::vector<TDMSChannelGroupHandle> groups;
    std::vector<TDMSChunk> pending;
    std::ofstream f;

    _TDMSFile(const char *filePath,
              TDMSFileFormat fileFormat,
              TDMSByteOrder byteOrder,
              unsigned int options,
              const char *_name,
              const char *_description,
              const char *_title,
              const char *_author)
        : path(filePath), name(_name), description(_description),
          title(_title), author(_author) {
    }
    void addGroup(_TDMSChannelGroup *group);
    void save();
    void close();
    _TDMSFile& bytes(const char *, size_t);
    _TDMSFile& i4(const int32_t val) {
        return bytes(reinterpret_cast<const char *>(&val), sizeof(val));
    }
    _TDMSFile& u4(const uint32_t val) {
        return bytes(reinterpret_cast<const char *>(&val), sizeof(val));
    }
    _TDMSFile& u8(const uint64_t val) {
        return bytes(reinterpret_cast<const char *>(&val), sizeof(val));
    }
    _TDMSFile& f8(double val) {
        return bytes(reinterpret_cast<const char *>(&val), sizeof(val));
    }
    _TDMSFile& lstr(std::string val) {
        return u4(val.size()) . bytes(val.c_str(), val.size());
    }
};

struct _TDMSChannelGroup {
    _TDMSFile *file;
    std::string name;
    std::string description;
    std::vector<TDMSChannelHandle> channels;

    _TDMSChannelGroup(const char *_name,
                      const char *_description)
        : name(_name), description(_description) {
    }

    void addChannel(_TDMSChannel *channel);
};

struct _TDMSChannel {
    _TDMSChannelGroup *group;
    TDMSDataType dataType;
    std::string name;
    std::string description;
    std::string unitString;

    _TDMSChannel(TDMSDataType _dataType,
                 const char *_name,
                 const char *_description,
                 const char *_unitString)
        : dataType(_dataType), name(_name), description(_description), unitString(_unitString) {
    }
    std::string fullPath() {
        return "'" + group->name + "'/'" + name + "'";
    }
};

//-----------------------------------------------------------------------
//      Method implementations
//-----------------------------------------------------------------------
void _TDMSFile::addGroup(_TDMSChannelGroup *group) {
    group->file = this;
    groups.push_back(group);
}

_TDMSFile &
_TDMSFile::bytes(const char *data, size_t size) {
    f.write(data, size);
    return *this;
}

void _TDMSFile::save() {
    if (! f.is_open()) {
        f.open(path);
    }
    bytes("TDSm", 4);
    u4(6);                              // toc_flags
    u4(4712);                           // version
    size_t indexTotal = 0;
    size_t dataTotal = 0;
    for (auto& chunk : pending) {
        indexTotal += chunk.indexSize();
        dataTotal += chunk.numBytes;
    }
    u8(indexTotal + dataTotal);         // next_segment_offset
    u8(indexTotal);                     // raw_data_offset
    // Index
    u4(pending.size());
    for (auto& chunk : pending) {
        chunk.writeIndex(*this);
    }
    for (auto& chunk : pending) {
        bytes(&chunk.dataCopy[0], chunk.numBytes);
    }
}

void _TDMSFile::close() {
    f.close();
}

void _TDMSChannelGroup::addChannel(_TDMSChannel *channel) {
    channel->group = this;
    channels.push_back(channel);
}

TDMSChunk::TDMSChunk(_TDMSChannel *_channel, size_t _numValues, char *_data, size_t _numBytes)
    : channel(_channel), numValues(_numValues), dataCopy(_data), numBytes(_numBytes)
{
}

size_t
TDMSChunk::indexSize() {
    return lstrSize(channel->fullPath())
        + 4                         // 20
        + 4                         // dtype
        + 4                         // dimension
        + 8                         // nvalues
        + 4;                        // nprops
}

void
TDMSChunk::writeIndex(_TDMSFile& file) {
    file
        .lstr(channel->fullPath())
        .u4(20)
        .u4(channel->dataType)
        .u4(1)                          // dimension
        .u8(numValues)
        .u4(0);                          // nprops
}

//-----------------------------------------------------------------------
//      C API
//-----------------------------------------------------------------------
int
TDMS_CreateFile(const char *filePath,
                TDMSFileFormat fileFormat,
                const char *name,
                const char *description,
                const char *title,
                const char *author,
                TDMSFileHandle *p_file)
{
    return TDMS_CreateFileEx(filePath,
                             fileFormat,
                             TDMS_ByteOrderLittleEndian,
                             0,         // options
                             name,
                             description,
                             title,
                             author,
                             p_file);
}

int // status: 0=OK, negative=error
TDMS_CreateFileEx(const char *filePath,
                  TDMSFileFormat fileFormat,
                  TDMSByteOrder byteOrder,
                  unsigned int options,
                  const char *name,
                  const char *description,
                  const char *title,
                  const char *author,
                  TDMSFileHandle *p_file)
{
    _TDMSFile *file = new _TDMSFile(filePath,
                                    fileFormat, byteOrder, options,
                                    name,
                                    description, title, author);
    *p_file = file;
    return OK;
}

int // status: 0=OK, negative=error
TDMS_AddChannelGroup(TDMSFileHandle file,
                     const char *name,
                     const char *description,
                     TDMSChannelGroupHandle *p_channelGroup)
{
    _TDMSChannelGroup *group = new _TDMSChannelGroup(name, description);
    file->addGroup(group);
    *p_channelGroup = group;
    return OK;
}

int // status: 0=OK, negative=error
TDMS_AddChannel(TDMSChannelGroupHandle group,
                TDMSDataType dataType,
                const char *name,
                const char *description,
                const char *unitString,
                TDMSChannelHandle *p_channel)
{
    TDMSChannelHandle channel = new _TDMSChannel(dataType, name, description, unitString);
    group->addChannel(channel);
    *p_channel = channel;
    return OK;
}

int // status: 0=OK, negative=error
TDMS_SaveFile(TDMSFileHandle file) {
    try {
        file->save();
    }
    catch (int e) {
        return e;
    }
    return OK;
}

int // status: 0=OK, negative=error
TDMS_CloseFile(TDMSFileHandle file) {
    file->close();
    return OK;
}

int // status: 0=OK, negative=error
TDMS_OpenFile(const char *filePath,
              int readOnly,
              TDMSFileHandle *file);
// TDMS_OpenFileEx(...)
int // status: 0=OK, negative=error
TDMS_AppendDataValues(TDMSChannelHandle channel,
                      void *values,
                      size_t numberOfValues,
                      int saveFile) {
    size_t numBytes = 0;
    switch (channel->dataType) {
    case TDMS_SingleFloat:
        numBytes = numberOfValues * sizeof(float);
        break;
    default:
        throw "Unhandled type";
    }
    char *dataCopy = new char[numBytes];
    memcpy(dataCopy, values, numBytes);
    channel->group->file->pending.emplace_back(channel, numberOfValues,
                                               dataCopy, numBytes);
    return OK;
}

int // status: 0=OK, negative=error
TDMS_AppendDataValuesMultiChannel(TDMSChannelHandle channels[],
                                  size_t numberOfChannels,
                                  void *values,
                                  size_t numberOfValues,
                                  TDMSDataLayout dataLayout,
                                  int saveFile);

