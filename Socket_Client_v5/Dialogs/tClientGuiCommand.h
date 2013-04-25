#ifndef TCLIENTGUICOMMAND_H
#define TCLIENTGUICOMMAND_H
#include "tAbstractGuiCommand.h"
#include <QStringListModel>



class tGuiReceiveFile : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiReceiveFile()
{
    return new tGuiReceiveFile;
}
//**********************************************************************************
class tGuiSendFile : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiSendFile()
{
    return new tGuiSendFile;
}
//**********************************************************************************
class tGuiReportReceiveFile : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiReportReceiveFile()
{
    return new tGuiReportReceiveFile;
}
//**********************************************************************************
class tGuiReportSendFile : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiReportSendFile()
{
    return new tGuiReportSendFile;
}
//**********************************************************************************
class tGuiError : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiError()
{
    return new tGuiError;
}
//**********************************************************************************
class tGuiConnectConfirmReport : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);
};
inline tAbstractGuiCommand* Create_tGuiConnectConfirmReport()
{
    return new tGuiConnectConfirmReport;
}
//**********************************************************************************
class tGuiPrepareSendFile : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiPrepareSendFile()
{
    return new tGuiPrepareSendFile;
}
//**********************************************************************************
class tGuiReportPrepareSendFile : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);
};
inline tAbstractGuiCommand* Create_tGuiReportPrepareSendFile()
{
    return new tGuiReportPrepareSendFile;
}
//**********************************************************************************
class tGuiSendAutorization : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);
};
inline tAbstractGuiCommand* Create_tGuiSendAutorization()
{
    return new tGuiSendAutorization;
}
//**********************************************************************************
class tGuiReportAutorization : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);
};
inline tAbstractGuiCommand* Create_tGuiReportAutorization()
{
    return new tGuiReportAutorization;
}
//**********************************************************************************
class tGuiStartTransaction : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiStartTransaction()
{
    return new tGuiStartTransaction;
}
//**********************************************************************************
class tGuiReportStartTransaction : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &);
};
inline tAbstractGuiCommand* Create_tGuiReportStartTransaction()
{
    return new tGuiReportStartTransaction;
}
//**********************************************************************************
class tGuiCommitTransaction : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);
};
inline tAbstractGuiCommand* Create_tGuiCommitTransaction()
{
    return new tGuiCommitTransaction;
}
//**********************************************************************************
class tGuiReportCommitTransaction : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &);
};
inline tAbstractGuiCommand* Create_tGuiReportCommitTransaction()
{
    return new tGuiReportCommitTransaction;
}
//**********************************************************************************
class tGuiCancelTransaction : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &);
};
inline tAbstractGuiCommand* Create_tGuiCancelTransaction()
{
    return new tGuiCancelTransaction;
}
//**********************************************************************************
class tGuiReportCancelTransaction : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &);
};
inline tAbstractGuiCommand* Create_tGuiReportCancelTransaction()
{
    return new tGuiReportCancelTransaction;
}
//**********************************************************************************
class tGuiDeleteFile : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiDeleteFile()
{
    return new tGuiDeleteFile;
}
//**********************************************************************************
class tGuiReportDeleteFile : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiReportDeleteFile()
{
    return new tGuiReportDeleteFile;
}
//**********************************************************************************
class tGuiGetListFiles : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiGetListFiles()
{
    return new tGuiGetListFiles;
}
//**********************************************************************************
class tReportGuiGetListFiles : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);


};
inline tAbstractGuiCommand* Create_tReportGuiGetListFiles()
{
    return new tReportGuiGetListFiles;
}
//**********************************************************************************
class tGuiGetLocalListFiles : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);
private:
    //Сканирование рабочей папки на клиенте
};
inline tAbstractGuiCommand* Create_tGuiGetLocalListFiles()
{
    return new tGuiGetLocalListFiles;
}
//**********************************************************************************
class tGuiPrepareReceiveFile : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGuiPrepareReceiveFile()
{
    return new tGuiPrepareReceiveFile;
}
//**********************************************************************************
class tGuiBreakReceiveFile : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &);

};
inline tAbstractGuiCommand* Create_tGuiBreakReceiveFile()
{
    return new tGuiBreakReceiveFile;
}
//**********************************************************************************
class tVerifyMoveDelete : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_out);

};
inline tAbstractGuiCommand* Create_tVerifyMoveDelete()
{
    return new tVerifyMoveDelete;
}
//**********************************************************************************
class tGetListModels : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);

};
inline tAbstractGuiCommand* Create_tGetListModels()
{
    return new tGetListModels;
}
//**********************************************************************************
class tReportGuiGetListServerModels : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &_in);


};
inline tAbstractGuiCommand* Create_tReportGuiGetListServerModels()
{
    return new tReportGuiGetListServerModels;
}
//**********************************************************************************
class tUpdateMainLocal : public tAbstractGuiCommand
{
    Q_OBJECT
public:
    virtual void ExeCommand(QDataStream &);


};
inline tAbstractGuiCommand* Create_tUpdateMainLocal()
{
    return new tUpdateMainLocal;
}
//**********************************************************************************
#endif // TCLIENTGUICOMMAND_H
