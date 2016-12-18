#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QStringList>
#include <QCoreApplication>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValueRef>
#include <QFile>


typedef struct CoordinatePair{
    double lat,lon;
    CoordinatePair(double ilat,double ilon): lat(ilat),lon(ilon){}
}CoordinatePair;

int main(int argc, char *argv[])
{
    QTextStream out(stdout);
    char c;
    QList <float> antHeight;
    QList <float>::iterator iantHeight;
    double startLat=0.0,startLon=0.0,endLat=0.0,endLon=0.0;
    int numLat=10,numLong=10,dim=1;
    QList <char> devType;
    QList <char>::iterator idevType;
    QList <char> devClass;
    QList <char>::iterator idevClass;
    int iter=1,sleepSecs=60;
    int confi=99;
    bool isSlave=false;
    QStringList buff;
    QStringList::iterator i;

    QCoreApplication a(argc, argv);
    //a.exec();


    QList <CoordinatePair> coordinateList;
    QList <CoordinatePair>::iterator icoordinateList;
    int reqCount=0;


    out<<"============================================"<<endl;
    out<<"==============WSDB Query Tool==============="<<endl;
    out<<"=================Ver 0.1===================="<<endl;
    out<<"============================================"<<endl;
    if (argc==1)
    {
        out<<"//Please specify input parameters."<<endl;
        out<<"==================HELP======================"<<endl;
        out<<"-h    Antenna height in meter (defalut 5)"<<endl;
        out<<"-x    Coordinate array start point - latitute "<<endl;
        out<<"-y    Coordinate array start point - longitute "<<endl;
        out<<"-i    Number of coordinate samples - latitute (default 10)"<<endl;
        out<<"-j    Number of coordinate samples - longitute (default 10)"<<endl;
        out<<"-r    Coordinate array end point - latitute "<<endl;
        out<<"-s    Coordinate array end point - longitute "<<endl;
        out<<"-m    Coordinate grid dimension (default 1) "<<endl;
        out<<"-t    Device Type (default A) "<<endl;
        out<<"-c    Device Emission Class (default 5) "<<endl;
        out<<"-n    Number of query iterations (default 1) "<<endl;
        out<<"-d    Query iteration seperation in seconds (default 60) "<<endl;
        out<<"-p    Confidence probability in percent (default 99) "<<endl;
        out<<"-f    Slave Flag (default 0) "<<endl;
        out<<"============================================"<<endl;
        return 0;
    }
    while ((c = getopt (argc, argv, "h:x:y:r:s:m:t:c:i:d:p:s")) != -1)
    {
        switch (c)// channel selection
        {
        case 'h':
            buff = QString(optarg).split(",");
            //            QStringList::iterator i;
            for (i = buff.begin();i != buff.end();++i)
            {
                if ((*i)=="1.5")
                    antHeight.append(1.5);
                else if ((*i)=="5")
                    antHeight.append(5);
                else if ((*i)=="10")
                    antHeight.append(10);
                else if ((*i)=="15")
                    antHeight.append(15);
                else if ((*i)=="20")
                    antHeight.append(20);
                else if ((*i)=="30")
                    antHeight.append(30);
                else
                {
                    out<<"//Antenna height error. Config failed."<<endl;
                    return 0;
                }
            }
            if (buff.isEmpty())
            {
                out<<"//Antenna height error. Config failed."<<endl;
                return 0;
            }
            else
                buff.clear();
            break;
        case 'x':
            startLat = atof(optarg);
            if (startLat==0)
            {
                out<<"//Start point latitude error. Config failed."<<endl;
                return 0;
            }
            break;
        case 'y':
            startLon = atof(optarg);
            if (startLon==0)
            {
                out<<"//Start point longitude error. Config failed."<<endl;
                return 0;
            }
            break;
        case 'i':
            numLat = atoi(optarg);
            if (numLat==1 |numLat==0)
            {
                out<<"//Number of samples in latitude error. Config failed."<<endl;
                return 0;
            }
            break;
        case 'j':
            numLong = atoi(optarg);
            if (numLong==1 | numLong==0)
            {
                out<<"//Number of samples in longitude error. Config failed."<<endl;
                return 0;
            }
            break;
        case 'r':
            endLat = atof(optarg);
            if (endLat==0)
            {
                out<<"//End point latitude error. Config failed."<<endl;
                return 0;
            }
            break;
        case 's':
            endLon = atof(optarg);
            if (endLon==0)
            {
                out<<"//Start point longitude error. Config failed."<<endl;
                return 0;
            }
            break;
        case 'm':
            if (atoi(optarg)==1)
                dim=1;
            else if (atoi(optarg)==2)
                dim=2;
            else
            {
                out<<"//Coordinate grid dimension error. Config failed."<<endl;
                return 0;
            }
            break;
        case 't':
            buff = QString(optarg).split(",");
            //            QStringList::iterator i;
            for (i = buff.begin();i != buff.end();++i)
            {
                if ((*i)=="A")
                    devType.append('A');
                else if ((*i)=="B")
                    devType.append('B');
                else
                {
                    out<<"//Device Emission Type error. Config failed."<<endl;
                    return 0;
                }
            }
            if (buff.isEmpty())
            {
                out<<"//Device Emission Type error. Config failed."<<endl;
                return 0;
            }
            else
                buff.clear();
            break;
        case 'c':
            buff = QString(optarg).split(",");
            //            QStringList::iterator i;
            for (i = buff.begin();i != buff.end();++i)
            {
                if ((*i)=="1")
                    devClass.append('1');
                else if ((*i)=="2")
                    devClass.append('2');
                else if ((*i)=="3")
                    devClass.append('3');
                else if ((*i)=="4")
                    devClass.append('4');
                else if ((*i)=="5")
                    devClass.append('5');
                else
                {
                    out<<"//Device Class Type error. Config failed."<<endl;
                    return 0;
                }
            }
            if (buff.isEmpty())
            {
                out<<"//Device Class Type error. Config failed."<<endl;
                return 0;
            }
            else
                buff.clear();
            break;
        case 'n':
            iter = atoi(optarg);
            if (iter==0)
            {
                out<<"//Number of iterations error. Config failed."<<endl;
                return 0;
            }
            break;
        case 'd':
            sleepSecs = atoi(optarg);
            if (sleepSecs==0)
            {
                out<<"//Query iteration seperation error. Config failed."<<endl;
                return 0;
            }
            break;
        case 'p':
            confi = atoi(optarg);
            if (confi>=100 | confi<=0)
            {
                out<<"//Confidence probability error. Config failed."<<endl;
                return 0;
            }
            break;
        case 'f':
            if (atoi(optarg)==1)
                isSlave=true;
            else if (atoi(optarg)==0)
                isSlave=false;
            else
            {
                out<<"//Slave flag error. Config failed."<<endl;
                return 0;
            }
            break;
        }
    }

    //default
    if (antHeight.isEmpty())
        antHeight.append(5);
    if (devType.isEmpty())
        devType.append('A');
    if (devClass.isEmpty())
        devClass.append('5');

    //parameter check
    if (numLat>1 && numLong>1)
    {
        if ((!startLat)|(!startLon)|(!endLat)|(!endLon))
        {
            out<<"//Coordinate missing. Config failed."<<endl;
            return 0;
        }
        else
        {
            if (dim == 1)
            {
                for (int i=0;i<numLat;i++)
                {
                    numLong == numLat;
                    coordinateList.append(CoordinatePair((startLat+(endLat-startLat)*i/(double)numLat)
                                                         ,(startLon+(endLon-startLon)*i/(double)numLong)));
                }
            }
            else if (dim == 2)
            {
                for (int i=0;i<numLat;i++)
                {
                    for (int j=0;j<numLong;j++)
                    {
                        coordinateList.append(CoordinatePair((startLat+(endLat-startLat)*i/(double)numLat)
                                                             ,(startLon+(endLon-startLon)*j/(double)numLong)));
                    }
                }
            }
        }
    }
    else if (numLat==1 | numLong==1)
    {
        numLong == 1;//force single point
        numLat == 1;
        if ((!startLat)|(!startLon))
        {
            out<<"//Coordinate missing. Config failed."<<endl;
            return 0;
        }
        else
        {
            coordinateList.append(CoordinatePair(startLat,startLon));
        }
    }

    out<<"//Config successful."<<endl;
    out<<""<<endl;
    out<<endl;
    out<<"--------------------------------------------"<<endl;
    out<<"Antenna height: ";
    for (iantHeight = antHeight.begin();iantHeight != antHeight.end();++iantHeight)
    {
        out<<(*iantHeight);
    }
    out<<endl;
    out<<"Start latitude: "<<QString::number(startLat,'f',6)<<endl;
    out<<"End latitude: "<<QString::number(endLat,'f',6)<<endl;
    out<<"Start longitude: "<<QString::number(startLon,'f',6)<<endl;
    out<<"End longitude: "<<QString::number(endLon,'f',6)<<endl;
    out<<"Grid Dimension: "<<dim<<endl;
    out<<"Number of Samples: "<<QString::number(coordinateList.length())<<endl;
    out<<"Device Class: ";
    for (idevClass = devClass.begin();idevClass != devClass.end();++idevClass)
    {
        out<<(*idevClass);
    }
    out<<endl;
    out<<"Device Emission Type: ";
    for (idevType = devType.begin();idevType != devType.end();++idevType)
    {
        out<<(*idevType);
    }
    out<<endl;
    out<<"Number of iterations: "<<iter<<endl;
    out<<"Number of Http requests each iteration: "<<antHeight.length()*coordinateList.length()*devClass.length()*devType.length()<<endl;
    out<<"Sleeping time between iterations: "<<sleepSecs<<endl;
    out<<"Confidence probability: "<<confi<<endl;
    out<<"Is slave: "<<isSlave<<endl;
    out<<"--------------------------------------------"<<endl;

    QNetworkAccessManager *postManager = new QNetworkAccessManager(0);
    //connect(postManager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));

    //construct input JSON template
    QJsonObject inputObject;

    QJsonValue jsonrpc("2.0");
    QJsonValue method("spectrum.paws.getSpectrum");
    QJsonObject params;
    QJsonValue type("AVAIL_SPECTRUM_REQ");
    QJsonValue version("1.0");
    QJsonObject deviceDesc;
    QJsonValue serialNumber(20150907);
    QJsonValue manufacturerId("QMU");
    QJsonValue modelId("virtual-radio");
    QJsonValue rulesetIds(QJsonArray{"ETSI-EN-301-598-1.1.1"});
    QJsonValue etsiEnDeviceType;
    QJsonValue etsiEnDeviceCategory;
    QJsonValue etsiEnDeviceEmissionsClass;
    QJsonValue etsiEnTechnologyId("wsTechnology");

    QJsonObject location;
    QJsonObject point;
    QJsonObject center;
    QJsonValue latitude;
    QJsonValue longitude;
    QJsonValue semiMajorAxis(0);
    QJsonValue semiMinorAxis(0);
    QJsonValue orientation(0);
    QJsonValue confidence;
    QJsonObject antenna;
    QJsonValue height;
    QJsonValue heightType("AGL");
    QJsonValue heightUncertainty(0);
    QJsonValue id(0);

    etsiEnDeviceCategory= QJsonValue((isSlave)?"slave":"master");
    confidence = QJsonValue(confi);

    //*(inputObject.find("params")->toObject().find("deviceDesc")->toObject().find("estiEnDeviceCategory")) = (isSlave)?(QJsonValue("slave")):(QJsonValue("master"));
    //*(inputObject.find("params")->toObject().find("location")->toObject().find("confidence")) = QJsonValue(confi);
    QFile outputFile(QDate::currentDate().toString("yyyyMMdd")+QTime::currentTime().toString("hhmmzz")+".csv");
    QTextStream *fileOut;
    if (outputFile.open(QFile::ReadWrite))
    {
        fileOut = new QTextStream(&outputFile);
        out<<"Streaming to file: "+outputFile.fileName()<<endl;
    }
    else
    {
        out<<"File creation failed."<<endl;
        return 0;
    }


    for (int iCount=0;iCount<iter;iCount++)
    {
        out<<"Entering "<<QString::number(iCount+1)<<"th iteration"<<endl;
        for( icoordinateList = coordinateList.begin();icoordinateList != coordinateList.end();++icoordinateList)
            for (iantHeight = antHeight.begin();iantHeight != antHeight.end();++iantHeight)
                for (idevClass = devClass.begin();idevClass != devClass.end();++idevClass)
                    for (idevType = devType.begin();idevType != devType.end();++idevType)
                    {
                        //*(inputObject.find("params")->toObject().find("antenna")->toObject().find("height")) = QJsonValue(*iantHeight);
                        //*(inputObject.find("params")->toObject().find("deviceDesc")->toObject().find("etsiEnDeviceType")) = QJsonValue(*idevType);
                        //*(inputObject.find("params")->toObject().find("deviceDesc")->toObject().find("etsiEnDeviceEmissionsClass")) = QJsonValue((int)((*idevClass)-'0'));
                        //*(inputObject.find("params")->toObject().find("location")->toObject().find("point")->toObject().find("center")->toObject().find("latitude")) = QJsonValue(icoordinateList->lat);
                        //*(inputObject.find("params")->toObject().find("location")->toObject().find("point")->toObject().find("center")->toObject().find("longitude")) = QJsonValue(icoordinateList->lon);
                        height=QJsonValue(*iantHeight);
                        etsiEnDeviceType=QJsonValue(QString(1,QChar(*idevType)));
                        etsiEnDeviceEmissionsClass=QJsonValue((int)((*idevClass)-'0'));
                        latitude=QJsonValue(icoordinateList->lat);
                        longitude=QJsonValue(icoordinateList->lon);

                        //construct input JSON
                        deviceDesc = QJsonObject{
                                {"serialNumber", serialNumber},
                                          {"manufacturerId", manufacturerId},
                                          {"modelId", modelId},
                                          {"rulesetIds", rulesetIds},
                                          {"etsiEnDeviceType", etsiEnDeviceType},
                                          {"etsiEnDeviceCategory", etsiEnDeviceCategory},
                                          {"etsiEnDeviceEmissionsClass", etsiEnDeviceEmissionsClass},
                                          {"etsiEnTechnologyId", etsiEnTechnologyId}
                    };
                        center = QJsonObject{
                        {"latitude",latitude},
                                {"longitude",longitude}
                    };
                        point = QJsonObject{
                        {"center",center},
                        {"semiMajorAxis", semiMajorAxis},
                                            {"semiMinorAxis", semiMinorAxis},
                                            {"orientation", orientation}
                    };
                        location = QJsonObject{
                        {"point",point},
                        {"confidence",confidence}
                    };
                        antenna = QJsonObject{
                        {"height",height},
                        {"heightType",heightType},
                        {"heightUncertainty",heightUncertainty}

                    };
                        params = QJsonObject{
                                {"type",type},
                                {"version",version},
                                {"deviceDesc",deviceDesc},
                        {"location",location},
                        {"antenna",antenna}
                    };
                        inputObject = QJsonObject{
                        {"jsonrpc",jsonrpc},
                        {"method",method},
                        {"params",params},
                        {"id",id}
                    };
                        QJsonDocument inputJSON(inputObject);
                        QByteArray inputJSONByteArray = inputJSON.toJson(QJsonDocument::Indented);
                        qDebug()<<QString(inputJSONByteArray);

                        reqCount++;
                        out<<"request count: "<<reqCount<<"/"<<iter*antHeight.length()*coordinateList.length()*devClass.length()*devType.length();
                        QNetworkRequest postRequest(QUrl("https://wsdb-deviceapi.thingzone.uk/paws?token=3c1a91fe-f5ed-4874-bc4e-d9932a74d1b2"));
                        postRequest.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
                        QNetworkReply *replyHttp = postManager->post(postRequest,inputJSONByteArray);
                        while (!(replyHttp->isFinished()))
                        {
                            //do nothing, blocking
                            out<<".";
                            out.flush();
                            QThread::msleep(200);
                            a.processEvents();
                        }
                        out<<"="<<endl;
                        if (replyHttp->error()!=QNetworkReply::NoError)
                            out<<"Request Error ("<<replyHttp->errorString()<<")"<<endl;
                        QByteArray replyByteArray = replyHttp->readAll();
                        delete replyHttp;
                        QJsonObject replyJSON = QJsonDocument::fromJson(replyByteArray).object();
                        QList <int> resultPwr;
                        for (int s=0;s<41;s++)
                        {
                            resultPwr.append(-1);
                        }
                        (*fileOut)<<QString::number(iCount+1)<<",";
                        (*fileOut)<<QString::number(((reqCount-1)%(antHeight.length()*coordinateList.length()*devClass.length()*devType.length()))+1)<<",";
                        (*fileOut)<<replyJSON.find("result")->toObject().find("timestamp")->toString()<<",";
                        (*fileOut)<<QString::number(icoordinateList->lat,'f',6)<<",";
                        (*fileOut)<<QString::number(icoordinateList->lon,'f',6)<<",";
                        (*fileOut)<<replyJSON.find("result")->toObject().find("timestamp")->toString()<<",";
                        (*fileOut)<<replyJSON.find("result")->toObject().find("deviceDesc")->toObject().find("etsiEnDeviceType")->toString()<<",";
                        (*fileOut)<<replyJSON.find("result")->toObject().find("deviceDesc")->toObject().find("etsiEnDeviceEmissionsClass")->toString()<<",";
                        (*fileOut)<<replyJSON.find("result")->toObject().find("deviceDesc")->toObject().find("etsiEnDeviceCategory")->toString()<<",";

                        QJsonArray resultArray = replyJSON.find("result")->toObject().find("spectrumSpecs")->toArray().begin()->toObject().
                                                   find("spectrumSchedules")->toArray().begin()->toObject().find("spectra")->toArray().begin()
                                                   ->toObject().find("profiles")->toArray().begin()->toArray();
                        QJsonArray::iterator iResultArray;
                        for (iResultArray=resultArray.begin();iResultArray!=resultArray.end();++iResultArray)
                        {
                            //qDebug()<<(int)((iResultArray->toObject().find("hz")->toDouble()-470000000)/8000000);
                            resultPwr[(int)((iResultArray->toObject().find("hz")->toDouble()-470000000)/8000000)]= iResultArray->toObject().find("dbm")->toInt();

                        }
                        for (int s=0;s<41;s++)
                        {
                            *fileOut<<QString::number(resultPwr[s])<<",";
                        }
                        *fileOut<<endl;
                        qDebug()<<"Done.";
                    }
        out<<"Leaving "<<QString::number(iCount+1)<<"th iteration"<<endl;
        out.flush();
        if (iCount<iter-1)
            QThread::sleep(sleepSecs);
    }

    outputFile.close();
    delete postManager;
    delete fileOut;
    return 0;
}
