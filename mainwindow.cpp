#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calculate.h"
#include <qinputdialog.h>
#include "QDebug"


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this,SIGNAL(historyUpdate(QString)),ui->textBrowser_history,SLOT(updateHistoryBoard()));

    diceThrowTimes = 1;
    successThreshold = 50;
    diceSides = 6;
    currentActCharacter =0;

    // emit updateTabletoCharacterList(currentActCharacter);

}

MainWindow::~MainWindow()
{
    delete ui;
}


/////////////////////////Protect My Dog Eye///////////////////////////////
///////////////////////// FileIO ////////////////////////////////
// 文件流区
// group_history
#pragma region FileIO

void MainWindow::on_action_loadFile_triggered()
{
    //为防止重复加载写的覆盖 有很大问题 以后有时间重写了一定要改一下这一点
    QList<Character> newCharacterList;
    characterList = newCharacterList;
    playFilePath = QFileDialog::getOpenFileName(this,"选择剧本文件","","所有文件(*.*) ;; XML文件 (*.xml)");
    setPlayFileXML();
}

void MainWindow::on_action_saveFile_triggered()
{
    savePlayFileXML();
}


//切换角色信息
void MainWindow::on_pushButton_toNextCharacter_clicked()
{
    if(characterList.length()==0) return;
    currentActCharacter = (++currentActCharacter) % characterList.length();
    qDebug()<<currentActCharacter;
    updateCharacterBoard(currentActCharacter);
    updateLabelBoard();

 saySomething("现在是"+QString::number(currentActCharacter)+"号角色"+":"+ui->label_charcterName->text()+"行动",true);
}


void MainWindow::on_pushButton_toPreCharacter_clicked()
{
    if(characterList.length()==0) return;
    currentActCharacter = (currentActCharacter+characterList.length()-1) % characterList.length();
    qDebug()<<currentActCharacter;
    updateCharacterBoard(currentActCharacter);
    updateLabelBoard();

    saySomething("现在是"+QString::number(currentActCharacter)+"号角色"+":"+ui->label_charcterName->text()+"行动",true);
}

void MainWindow::on_pushButton_toRandomCharacter_clicked()
{
    if(characterList.length()==0) return;
    currentActCharacter = qrand() % characterList.length();
    qDebug() << currentActCharacter;
    updateCharacterBoard(currentActCharacter);
    updateLabelBoard();

    saySomething("现在是"+QString::number(currentActCharacter)+"号角色"+":"+ui->label_charcterName->text()+"行动",true);
}


//显示和增加角色的记录
void MainWindow::on_pushButton_showCharacterRecords_clicked()
{
    qDebug() <<characterList[currentActCharacter].records.length();
    for(int i=0;i<characterList[currentActCharacter].records.length();i++)
    {   qDebug() <<characterList[currentActCharacter].records[i];
        saySomething(characterList[currentActCharacter].records[i],true);
    }

}

void MainWindow::on_pushButton_addCharacterRecord_clicked()
{
    //获得输入元素
     qDebug()<< "??";
    bool inputDone;
    QString record = QInputDialog::getText(this,"输入该角色的新的记录","",QLineEdit::Normal,"",&inputDone);
    if(!inputDone) return;

    qDebug()<<record;

    //更新list
    characterList[currentActCharacter].records.append(record);
    qDebug()<<"list更新完成";

    saySomething(ui->label_charcterName->text());
    saySomething("获得了一条新记录："+record+"!",true);

}
void MainWindow::on_pushButton_deleteTableHeader_clicked()
{
    QString key = ui->tableWidget_characterInfo->verticalHeaderItem(currentSelectTableRow)->text();
    emit update_tableItemDelete(key);
    ui->tableWidget_characterInfo->removeRow(currentSelectTableRow);
    ui->pushButton_deleteTableHeader->setEnabled(false);

}


void MainWindow::on_pushButton_addTableItem_clicked()
{
    //获得输入元素
     qDebug()<< "??";
    bool inputDone;
    QString item = QInputDialog::getText(this,"输入新的配对","格式如 键;值",QLineEdit::Normal,"",&inputDone);
    if(!inputDone) return;
    QStringList inputBuffer=item.split(";");
    qDebug()<<inputBuffer[0] << "  " << inputBuffer[1];

    //更新table
    int row = ui->tableWidget_characterInfo->rowCount();
    ui->tableWidget_characterInfo->insertRow(row);
    ui->tableWidget_characterInfo->setVerticalHeaderItem(row,new QTableWidgetItem(inputBuffer[0]));
    ui->tableWidget_characterInfo->setItem(row,0,new QTableWidgetItem(inputBuffer[1]));

    qDebug()<<"table更新完成";

    //更新list
    characterList[currentActCharacter].property.insert(inputBuffer[0],inputBuffer[1]);
     qDebug()<<"list更新完成";

}


void MainWindow::on_tableWidget_characterInfo_itemClicked(QTableWidgetItem *item)
{
    currentSelectTableRow =  item->row();
    if(characterList.length()!=0){
          ui->pushButton_deleteTableHeader->setEnabled(true);
    }

}


//更新tablewidget
void MainWindow::updateCharacterBoard(int characterIndex){
    ui->tableWidget_characterInfo->clear();
    currentActCharacter = characterIndex;
    if(characterList.length()==0){
        qDebug() << "玩家信息列表写入失败";
        return;
    }

    //遍历property写入
    QMap<QString,QVariant> propertyBuffer = characterList[characterIndex].property;
    QMap<QString,QVariant>::Iterator it = propertyBuffer.begin(),itend = propertyBuffer.end();


    for (int i=0;it != itend; it++,i++){
        ui->tableWidget_characterInfo->setVerticalHeaderItem(
                    i,new QTableWidgetItem(it.key()));

        ui->tableWidget_characterInfo->setItem(
                    i,0,new QTableWidgetItem(it.value().toString()));

        // qDebug() << it.key();
        // qDebug() << it.value();
    }
    ui->tableWidget_characterInfo->setHorizontalHeaderItem(0,new QTableWidgetItem("value"+QString::number(currentActCharacter)));
}

//反向更新
void MainWindow::update_tableItemDelete(QString key){
    QMap<QString, QVariant>::iterator it;

    qDebug() << characterList[currentActCharacter].property.contains(key);
    if(characterList[currentActCharacter].property.contains(key))
    {
        it = characterList[currentActCharacter].property.find(key);
        qDebug() <<it.key() << it.value().toString()<< "删除成功！";
        characterList[currentActCharacter].property.erase(it);

    }





}

//更新玩家标签信息label
void MainWindow::updateLabelBoard(){
    QMap<QString,QVariant>::Iterator it;
    if(characterList[currentActCharacter].property.contains("name"))
    {
        it = characterList[currentActCharacter].property.find("name");
        ui->label_charcterName->setText(it.value().toString());
    }
    else
    {
         ui->label_charcterName->setText("Anonymous");
    }


}



void MainWindow::setPlayFileXML(){


    QFile playFile(playFilePath);

    if (!playFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug()<<"Error: cannot open file";
        return;
    }

    int characterIndex=0; //设定最初的玩家编号为0
    QString characterSectionName;
    Character characterBuffer;
    //    QVector<QString> headerBuffer;
    //    QVector<QVariant> valueBuffer;


    QXmlStreamReader playFileReader;
    QXmlStreamAttributes streamAttr;
    playFileReader.setDevice(&playFile);
    qDebug() << playFilePath ;
    while(!playFileReader.atEnd()){

        QXmlStreamReader::TokenType type = playFileReader.readNext();
        playFileReader.readNext();

        streamAttr = playFileReader.attributes();

        if (type != QXmlStreamReader::Characters || playFileReader.isWhitespace())
        {
            continue;
            qDebug() << "skipped";
        }

        //当这一小节存在ID值时，可认为要写入的玩家的档案序号为该值
        if(streamAttr.hasAttribute("id")){
            characterIndex = streamAttr.value("id").toInt();
            qDebug() << "读取到id信息";
        }
        else if(playFileReader.isEndElement() && playFileReader.name()  == "character"){

            //根据位置配对插入
            for(int i =0;i<headerBuffer.length();i++){
                characterBuffer.property.insert(headerBuffer[i],valueBuffer[i]);
            }

            characterBuffer.index = characterIndex;

            characterList.append(characterBuffer);
            //qDebug() << "检查list中是否包含recrod" << characterList.last().records.length();

            //把各个具体的人的buffer清空
            headerBuffer.clear();
            valueBuffer.clear();
            characterBuffer.property.clear();
            characterBuffer.records.clear();
        }
        //读到档案记录
        else if(playFileReader.name() == "record"){
            characterBuffer.records.append( playFileReader.readElementText());
            qDebug() <<  "record写入" << characterBuffer.records.last();
            qDebug() <<  "record确认" << characterBuffer.records.length();
        }

        //写入数据 header
        else if(playFileReader.name() == "h"){
            headerBuffer.append(playFileReader.readElementText());
            qDebug() <<  "信息写入" << headerBuffer.last();
        }
        //写入数据 character value
        else if(playFileReader.name() == "c"){
            valueBuffer.append(playFileReader.readElementText());
            qDebug() <<  "信息写入" << valueBuffer.last();
        }
    }


    playFile.close();

    //发个更新信号呗
    emit updateCharacterBoard(0);
    emit updateLabelBoard();

    //启用增加条目按钮
    ui->pushButton_addTableItem->setEnabled(true);



}

void MainWindow::savePlayFileXML(){
    if(playFilePath == nullptr){
        playFilePath = "newPlayFile.xml";
    }

    QFile playFile(playFilePath);


    if (!playFile.open(QFile::ReadWrite | QFile::Text)){
        qDebug() << "Error: cannot open file";
        return;
    }

    QXmlStreamWriter writer(&playFile);

    writer.setAutoFormatting(true);

    writer.writeStartDocument();
    writer.writeStartElement("root");
    for(int n=0;n<characterList.length();n++){

        qDebug() << "共有" << characterList.length() << "号玩家";
        qDebug() << "正在保存" << n << "号玩家";

        //写入header表
        writer.writeStartElement("header");
        QMap<QString,QVariant>::Iterator ith;
        writer.writeAttribute("id",QString::number(n));
        for(ith=characterList[n].property.begin();ith!=characterList[n].property.end();ith++){
            writer.writeTextElement("h",ith.key());
            qDebug() << "写入" << ith.key();
        }

        writer.writeEndElement();

        //写入character表
        writer.writeStartElement("character");

        //写入属性
        QMap<QString,QVariant>::Iterator itc;
        writer.writeAttribute("id",QString::number(n));
        for(itc=characterList[n].property.begin();itc!=characterList[n].property.end();itc++){
            writer.writeTextElement("c",itc.value().toString());
            qDebug() << "写入" << itc.value().toString();
        }


        //写入records
        for(int ir=0;ir<characterList[n].records.length();ir++){
            writer.writeTextElement("records",characterList[n].records.at(ir));
            qDebug() << "写入" << characterList[n].records.at(ir);
        }

        qDebug() << n <<"号玩家保存完毕";

        //两个都写完了
         writer.writeEndElement();
    }

    qDebug() << "保存成功！";
    writer.writeEndElement(); //root
    writer.writeEndDocument();
    playFile.close();


}
#pragma endregion FileIO





/////////////////////////Protect My Dog Eye///////////////////////////////
///////////////////////// group_history ////////////////////////////////
// 历史记录面板区
// group_history
#pragma region group_history


void MainWindow::on_checkBox_noRecordHistory_clicked()
{
    ui->checkBox_noRecordHistory->setText("别点了 没用的");
}

void MainWindow::saySomething(QString words,bool updateImmediately){

    historyLineBuffer += words;

    if(updateImmediately){
        emit updateHistoryBoard(historyLineBuffer);
    }
}

void MainWindow:: updateHistoryBoard(QString &infoBuffer){
    //   QList<QString>::Iterator it = infoBuffer.begin(),itend = infoBuffer.end();
    //   for (int i=0;it != itend; it++,i++){
    //        ui->textBrowser_history->append(infoBuffer.at(i));
    //    }

    //更新历史记录面板，并清空
    historyInfoBuffer.append(infoBuffer);
    ui->textBrowser_history->append(infoBuffer);
    infoBuffer.clear();
}
#pragma endregion group_history




/////////////////////////Protect My Dog Eye///////////////////////////////
///////////////////////// group_diceThrow ////////////////////////////////
// 骰子信息区
// group_diceThrow
#pragma region group_diceThrow


//开启具体数值控制判定
void MainWindow::on_checkBox_exactSwitch_stateChanged(int arg)
{
    if(arg == 2)
    {
        ui->pushButton_general->setText("根据自定义数值判定");
        successThreshold = ui->exactSlider->value();
        ui->pushButton_veryEasy->setEnabled(false);
        ui->pushButton_easy->setEnabled(false);
        ui->pushButton_hard->setEnabled(false);
        ui->pushButton_impossible->setEnabled(false);
        ui->pushButton_zero->setEnabled(false);

        saySomething("god启用了精确数值判定系统！",true);
    }
    else if(arg == 0)
    {
        ui->pushButton_general->setText("掷骰(一般情况)");

        ui->pushButton_veryEasy->setEnabled(true);
        ui->pushButton_easy->setEnabled(true);
        ui->pushButton_hard->setEnabled(true);
        ui->pushButton_impossible->setEnabled(true);
        ui->pushButton_zero->setEnabled(true);

        saySomething("god关闭了精确数值判定系统！",true);
    }

}

//感觉滑动条来改变成功阈值
void MainWindow::on_exactSlider_valueChanged()
{
    successThreshold = 100 - ui->exactSlider->value();
    ui->label_thresholdValue->setText(QString::number((int)successThreshold));
}

//增加骰子丢的次数
void MainWindow::on_pushButton_addDThrow_clicked()
{
    diceThrowTimes++;

    ui->label_diceThrowTimes->setText("当前骰子连掷个数:"+QString::number(diceThrowTimes));

}

//减少骰子丢的次数
void MainWindow::on_pushButton_subtractThrow_clicked()
{
    if(diceThrowTimes >1) diceThrowTimes--;
    ui->label_diceThrowTimes->setText("当前骰子连掷个数:"+QString::number(diceThrowTimes));

}

//重置骰子丢的次数
void MainWindow::on_pushButton_timesReset_clicked()
{
    diceThrowTimes = 1;
    ui->label_diceThrowTimes->setText("当前骰子连掷个数:"+QString::number(diceThrowTimes));
    saySomething("重置了骰子的连掷个数为 1",true);
}

//那几个按钮丢骰子
void MainWindow::on_pushButton_veryEasy_clicked()
{
    on_anyJudgePushButton_clicked(10,true);
}
void MainWindow::on_pushButton_easy_clicked()
{
    on_anyJudgePushButton_clicked(30,true);
}
void MainWindow::on_pushButton_general_clicked()
{
    if(!ui->checkBox_exactSwitch->isChecked())
        on_anyJudgePushButton_clicked(50,true);
    else
        on_anyJudgePushButton_clicked(successThreshold,true);
}

void MainWindow::on_pushButton_hard_clicked()
{
    on_anyJudgePushButton_clicked(70,true);
}

void MainWindow::on_pushButton_impossible_clicked()
{
    on_anyJudgePushButton_clicked(90,true);
}

void MainWindow::on_pushButton_zero_clicked()
{
    on_anyJudgePushButton_clicked(99,true);
}

//仅获得数字
void MainWindow::on_pushButton_getNumber_clicked()
{
    on_anyJudgePushButton_clicked(successThreshold,false);
    ui->label_judgeInfo->clear();
}

void MainWindow::on_anyJudgePushButton_clicked(int threshold,bool needJudgement){

    successThreshold = threshold;

    //获得判定数字
    int judgeNumber = Calculate().getRandomNumber(historyLineBuffer,diceSides,diceThrowTimes);
    ui->diceNumberDisplayer->setText(QString::number(judgeNumber));
    ui->diceNumberDisplayer->setAlignment(Qt::AlignHCenter); //对齐

    //进行判断

    if(needJudgement){


        if(Calculate().judgeDiceResult(judgeNumber,successThreshold)){
            ui->label_judgeInfo->setText("成功!");
            ui->label_judgeInfo->setStyleSheet("color:green;");
        }
        else{
            ui->label_judgeInfo->setText("失败!");
            ui->label_judgeInfo->setStyleSheet("color:red;");
        }
        saySomething(ui->label_charcterName->text());
        saySomething("进行了一次判定，");
        saySomething("这次判定的阈值是"+QString::number(successThreshold)+",并且");
        saySomething(ui->label_judgeInfo->text());
    }

    saySomething("",true);


}

#pragma endregion group_diceThrow






/////////////////////////Protect My Dog Eye///////////////////////////////
///////////////////////// group_diceType ////////////////////////////////
// 骰子信息区
// group_diceType
#pragma region group_diceType

void MainWindow::on_radioButton_d2_clicked()
{
    diceSides=2;
}

void MainWindow::on_radioButton_d6_clicked()
{
    diceSides=6;
}

void MainWindow::on_radioButton_d12_clicked()
{
    diceSides=12;
}

void MainWindow::on_radioButton_d16_clicked()
{
    diceSides=16;
}

void MainWindow::on_radioButton_d20_clicked()
{
    diceSides=20;
}

void MainWindow::on_radioButton_d28_clicked()
{
    diceSides=28;
}

void MainWindow::on_radioButton_d34_clicked()
{
    diceSides=34;
}

void MainWindow::on_radioButton_d100_clicked()
{
    diceSides=100;
}

void MainWindow::on_lineEdit_customDiceSides_textChanged(const QString &arg)
{
    validatorInt = new QIntValidator(1,99,this);
    if( ui->radioButton_customSides->isChecked())
    {
        diceSides = arg.toInt();
    }

}

#pragma endregion group_diceType







//////////////////////////////Protect My Dog Eye///////////////////////////////
/////////////////////////// group_diceType ////////////////////////////////
//// 骰子信息区
//// group_diceType
//#pragma region group_diceType

//#pragma endregion group_diceType

/////

//        QXmlStreamReader::TokenType type = playFileReader.readNext();
//        if (type == QXmlStreamReader::Characters && !playFileReader.isWhitespace()){
//            playFileDataTemp.append(playFileReader.text().toUtf8());
//        }










