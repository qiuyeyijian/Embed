// fm是文件管理file manager的简称

#include "mcu.h"
#include "printf.h"
#include "gec.h"
#include "flash.h"

#define recordSect (63)

// 文件管理初始化
void fm_init();

// 添加文件数据
void fm_addFileData(uint16_t sect, uint8_t* buff);

// 添加一条文件记录
void fm_addFileRecord(uint16_t offset, uint8_t* buff);

// 读取文件数据
void fm_readFileData(uint16_t sect, uint8_t* buff);


