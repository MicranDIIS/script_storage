#ifndef sheader_h
#define sheader_h
#include<QString>
#include<QFile>
#include<QDebug>
#include<QMessageBox>
#include<QTextCodec>
#include<QDateTime>
#include<QFileInfo>
#include <QMetaType>

#pragma pack(push, 1) // 1 byte alignment

struct stPacketHeader{
   short uid;
   short titleSize; //title
   short categorySize; //category
   short stateSize; //state
   short defstateSize; //def_state
   short geomSize;
   short defgeomSize;
   };

struct stDecodeData{
    QString title;
    QString category;
    QByteArray curr_state;
    QByteArray curr_geom;
    QByteArray def_state;
    QByteArray def_geom;
    QDateTime scriptChangeTime;

    stDecodeData() {}
};

Q_DECLARE_METATYPE(stDecodeData)

#define UID 3865
static char data_mas[65535];

class Reader
{
public:
    static QString packLine(stDecodeData data,QString comment){
        if(data.title.isEmpty()) return "";
        QByteArray baTitle = data.title.toLocal8Bit().toHex();
        QByteArray baCategory = data.category.toLocal8Bit().toHex();
        stPacketHeader* header = reinterpret_cast<stPacketHeader*>(data_mas);

        header->uid = UID;
        header->titleSize = baTitle.size();
        header->categorySize = baCategory.size();
        header->stateSize = data.curr_state.toHex().size();
        header->defstateSize = data.def_state.toHex().size();
        header->geomSize = data.curr_geom.toHex().size();
        header->defgeomSize = data.def_geom.toHex().size();

        char* ptr = &data_mas[sizeof(stPacketHeader)];
        int totalSize = sizeof(stPacketHeader) +
                header->titleSize +
                header->categorySize +
                header->stateSize +
                header->defstateSize +
                header->geomSize +
                header->defgeomSize;
        memcpy(ptr,baTitle.data(),header->titleSize);
        ptr += header->titleSize;
        memcpy(ptr,baCategory.data(),header->categorySize);
        ptr += header->categorySize;
        memcpy(ptr,data.curr_state.toHex().data(),header->stateSize);
        ptr += header->stateSize;
        memcpy(ptr,data.def_state.toHex().data(),header->defstateSize);
        ptr += header->defstateSize;
        memcpy(ptr,data.curr_geom.toHex().data(),header->geomSize);
        ptr += header->geomSize;
        memcpy(ptr,data.def_geom.toHex().data(),header->defgeomSize);
        ptr += header->defgeomSize;

	quint64 tstamp = QDateTime::currentMSecsSinceEpoch() / 1000;//QDateTime::currentSecsSinceEpoch();
        if(data.scriptChangeTime.isValid()){
	    tstamp = data.scriptChangeTime.toTime_t();
        }
        memcpy(ptr,&tstamp,sizeof(quint64));
        totalSize+=sizeof(quint64);

        QByteArray t;
        t.append(comment);
        t.append(QByteArray(data_mas,totalSize).toHex());
        return QString(t);
    }

    static stDecodeData unpackLine(QString line)
    {
        stDecodeData data;
        QByteArray ba;
        if(!checkLine(line))
        {
            data.category = "";
            data.title = "New script";
            return data;
        }

        line = line.right(line.length()-2);
        ba.append(QByteArray::fromHex(line.toLocal8Bit()));
        char *p = ba.data(  );
        stPacketHeader* pLineHead = reinterpret_cast<stPacketHeader*>(p);

        char* pLineData = &p[sizeof(stPacketHeader)];
        data.title = QString(QByteArray::fromHex(QByteArray(pLineData, pLineHead->titleSize)));
        pLineData+=pLineHead->titleSize;
        data.category = QString(QByteArray::fromHex(QByteArray(pLineData, pLineHead->categorySize)));
        pLineData+=pLineHead->categorySize;
        data.curr_state.append(QByteArray::fromHex(QByteArray(pLineData,pLineHead->stateSize)));
        pLineData+=pLineHead->stateSize;
        data.def_state.append(QByteArray::fromHex(QByteArray(pLineData,pLineHead->defstateSize)));
        pLineData+=pLineHead->defstateSize;
        data.curr_geom.append(QByteArray::fromHex(QByteArray(pLineData,pLineHead->geomSize)));
        pLineData+=pLineHead->geomSize;
        data.def_geom.append(QByteArray::fromHex(QByteArray(pLineData,pLineHead->defgeomSize)));
        pLineData+=pLineHead->defgeomSize;

        int count = sumCount(pLineHead);
        quint64 tstamp = *reinterpret_cast<quint64*>(pLineData);
        if(count<ba.size()){
	    data.scriptChangeTime = QDateTime::fromTime_t(tstamp);
//		    QDateTime::fromSecsSinceEpoch(tstamp);
        }
        else{
            data.scriptChangeTime = QDateTime();//QDateTime::currentDateTime();
        }

        return data;
    }

    static int sumCount(stPacketHeader* pLineHead)
    {
        return sizeof(stPacketHeader) +
                pLineHead->categorySize +
                pLineHead->defgeomSize +
                pLineHead->defstateSize +
                pLineHead->geomSize +
                pLineHead->stateSize +
                pLineHead->titleSize;
    }

    static bool checkLine(QString line)
    {
        return line.startsWith("--190f",Qt::CaseInsensitive) || line.startsWith("##190f",Qt::CaseInsensitive);
    }

    static bool unpackFile(QString path, /*QString &lastLine,*/ QString &text, stDecodeData &data)
    {
        QFile file(path);
        if (!file.open(QFile::ReadOnly)) {
            return false;
        }
        QTextStream in(&file);
        text = in.readAll();
        in.seek(0);

        QString firstLine = in.readLine();
        QString lastLine = "";

        while(!in.atEnd())
            lastLine = in.readLine();

        if(!checkLine(lastLine))
        {
            QString comment = "--";

            int start = firstLine.indexOf("--Specific:");
            if(start==-1){
                QString comment = "##";
                start = firstLine.indexOf("##Specific:");
                if(start==-1)
                    return false;
            }
            start+=11;
            text = text.left(text.trimmed().length()).toLocal8Bit();
            data.title = firstLine.mid(start,firstLine.indexOf(";")-start).trimmed(); //Title;
            start = firstLine.indexOf("Categories:");
            data.category = (start==-1) ? "" : firstLine.mid((start+11),firstLine.indexOf(";",start)-(start+11)).trimmed();
            data.curr_geom = data.def_geom = QByteArray();
            data.curr_state = data.def_state = QByteArray();
            lastLine = packLine(data,comment);
        }
        else
        {
            text = text.left(text.trimmed().length() - lastLine.length()).toLocal8Bit();
            data = Reader::unpackLine(lastLine);
        }
        return true;
    }

    static bool packMetaInFile(QString path, stDecodeData data, QString text="")
    {
        QString lastStr;
        QString scr = "";
        QFileInfo fi(path);
        QString comment = (fi.completeSuffix().contains("py")) ? "##" : "--";
        QString newMetaLine = Reader::packLine(data, comment);

        QFile file(path);
        if (file.open(QFile::ReadOnly)) {
            QTextStream in(&file);
            scr = in.readAll();
            in.seek(0);
            while(!in.atEnd())
            {
                lastStr = in.readLine();
            }
            file.close();
        }
        else if(text.isEmpty())
            return false;

        int metaLength = Reader::checkLine(lastStr) ? lastStr.length() : 0;
        int allDataLength = scr.trimmed().length();

        if (!file.open(QFile::WriteOnly)) {
            return false;
        }

        QString txt = text.length() ? text : scr.left(allDataLength - metaLength);
        QTextStream out(&file);
        out << txt.trimmed();
        out << "\n"<<newMetaLine<<"\n";
        return true;
    }

    static bool saveFile(QString path, QString text)
    {
        QFile file(path);
        if (!file.open(QFile::WriteOnly))
            return false;
        QTextStream out(&file);
        out<<text;
        file.close();
        return true;
    }

    static void recodeFile(QString path)
    {
        QFile file(path);
        if (!file.open(QFile::ReadOnly)) {
            return;
        }
        QByteArray txt= file.readAll();
        file.close();
        QTextCodec::ConverterState state;
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QString text = codec->toUnicode(txt.constData(), txt.size(), &state);
        if (state.invalidChars > 0)
        {
            // Not a UTF-8 text - using system default locale
            QTextCodec * codecW = QTextCodec::codecForName("windows-1251");//QTextCodec::codecForLocale();
            if (!codecW)
                return;

            text = codecW->toUnicode(txt);
            if (!file.open(QFile::WriteOnly)) {
                return;
            }
            QTextStream out(&file);
            out << text;
            file.close();
        }
    }
};
#pragma pack(pop)
#endif
