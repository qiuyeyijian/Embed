==========================================================================
程序说明：
（1）程序摘要：DMA 普通模式触发
（2）运行芯片：本工程在KL36芯片运行
（3）软件环境：AHL-GEC-IDE，兼容KDS3.2
（4）硬件环境：嵌入式系统实验套件（AHL-MCU-KL36）
（5）硬件连接：见"..\05_UserBoard\user.h"文件
（6）功能简述：
			 串口发送10个字节数据，通过DMA放到目的内存地址内，让后通过DMA发送到串口
             因为是普通模式触发，所以只能发送和接收固定长度的字符串

============================================================================
版权所有：SD-Arm(sumcu.suda.edu.cn)
版本更新：20190906-20200318 
特别声明：
         ① 本代码在所提供硬件系统测试通过，真诚奉献给社会，不足之处，欢迎指正。
         ② 若使用其他硬件系统，移植代码时，请仔细根据自己的硬件匹配。
=============================================================================    







【20200326】WYH改，..\startup\.S文件，误中断，回原处
【20200430】V2.8 WYH改，..\startup\.c，不对看门狗操作，留给main操作
【20200503】LYH 修改。灌入程序后，不能运行。连接串口0至PC机后，可以运行，但是不能继续
通过IDE串口写入程序了。












