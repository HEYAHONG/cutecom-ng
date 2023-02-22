#ifndef MODBUSSESSIONMANAGER_H
#define MODBUSSESSIONMANAGER_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <string>
#include <Modbus.h>
#include <QHash>
#include <QMap>
#include <functional>
#include <QTime>



class ModbusSessionManager : public QObject
{
    Q_OBJECT
public:
    explicit ModbusSessionManager(QObject *parent = nullptr);
    ~ModbusSessionManager();

    void SerialDataReceived(const QByteArray &data);

    typedef struct
    {
        uint16_t data;
        QTime time;
    } ModbusValue;

    typedef uint16_t ModbusAddress;

    /*
     *从session的缓存中读取数据
    */
    ModbusValue GetCoils(ModbusAddress addr);
    ModbusValue GetDiscreteInputs(ModbusAddress addr);
    ModbusValue GetHoldingRegisters(ModbusAddress addr);
    ModbusValue GetInputRegisters(ModbusAddress addr);
    QMap<ModbusAddress,ModbusValue> GetAllCoils();
    QMap<ModbusAddress,ModbusValue> GetAllDiscreteInputs();
    QMap<ModbusAddress,ModbusValue> GetAllHoldingRegisters();
    QMap<ModbusAddress,ModbusValue> GetAllInputRegisters();

    typedef enum
    {
        ReadCoils=0,
        ReadDiscreteInputs,
        ReadHoldingRegisters,
        ReadInputRegisters
    } ModbusReadOperation;
    typedef enum
    {
        WriteCoils=0x100,
        WriteHoldingRegisters
    } ModbusWriteOperation;

    void RequestModbusRead(uint8_t slaveaddr,ModbusSessionManager::ModbusReadOperation op,ModbusSessionManager::ModbusAddress addr,size_t length);
    void RequestModbusWrite(uint8_t slaveaddr,ModbusSessionManager::ModbusWriteOperation op,ModbusSessionManager::ModbusAddress addr,QVector<uint16_t> data);


signals:
    /*
     *数据变化
    */
    void DataChanged();

    /*
     *串口发送数据（一般用于内部处理）
    */
    void SendSerialData(const QByteArray data);

    /*
     * 读请求（一般用于内部处理）
    */
    void OnRequestModbusRead(uint8_t slaveaddr,ModbusSessionManager::ModbusReadOperation op,ModbusSessionManager::ModbusAddress addr,size_t length);
    /*
     * 写请求（一般用于内部处理）
    */
    void OnRequestModbusWrite(uint8_t slaveaddr,ModbusSessionManager::ModbusWriteOperation op,ModbusSessionManager::ModbusAddress addr,QVector<uint16_t> data);

    /*
     * 忙
    */
    void Busy();
    /*
     * 空闲
    */
    void Idle();

public slots:
    /*
     *关闭会话（不可直接使用delete删除ModbusSessionManager，必须通过connect转入ModbusSessionManager关闭会话）
    */
    void CloseSession();

private:
    QThread *session_thread;
    QMutex SessionLock;
    QByteArray modbus_reply;
    QByteArray modbus_reply_temp;
    void SetModbusReply(QByteArray data);
    std::string GetModbusReply();
    void ClearModbusReply();
    void SendModbusRequest(std::string frame);

    QHash<ModbusAddress,ModbusValue> CoilsTable;
    QHash<ModbusAddress,ModbusValue> DiscreteInputsTable;
    QHash<ModbusAddress,ModbusValue> HoldingRegistersTable;
    QHash<ModbusAddress,ModbusValue> InputRegistersTable;

    void SetCoils(ModbusAddress addr,uint16_t val);
    void SetDiscreteInputs(ModbusAddress addr,uint16_t val);
    void SetHoldingRegisters(ModbusAddress addr,uint16_t val);
    void SetInputRegisters(ModbusAddress addr,uint16_t val);


    modbus_master_context_t GetModbusContext(uint8_t slaveaddr);

private slots:
    /*
     * 读请求
    */
    void OnRequestModbusReadImp(uint8_t slaveaddr,ModbusSessionManager::ModbusReadOperation op,ModbusSessionManager::ModbusAddress addr,size_t length);
    /*
     * 写请求
    */
    void OnRequestModbusWriteImp(uint8_t slaveaddr,ModbusSessionManager::ModbusWriteOperation op,ModbusSessionManager::ModbusAddress addr,QVector<uint16_t> data);


};

#endif // MODBUSSESSIONMANAGER_H
