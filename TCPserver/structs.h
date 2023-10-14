#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>
#include <QTime>

//Идентификатор сообщения
#define ID 0xFFFE

//При передачи данных крайне желательно выравнивать структуры побайтово,
//чтобы избежать неправильной интерпритации данных!

#pragma pack(push, 1)

//Коды сообщений которые обрабатывает сервер
enum Messages{
    //Для удобства можно группировать, 100е это запросы, 200е установки
    GET_TIME = 100,
    GET_SIZE = 101,
    GET_STAT = 102,

    SET_DATA = 200,
    CLEAR_SPACE = 201
};

//Статусы сообщений
enum StatusMessages{

    STATUS_SUCCES = 1,

    ERR_NO_FREE_SPACE = 10, // Недостаточно свободного места
    ERR_ZERO_LEN = 11
};

/*!
 * \brief Стрктура описывает служебный заголовок пакета
 */
struct ServiceHeader{

    uint16_t id = ID;     //Идентификатор начала пакета
    uint16_t idData = 0;  //Идентификатор типа данных
    uint8_t status = 0;   //Статус сообщения, есть ли ошибка.
    uint32_t len = 0;     //Длина пакета далее, байт
};

/*!
 * \brief Структура хранит в себе статистику работы сервера.
 */
struct StatServer{

    StatServer(){
            memset(this, 0, sizeof(*this));
    }

    uint32_t incBytes;  //принято байт
    uint32_t sendBytes; //передано байт
    uint32_t revPck;    //принто пакетов
    uint32_t sendPck;   //передано пакетов
    uint32_t workTime;  //Время работы сервера секунд
    uint32_t clients;   //Количество подключенных клиентов
};


#pragma pack(pop)
#endif // STRUCTS_H
