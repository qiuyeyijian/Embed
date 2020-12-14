#include "fm.h"


// 文件管理初始化
void fm_init() {
    flash_init();
}

// 添加文件数据
void fm_addFileData(uint16_t sect, uint8_t* buff) {
    flash_write(sect, 0, 1024, buff);
}

// 添加一条文件记录
void fm_addFileRecord(uint16_t offset, uint8_t* buff) {
    flash_write(recordSect, offset, 16, buff);
}

// 读取文件数据
void fm_readFileData(uint16_t sect, uint8_t* buff) {
    flash_read_logic(buff, sect, 0, 1024);
}