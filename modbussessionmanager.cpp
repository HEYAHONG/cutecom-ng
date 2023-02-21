#include <QVector>
#include "modbussessionmanager.h"
#include <QThread>
#include <QApplication>

ModbusSessionManager::ModbusSessionManager(QObject *parent)
    : QObject{parent},
      session_thread(new QThread(this))
{
    //注册uint8_t
    qRegisterMetaType<uint8_t>("uint8_t");
    //注册ModbusSessionManager::ModbusReadOperation
    qRegisterMetaType<ModbusSessionManager::ModbusReadOperation>("ModbusSessionManager::ModbusReadOperation");
    //注册ModbusSessionManager::ModbusWriteOperation
    qRegisterMetaType<ModbusSessionManager::ModbusWriteOperation>("ModbusSessionManager::ModbusWriteOperation");
    //注册ModbusSessionManager::ModbusAddress
    qRegisterMetaType<ModbusSessionManager::ModbusAddress>("ModbusSessionManager::ModbusAddress");
    //注册QVector<uint16_t>
    qRegisterMetaType<QVector<uint16_t>>("QVector<uint16_t>");
    //注册size_t
    qRegisterMetaType<size_t>("size_t");


    moveToThread(session_thread);
    session_thread->start();
    connect(this,&ModbusSessionManager::OnRequestModbusRead,this,&ModbusSessionManager::OnRequestModbusReadImp);
    connect(this,&ModbusSessionManager::OnRequestModbusWrite,this,&ModbusSessionManager::OnRequestModbusWriteImp);
}

void ModbusSessionManager::SerialDataReceived(const QByteArray &data)
{
    SetModbusReply(data);
}

ModbusSessionManager::ModbusValue ModbusSessionManager::GetCoils(ModbusAddress addr)
{
    QMutexLocker lock(&SessionLock);
    ModbusValue ret;
    if(CoilsTable.contains(addr))
    {
        ret=CoilsTable[addr];
    }
    return ret;
}
ModbusSessionManager::ModbusValue ModbusSessionManager::GetDiscreteInputs(ModbusAddress addr)
{
    QMutexLocker lock(&SessionLock);
    ModbusValue ret;
    if(DiscreteInputsTable.contains(addr))
    {
        ret=DiscreteInputsTable[addr];
    }
    return ret;
}
ModbusSessionManager::ModbusValue ModbusSessionManager::GetHoldingRegisters(ModbusAddress addr)
{
    QMutexLocker lock(&SessionLock);
    ModbusValue ret;
    if(HoldingRegistersTable.contains(addr))
    {
        ret=HoldingRegistersTable[addr];
    }
    return ret;
}
ModbusSessionManager::ModbusValue ModbusSessionManager::GetInputRegisters(ModbusAddress addr)
{
    QMutexLocker lock(&SessionLock);
    ModbusValue ret;
    if(InputRegistersTable.contains(addr))
    {
        ret=InputRegistersTable[addr];
    }
    return ret;
}

QMap<ModbusSessionManager::ModbusAddress,ModbusSessionManager::ModbusValue> ModbusSessionManager::GetAllCoils()
{
    QMutexLocker lock(&SessionLock);
    QMap<ModbusSessionManager::ModbusAddress,ModbusSessionManager::ModbusValue> ret;
    {
        for(auto it=CoilsTable.begin();it!=CoilsTable.end();it++)
        {
            ret[it.key()]=it.value();
        }
    }
    return ret;
}
QMap<ModbusSessionManager::ModbusAddress,ModbusSessionManager::ModbusValue> ModbusSessionManager::GetAllDiscreteInputs()
{
    QMutexLocker lock(&SessionLock);
    QMap<ModbusSessionManager::ModbusAddress,ModbusSessionManager::ModbusValue> ret;
    {
        for(auto it=DiscreteInputsTable.begin();it!=DiscreteInputsTable.end();it++)
        {
            ret[it.key()]=it.value();
        }
    }
    return ret;
}
QMap<ModbusSessionManager::ModbusAddress,ModbusSessionManager::ModbusValue> ModbusSessionManager::GetAllHoldingRegisters()
{
    QMutexLocker lock(&SessionLock);
    QMap<ModbusSessionManager::ModbusAddress,ModbusSessionManager::ModbusValue> ret;
    {
        for(auto it=HoldingRegistersTable.begin();it!=HoldingRegistersTable.end();it++)
        {
            ret[it.key()]=it.value();
        }
    }
    return ret;
}
QMap<ModbusSessionManager::ModbusAddress,ModbusSessionManager::ModbusValue> ModbusSessionManager::GetAllInputRegisters()
{
    QMutexLocker lock(&SessionLock);
    QMap<ModbusSessionManager::ModbusAddress,ModbusSessionManager::ModbusValue> ret;
    {
        for(auto it=InputRegistersTable.begin();it!=InputRegistersTable.end();it++)
        {
            ret[it.key()]=it.value();
        }
    }
    return ret;
}

void ModbusSessionManager::RequestModbusRead(uint8_t slaveaddr,ModbusSessionManager::ModbusReadOperation op,ModbusSessionManager::ModbusAddress addr,size_t length)
{
    emit OnRequestModbusRead(slaveaddr,op,addr,length);
}
void ModbusSessionManager::RequestModbusWrite(uint8_t slaveaddr,ModbusSessionManager::ModbusWriteOperation op,ModbusSessionManager::ModbusAddress addr,QVector<uint16_t> data)
{
    emit OnRequestModbusWrite(slaveaddr,op,addr,data);
}


void ModbusSessionManager::SetModbusReply(QByteArray data)
{
    QMutexLocker lock(&SessionLock);
    modbus_reply_temp+=data;
    std::string c_data=modbus_reply_temp.toStdString();
    if(Modbus_Payload_Check_CRC((uint8_t *)c_data.c_str(),c_data.length()))
    {
        modbus_reply=modbus_reply_temp;
    }

    if(modbus_reply_temp.length()>MODBUS_RTU_MAX_ADU_LENGTH)
    {
        modbus_reply_temp.clear();
    }
}

std::string ModbusSessionManager::GetModbusReply()
{
    QMutexLocker lock(&SessionLock);
    return modbus_reply.toStdString();
}

void ModbusSessionManager::ClearModbusReply()
{
   QMutexLocker lock(&SessionLock);
   modbus_reply.clear();
   modbus_reply_temp.clear();
}

void ModbusSessionManager::SendModbusRequest(std::string frame)
{
    emit SendSerialData(QByteArray::fromStdString(frame));
}

void ModbusSessionManager::CloseSession()
{

    moveToThread(QApplication::instance()->thread());
    deleteLater();
    /*
     *退出当前线程
    */
    QThread::currentThread()->exit(0);
}

void ModbusSessionManager::SetCoils(ModbusAddress addr,uint16_t val)
{
    ModbusValue data={val,QTime::currentTime()};
    QMutexLocker lock(&SessionLock);
    CoilsTable[addr]=data;
}
void ModbusSessionManager::SetDiscreteInputs(ModbusAddress addr,uint16_t val)
{
    ModbusValue data={val,QTime::currentTime()};
    QMutexLocker lock(&SessionLock);
    DiscreteInputsTable[addr]=data;
}
void ModbusSessionManager::SetHoldingRegisters(ModbusAddress addr,uint16_t val)
{
    ModbusValue data={val,QTime::currentTime()};
    QMutexLocker lock(&SessionLock);
    HoldingRegistersTable[addr]=data;
}
void ModbusSessionManager::SetInputRegisters(ModbusAddress addr,uint16_t val)
{
    ModbusValue data={val,QTime::currentTime()};
    QMutexLocker lock(&SessionLock);
    InputRegistersTable[addr]=data;
}


static thread_local ModbusSessionManager *current_session=NULL;
modbus_master_context_t ModbusSessionManager::GetModbusContext(uint8_t slaveaddr)
{
    modbus_master_context_t ctx={0,NULL,NULL};

    ctx.slave_addr=slaveaddr;

    current_session=this;

    ctx.output=[](uint8_t *data,size_t length)
    {
        current_session->ClearModbusReply();
        current_session->SendModbusRequest(std::string((char *)data,length));
    };

    ctx.request_reply=[](uint8_t *data,size_t length) -> size_t
    {
        size_t count=0;
        while(count < 3000)
        {
            QThread::msleep(1);
            count++;
            std::string datareply=current_session->GetModbusReply();
            if(!datareply.empty())
            {
                current_session->ClearModbusReply();
                size_t copy_size=length>datareply.length()?datareply.length():length;
                memcpy(data,datareply.c_str(),copy_size);
                return copy_size;
            }
        }

        return 0;
    };

    return ctx;
}

void ModbusSessionManager::OnRequestModbusReadImp(uint8_t slaveaddr,ModbusSessionManager::ModbusReadOperation op,ModbusSessionManager::ModbusAddress addr,size_t length)
{
    if((addr+length) > 0xFFFF)
    {
        //地址越界
        return;
    }
    bool datachanged=false;
    modbus_master_context_t ctx=GetModbusContext(slaveaddr);
    switch(op)
    {
    case ReadCoils:
    {
        uint8_t buff[MODBUS_RTU_MAX_ADU_LENGTH]={0};
        bool    val[MODBUS_MAX_READ_BITS]={0};
        ModbusAddress startaddr=addr;
        while(length > MODBUS_MAX_READ_BITS)
        {
            if(Modbus_Master_Read_OX(&ctx,startaddr,val,MODBUS_MAX_READ_BITS,buff,sizeof(buff)))
            {
                datachanged=true;
                for(size_t i= 0;i < MODBUS_MAX_READ_BITS ; i++)
                {
                    SetCoils(startaddr+i,val[i]);
                }
            }
            startaddr+=MODBUS_MAX_READ_BITS;
            length-=MODBUS_MAX_READ_BITS;
        }

        if(length>0)
        {
            if(Modbus_Master_Read_OX(&ctx,startaddr,val,length,buff,sizeof(buff)))
            {
                datachanged=true;
                for(size_t i= 0;i < length ; i++)
                {
                    SetCoils(startaddr+i,val[i]);
                }
            }
        }

    }
    break;
    case ReadDiscreteInputs:
    {
        uint8_t buff[MODBUS_RTU_MAX_ADU_LENGTH]={0};
        bool    val[MODBUS_MAX_READ_BITS]={0};
        ModbusAddress startaddr=addr;
        while(length > MODBUS_MAX_READ_BITS)
        {
            if(Modbus_Master_Read_IX(&ctx,startaddr,val,MODBUS_MAX_READ_BITS,buff,sizeof(buff)))
            {
                datachanged=true;
                for(size_t i= 0;i < MODBUS_MAX_READ_BITS ; i++)
                {
                    SetDiscreteInputs(startaddr+i,val[i]);
                }
            }
            startaddr+=MODBUS_MAX_READ_BITS;
            length-=MODBUS_MAX_READ_BITS;
        }

        if(length>0)
        {
            if(Modbus_Master_Read_IX(&ctx,startaddr,val,length,buff,sizeof(buff)))
            {
                datachanged=true;
                for(size_t i= 0;i < length ; i++)
                {
                    SetDiscreteInputs(startaddr+i,val[i]);
                }
            }
        }
    }
    break;
    case ReadHoldingRegisters:
    {
        uint8_t buff[MODBUS_RTU_MAX_ADU_LENGTH]={0};
        uint16_t    val[MODBUS_MAX_READ_REGISTERS]={0};
        ModbusAddress startaddr=addr;
        while(length > MODBUS_MAX_READ_REGISTERS)
        {
            if(Modbus_Master_Read_Hold_Register(&ctx,startaddr,val,MODBUS_MAX_READ_REGISTERS,buff,sizeof(buff)))
            {
                datachanged=true;
                for(size_t i= 0;i < MODBUS_MAX_READ_REGISTERS ; i++)
                {
                    SetHoldingRegisters(startaddr+i,val[i]);
                }
            }
            startaddr+=MODBUS_MAX_READ_REGISTERS;
            length-=MODBUS_MAX_READ_REGISTERS;
        }

        if(length>0)
        {
            if(Modbus_Master_Read_Hold_Register(&ctx,startaddr,val,length,buff,sizeof(buff)))
            {
                datachanged=true;
                for(size_t i= 0;i < length ; i++)
                {
                    SetHoldingRegisters(startaddr+i,val[i]);
                }
            }
        }
    }
    break;
    case ReadInputRegisters:
    {
        uint8_t buff[MODBUS_RTU_MAX_ADU_LENGTH]={0};
        uint16_t    val[MODBUS_MAX_READ_REGISTERS]={0};
        ModbusAddress startaddr=addr;
        while(length > MODBUS_MAX_READ_REGISTERS)
        {
            if(Modbus_Master_Read_Input_Register(&ctx,startaddr,val,MODBUS_MAX_READ_REGISTERS,buff,sizeof(buff)))
            {
                datachanged=true;
                for(size_t i= 0;i < MODBUS_MAX_READ_REGISTERS ; i++)
                {
                    SetInputRegisters(startaddr+i,val[i]);
                }
            }
            startaddr+=MODBUS_MAX_READ_REGISTERS;
            length-=MODBUS_MAX_READ_REGISTERS;
        }

        if(length>0)
        {
            if(Modbus_Master_Read_Input_Register(&ctx,startaddr,val,length,buff,sizeof(buff)))
            {
                datachanged=true;
                for(size_t i= 0;i < length ; i++)
                {
                    SetInputRegisters(startaddr+i,val[i]);
                }
            }
        }
    }
    break;
    }

    if(datachanged)
    {
        emit DataChanged();
    }
}

void ModbusSessionManager::OnRequestModbusWriteImp(uint8_t slaveaddr,ModbusSessionManager::ModbusWriteOperation op,ModbusSessionManager::ModbusAddress addr,QVector<uint16_t> data)
{
    size_t length=data.length();
    if((addr+length) > 0xFFFF)
    {
        //地址越界
        return;
    }
    modbus_master_context_t ctx=GetModbusContext(slaveaddr);
    switch(op)
    {
    case WriteCoils:
    {
        uint8_t buff[MODBUS_RTU_MAX_ADU_LENGTH]={0};
        bool    val[MODBUS_MAX_WRITE_BITS]={0};
        ModbusAddress startaddr=addr;
        while(length>MODBUS_MAX_WRITE_BITS)
        {
            for(size_t i = 0; i < MODBUS_MAX_WRITE_BITS;i++)
            {
                if(data[startaddr+i-addr])
                {
                    val[i]=true;
                }
                else
                {
                    val[i]=false;
                }
            }
            Modbus_Master_Write_OX(&ctx,startaddr,val,MODBUS_MAX_WRITE_BITS,buff,sizeof(buff));
            startaddr+=MODBUS_MAX_WRITE_BITS;
            length-=MODBUS_MAX_WRITE_BITS;
        }

        if(length>0)
        {
            for(size_t i = 0; i < length;i++)
            {
                if(data[startaddr+i-addr])
                {
                    val[i]=true;
                }
                else
                {
                    val[i]=false;
                }
            }
            Modbus_Master_Write_OX(&ctx,startaddr,val,length,buff,sizeof(buff));
        }
    }
    break;
    case WriteHoldingRegisters:
    {
        uint8_t buff[MODBUS_RTU_MAX_ADU_LENGTH]={0};
        uint16_t    val[MODBUS_MAX_WRITE_REGISTERS]={0};
        ModbusAddress startaddr=addr;
        while(length>MODBUS_MAX_WRITE_REGISTERS)
        {
            for(size_t i = 0; i < MODBUS_MAX_WRITE_REGISTERS;i++)
            {
                val[i]=data[startaddr+i-addr];
            }
            Modbus_Master_Write_Hold_Register(&ctx,startaddr,val,MODBUS_MAX_WRITE_REGISTERS,buff,sizeof(buff));
            startaddr+=MODBUS_MAX_WRITE_REGISTERS;
            length-=MODBUS_MAX_WRITE_REGISTERS;
        }

        if(length>0)
        {
            for(size_t i = 0; i < length;i++)
            {
                val[i]=data[startaddr+i-addr];
            }
            Modbus_Master_Write_Hold_Register(&ctx,startaddr,val,length,buff,sizeof(buff));
        }
    }
    break;
    }
}


ModbusSessionManager::~ModbusSessionManager()
{

}
