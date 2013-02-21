#ifndef __RS485_H__
#define __RS485_H__


#define FILE_DEVICE_RS485     FILE_DEVICE_CONTROLLER

// IN:  PRS485_IO_DESC
#define IOCTL_RS485_SET_BUS_TYPE		CTL_CODE(FILE_DEVICE_RS485, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_RS485_SET_BUS_P2P 		CTL_CODE(FILE_DEVICE_RS485, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_RS485_GET_BUS_TYPE 		CTL_CODE(FILE_DEVICE_RS485, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)


#endif // __RS485_H__
