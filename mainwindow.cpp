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
    currentActEnemy = 0;
    currentActionTimes=0;
    totalPlayTimes = "1";

}

MainWindow::~MainWindow()
{
    delete ui;
}

/////////////////////////Protect My Dog Eye///////////////////////////////
///////////////////////// 敌方面板区 ////////////////////////////////
// 敌方面板区
#pragma region enemyBoard


void MainWindow::on_pushButton_addEnemyItem_clicked()
{
    //获得输入元素
    bool inputDone;
    QString item = QInputDialog::getText(this,"输入新的配对","格式如 键;值",QLineEdit::Normal,"",&inputDone);
    if(!inputDone) return;
    QStringList inputBuffer=item.split(";");
    qDebug()<<inputBuffer[0] << "  " << inputBuffer[1];

    //更新table
    int row = ui->tableWidget_enemyInfo->rowCount();
    ui->tableWidget_enemyInfo->insertRow(row);
    ui->tableWidget_enemyInfo->setVerticalHeaderItem(row,new QTableWidgetItem(inputBuffer[0]));
    ui->tableWidget_enemyInfo->setItem(row,0,new QTableWidgetItem(inputBuffer[1]));

    qDebug()<<"table更新完成";

    //更新list
    enemyList[currentActEnemy].property.insert(inputBuffer[0],inputBuffer[1]);
    qDebug()<<"list更新完成";
}


void MainWindow::on_pushButton_deleteEnemyItem_clicked()
{
    QString key = ui->tableWidget_enemyInfo->verticalHeaderItem(currentSelectTableRow)->text();
    emit update_enemyTableItemDelete(key);
    ui->tableWidget_enemyInfo->removeRow(currentSelectTableRow);
    ui->pushButton_deleteEnemyItem->setEnabled(false);
}
void MainWindow::on_pushButton_destroyEnemy_clicked()
{

}
void MainWindow::on_pushButton_duplicateEnemy_clicked()
{

}

void MainWindow::on_pushButton_product_clicked()
{

}

void MainWindow::on_pushButton_addEnemy_clicked()
{

}

void MainWindow::on_horizontalSlider_quickSelect_valueChanged(int value)
{
    currentActEnemy = value % enemyList.length();
}

void MainWindow::setSwitchEnemySlider(){
    if(enemyList.length()==0) return;
    ui->horizontalSlider_quickSelect->setMinimum(0);
    ui->horizontalSlider_quickSelect->setMaximum(enemyList.length()-1);

}
#pragma endregion enemyBoard


/////////////////////////Protect My Dog Eye///////////////////////////////
///////////////////////// BattleArea ////////////////////////////////
// 战斗信息区域
// group_battle
#pragma region BattleArea

void MainWindow:: updateLabelActionTimes(int times){
    currentActionTimes=times;
    ui->label_actionTimes->setText(QString::number(times));

}

//根据文字加载对应的计算公式，这是描述层，不是具体逻辑层
void MainWindow:: loadBattleDetail(){
    //*////////////////////////////////////////////
    //*todo:
    //*     以后写sk[n]exprdcd dicide 决定具体是哪一个表达的表达式
    //*     combox拉出来的技能列表可以依据类型来设置一个图标
    //*

    //清空
    skillComboxInfo.clear();
    skillDescMap.clear();

    QRegularExpression rx_skillName("\\d+name\\b");
    QRegularExpression rx_skillExpr("\\d+expr\\b");
    QRegularExpression rx_skillIndex("\\d+");

    QStringList tmpCombox;
    QString tmpName;
    QString tmpExpr;
    QRegularExpressionMatch match;
    QString skillIndex;
    //skillDescMap

    //查找所有玩家的信息
    for(int i=0;i<characterList.length();i++){
        QMap<QString,QVariant>::iterator it,itend;
        it = characterList[i].property.begin();
        itend = characterList[i].property.end();

        for(;it!=itend;it++){
            //整个信息列表已经被Map排序过，因此必定是先找到expr，再找到name
            //这里默认了有name必定有expr

            match=rx_skillExpr.match(it.key());
            if(match.hasMatch()){
                skillIndex = rx_skillIndex.match(it.key()).captured(0);
                tmpExpr = it.value().toString();
                qDebug() << tmpExpr << "技能编号" << skillIndex;

                continue;
            }

            match=rx_skillName.match(it.key());
            if(match.hasMatch()){

                tmpName = it.value().toString();
                qDebug() << tmpName;
                tmpCombox <<tmpName;

                //将技能记录插入到map中,其标记为 玩家或敌人 序号 技能名 ： 技能表达式
                skillDescMap.insert("c"+QString::number(i)+tmpName,tmpExpr);
                qDebug() <<"插入的名称是" <<"c"+QString::number(i)+tmpName <<tmpExpr;
            }
        }
        //遍历单个characterlist完成，将得到的所有skname存储
        skillComboxInfo.insert("c"+QString::number(i),tmpCombox);
        //清空
        tmpCombox.clear();
    }


    //查找所有敌人的信息
    for(int i=0;i<enemyList.length();i++){

        QMap<QString,QVariant>::iterator it,itend;
        it = enemyList[i].property.begin();
        itend = enemyList[i].property.end();

        for(;it!=itend;it++){

            match=rx_skillExpr.match(it.key());
            if(match.hasMatch()){
                skillIndex = rx_skillIndex.match(it.key()).captured(0);
                tmpExpr = it.value().toString();
                qDebug() << tmpExpr << "技能编号" << skillIndex;
                continue;
            }

            match=rx_skillName.match(it.key());
            if(match.hasMatch()){
                tmpName = it.value().toString();
                qDebug() << tmpName;
                tmpCombox <<tmpName;
                //将技能记录插入到map中,其标记为 玩家或敌人 序号 技能名 ： 技能表达式
                skillDescMap.insert("e"+QString::number(i)+tmpName,tmpExpr);
                qDebug() <<"插入的名称是" <<"e"+QString::number(i)+tmpName <<tmpExpr;
            }
        }
        //遍历单个characterlist完成，将得到的所有skname存储
        skillComboxInfo.insert("e"+QString::number(i),tmpCombox);
        //清空
        tmpCombox.clear();
    }
    loadCombox();

}

void MainWindow::loadCombox(){

    QStringList cList = skillComboxInfo.find("c"+QString::number(currentActCharacter)).value();
    QStringList eList = skillComboxInfo.find("e"+QString::number(currentActEnemy)).value();;
    //清空
    ui->comboBox_characterSpecial->clear();
    ui->comboBox_enemySpecial->clear();
    //更新双方的combox
    ui->comboBox_characterSpecial->addItems(cList);
    ui->comboBox_enemySpecial->addItems(eList);

}

//普通攻击通用方法
int MainWindow::attackGeneralMethod(QString expression){

    //正则表达式
    QRegularExpression rx_getCoef("\\b\\d+"); //从开头
    QRegularExpression rx_getDiceType("d\\d+"); // mid1
    QRegularExpression rx_getConst("\\+\\d\\b"); //mid1
    QRegularExpressionMatch tmpResult;

    //定义伤害
    int damage=0;
    int coef=0,diceType=6,minDamage=0;

    //获取值
    tmpResult=rx_getCoef.match(expression);
    if(tmpResult.hasMatch()){
        coef = tmpResult.captured(0).toInt();
        qDebug()<<"骰子系数"<<tmpResult.captured(0);
    }
    tmpResult=rx_getDiceType.match(expression);
    if(tmpResult.hasMatch()){
        diceType = tmpResult.captured(0).mid(1).toInt();
        qDebug()<<"骰子类型"<<diceType;
    }
    tmpResult=rx_getConst.match(expression);
    if(tmpResult.hasMatch()){
        minDamage = tmpResult.captured(0).mid(1).toInt();
        qDebug()<<"固定伤害"<<tmpResult.captured(0);
    }
    saySomething("打出了"+QString::number(coef)+"段攻击,");
    damage =Calculate().getRandomNumber(historyLineBuffer,diceType,coef)+minDamage;
    ui->diceNumberDisplayer->setText(QString::number(damage));
    ui->diceNumberDisplayer->setAlignment(Qt::AlignHCenter); //对齐


    saySomething("造成了"+QString::number(damage)+"的伤害",true);
    return damage;
}


//特殊攻击
void MainWindow::on_pushButton_cSpecialAttack_clicked()
{
    qDebug() << "c"+QString::number(currentActCharacter)+ui->comboBox_characterSpecial->currentText();
    //查询后获得表达式
    QString atkExprField = skillDescMap.find("c"+QString::number(currentActCharacter)+ui->comboBox_characterSpecial->currentText()).value();

    saySomething(ui->label_charcterName->text());
    onDamageTaken_enemy(attackGeneralMethod(atkExprField),false);

    updateInfoBoard(enemyList,currentActEnemy,ui->tableWidget_enemyInfo);

    currentActionTimes++;
    updateLabelActionTimes(currentActionTimes);
}

void MainWindow::on_pushButton_eSpecialAttack_clicked()
{
    QString atkExprField = skillDescMap.find("e"+QString::number(currentActEnemy)+ui->comboBox_enemySpecial->currentText()).value();

    saySomething(ui->label_enemy->text());
    onDamageTaken_character(attackGeneralMethod(atkExprField),false);
    updateInfoBoard(characterList,currentActCharacter,ui->tableWidget_characterInfo);


}

//普通攻击
void MainWindow::on_pushButton_attack_clicked()
{
    QString atkExprField ;

    //读取list中有关攻击的字段 就是attack
    if(characterList[currentActCharacter].property.contains("attack")){
        atkExprField = characterList[currentActCharacter].property.find("attack").value().toString();
        saySomething(ui->label_charcterName->text());
        onDamageTaken_enemy(attackGeneralMethod(atkExprField),false);
    }

    updateInfoBoard(enemyList,currentActEnemy,ui->tableWidget_enemyInfo);
    currentActionTimes++;
    updateLabelActionTimes(currentActionTimes);

}
void MainWindow::on_pushButton_enemyAttack_clicked()
{
    QString atkExprField ;

    //读取list中有关攻击的字段 就是attack
    if(enemyList[currentActEnemy].property.contains("attack")){
        atkExprField = enemyList[currentActEnemy].property.find("attack").value().toString();
        saySomething(ui->label_enemy->text());
        onDamageTaken_character(attackGeneralMethod(atkExprField),false);
    }

    updateInfoBoard(characterList,currentActCharacter,ui->tableWidget_characterInfo);


}

//伤害造成 数据交互
void MainWindow:: onDamageTaken_enemy(int damage,bool all){

    QMap<QString,QVariant>::iterator it;
    if(all == true){
        for(int i=0;i<enemyList.length();i++)
        {
            if(enemyList[i].property.contains("hp")){
                it = enemyList[i].property.find("hp");
                it.value()=it.value().toInt()-damage;
            }
        }
        saySomething("全体敌人受到了"+QString::number(damage)+" 点伤害",true);
    }
    else{
        if(enemyList[currentActEnemy].property.contains("hp")){
            it = enemyList[currentActEnemy].property.find("hp");
            it.value()=it.value().toInt()-damage;
            qDebug() << it.value();
            saySomething(ui->label_enemy->text()+QString::number(damage)+" 点伤害",true);
        }
    }
}
void MainWindow:: onDamageTaken_character(int damage,bool all){

    QMap<QString,QVariant>::iterator it;
    if(all == true){
        for(int i=0;i<characterList.length();i++)
        {
            if(characterList[i].property.contains("hp")){
                it = enemyList[i].property.find("hp");
                it.value()=it.value().toInt()-damage;
            }
        }
        saySomething("全体玩家受到了"+QString::number(damage)+" 点伤害",true);
    }
    else{
        if(characterList[currentActCharacter].property.contains("hp")){
            it = characterList[currentActCharacter].property.find("hp");
            it.value()=it.value().toInt()-damage;
            qDebug() << it.value();
            saySomething(ui->label_charcterName->text()+QString::number(damage)+" 点伤害",true);
        }
    }
}

//特殊攻击
void MainWindow::on_comboBox_characterSpecial_activated(const QString &arg1)
{
    //加载的时候就要默认是0号元素 如果元素空的话要禁用这一行函数
}

#pragma endregion BattleArea



/////////////////////////Protect My Dog Eye///////////////////////////////
///////////////////////// FileIO ////////////////////////////////
// 文件流区
// group_history
#pragma region FileIO


void MainWindow::on_action_loadFile_triggered()
{
    // memset(&characterList,0,sizeof(Character)*characterList.length());
    //为防止重复加载写的覆盖 有很大问题 以后有时间重写了一定要改一下这一点
    QList<Character> newCharacterList;
    QList<Character> newEnemyList;
    characterList = newCharacterList;
    enemyList = newEnemyList;

    playFilePath = QFileDialog::getOpenFileName(this,"选择剧本文件","","所有文件(*.*) ;; XML文件 (*.xml)");
    ui->textBrowser_history->clear();

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
    updateLabelActionTimes(0);
    updateInfoBoard(characterList,currentActCharacter,ui->tableWidget_characterInfo);

    updateLabelBoard();
    loadCombox();

    saySomething("现在是"+QString::number(currentActCharacter)+"号角色"+":"+ui->label_charcterName->text()+"行动",true);
}


void MainWindow::on_pushButton_toPreCharacter_clicked()
{

    if(characterList.length()==0) return;
    currentActCharacter = (currentActCharacter+characterList.length()-1) % characterList.length();
    qDebug()<<currentActCharacter;
    updateLabelActionTimes(0);

     updateInfoBoard(enemyList,currentActEnemy,ui->tableWidget_enemyInfo);
    updateLabelBoard();
    loadCombox();

    saySomething("现在是"+QString::number(currentActCharacter)+"号角色"+":"+ui->label_charcterName->text()+"行动",true);
}

void MainWindow::on_pushButton_toRandomCharacter_clicked()
{

    if(characterList.length()==0) return;
    currentActCharacter = qrand() % characterList.length();
    qDebug() << currentActCharacter;

    updateLabelActionTimes(0);
    updateInfoBoard(characterList,currentActCharacter,ui->tableWidget_characterInfo);

    updateLabelBoard();
    loadCombox();

    saySomething("现在是"+QString::number(currentActCharacter)+"号角色"+":"+ui->label_charcterName->text()+"行动",true);
}

//敌人选择和跳转

void MainWindow::on_pushButton_toPreEnemy_clicked()
{
    if(enemyList.length()==0) return;
    currentActEnemy = (++currentActEnemy) % enemyList.length();
    qDebug()<<currentActEnemy;

     updateInfoBoard(enemyList,currentActEnemy,ui->tableWidget_enemyInfo);
    updateLabelBoard();
    loadCombox();

}

void MainWindow::on_pushButton_toNextEnemy_clicked()
{
    if(enemyList.length()==0) return;
    currentActEnemy = (++currentActEnemy) % enemyList.length();
    qDebug()<<currentActEnemy;

     updateInfoBoard(enemyList,currentActEnemy,ui->tableWidget_enemyInfo);
    updateLabelBoard();
    loadCombox();
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

//修改主角table
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

//修改敌人table
void MainWindow::on_pushButton_addTableItemEnemy_clicked()
{
    //获得输入元素
    qDebug()<< "??";
    bool inputDone;
    QString item = QInputDialog::getText(this,"输入新的配对","格式如 键;值",QLineEdit::Normal,"",&inputDone);
    if(!inputDone) return;
    QStringList inputBuffer=item.split(";");
    qDebug()<<inputBuffer[0] << "  " << inputBuffer[1];

    //更新table
    int row = ui->tableWidget_enemyInfo->rowCount();
    ui->tableWidget_enemyInfo->insertRow(row);
    ui->tableWidget_enemyInfo->setVerticalHeaderItem(row,new QTableWidgetItem(inputBuffer[0]));
    ui->tableWidget_enemyInfo->setItem(row,0,new QTableWidgetItem(inputBuffer[1]));

    qDebug()<<"enemytable更新完成";

    //更新list
    enemyList[currentActEnemy].property.insert(inputBuffer[0],inputBuffer[1]);
    qDebug()<<"enemylist更新完成";
}



void MainWindow::on_tableWidget_enemyInfo_itemClicked(QTableWidgetItem *item)
{
    currentSelectTableRow =  item->row();
    if(characterList.length()!=0){
        ui->pushButton_deleteEnemyItem->setEnabled(true);
    }
}


void MainWindow::updateInfoBoard(QList<Character>& list,int index,QTableWidget *table){
    table->clear();


    if(list.length()==0){
        qDebug() << "敌对单位信息列表写入失败";
        return;
    }

    //遍历property写入
    QMap<QString,QVariant> propertyBuffer = list[index].property;
    QMap<QString,QVariant>::Iterator it = propertyBuffer.begin(),itend = propertyBuffer.end();


    for (int i=0;it != itend; it++,i++){
        table->setVerticalHeaderItem(
                    i,new QTableWidgetItem(it.key()));

        table->setItem(
                    i,0,new QTableWidgetItem(it.value().toString()));

        // qDebug() << it.key();
        // qDebug() << it.value();
    }

    table->setHorizontalHeaderItem(0,new QTableWidgetItem("value"+QString::number(currentActEnemy)));



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
//反向更新敌方单位信息的table
void MainWindow::update_enemyTableItemDelete(QString key){
    QMap<QString, QVariant>::iterator it;

    qDebug() << enemyList[currentActEnemy].property.contains(key);
    if(enemyList[currentActEnemy].property.contains(key))
    {
        it = enemyList[currentActEnemy].property.find(key);
        qDebug() <<it.key() << it.value().toString()<< "删除成功！";
        enemyList[currentActEnemy].property.erase(it);

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
    if(enemyList[currentActEnemy].property.contains("name"))
    {
        it = enemyList[currentActEnemy].property.find("name");
        ui->label_enemy->setText(it.value().toString());
    }
    else
    {
        ui->label_enemy->setText("");
    }


}



void MainWindow::setPlayFileXML(){

    QVector<QString> headerBuffer;
    QVector<QVariant> valueBuffer;
    QVector<QString> eHeaderBuffer;
    QVector<QVariant> eValueBuffer;

    QFile playFile(playFilePath);

    if (!playFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug()<<"Error: cannot open file";
        return;
    }

    int characterIndex=0; //设定最初的玩家编号为0
    int enemyIndex = 0;
    QString characterSectionName;
    Character characterBuffer;
    Character enemyBuffer;
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
        if((playFileReader.name()=="character"||playFileReader.name()=="header") && streamAttr.hasAttribute("id")){
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

        ////////////////////////////敌人table部分/////////////////////////////// else if 还没断
        //读到enemy的id
        else if((playFileReader.name()=="enemy"|| playFileReader.name()  == "eheader") && streamAttr.hasAttribute("id")){
            enemyIndex = streamAttr.value("id").toInt();
            qDebug() << "读取到敌方id信息";
        }
        //结束enemy的e属性区域的读取 上载
        else if(playFileReader.isEndElement() && (playFileReader.name()  == "enemy")){
            //根据位置配对插入
            for(int i =0;i<eHeaderBuffer.length();i++){
                enemyBuffer.property.insert(eHeaderBuffer[i],eValueBuffer[i]);
            }

            enemyBuffer.index = enemyIndex;
            enemyList.append(enemyBuffer);

            //把各个具体的人的buffer清空
            eHeaderBuffer.clear();
            eValueBuffer.clear();
            enemyBuffer.property.clear();
            enemyBuffer.records.clear();
        }
        //读到档案记录
        else if(playFileReader.name() == "hint"){
            enemyBuffer.records.append( playFileReader.readElementText());
            qDebug() <<  "record写入" << enemyBuffer.records.last();
            qDebug() <<  "record确认" << enemyBuffer.records.length();
        }
        //写入数据 header
        else if(playFileReader.name() == "eh"){
            eHeaderBuffer.append(playFileReader.readElementText());
            qDebug() <<  "信息写入" << eHeaderBuffer.last();
        }
        //写入数据 character value
        else if(playFileReader.name() == "ec"){
            eValueBuffer.append(playFileReader.readElementText());
            qDebug() <<  "信息写入" << eValueBuffer.last();
        }

        //写入历史记录
        else if(playFileReader.name()=="history"){
            totalPlayTimes = playFileReader.attributes().value("times").toString();
            historyInfoBuffer.append(playFileReader.readElementText());
            historyAll.append(historyInfoBuffer.join("|"));
            qDebug() << "读取历史游戏记录" ;
        }
    }


    playFile.close();

    //发个更新信号呗
    emit updateInfoBoard(characterList,currentActCharacter,ui->tableWidget_characterInfo);
    emit updateInfoBoard(enemyList,currentActEnemy,ui->tableWidget_enemyInfo);
    emit updateLabelBoard();
    emit loadBattleDetail();
    emit setSwitchEnemySlider();
    emit updateLastPlayHistoryBoard();

    //历史游戏记录次数+1
    totalPlayTimes = QString::number(totalPlayTimes.toInt()+1);

    //启用增加条目按钮
    ui->pushButton_addTableItem->setEnabled(true);
    ui->tableWidget_characterInfo->verticalHeader()->setVisible(true);


}

void MainWindow::savePlayFileXML(){
    if(playFilePath == nullptr){
        playFilePath = "newPlayFile.xml";
    }


    QFile playFile(playFilePath);
    playFile.resize(0);

    historyAll.append(historyInfoBuffer.join("|")); //将这次游玩的历史记录全部压缩成一个stringapped到这个list里


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

        //写入角色header表
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
            writer.writeTextElement("record",characterList[n].records.at(ir));
            qDebug() << "写入" << characterList[n].records.at(ir);
        }
        writer.writeEndElement(); //character

        qDebug() << n <<"号玩家保存完毕";

    }

    for(int n=0;n<enemyList.length();n++){

        qDebug() << "共有" << enemyList.length() << "个敌人单位信息";
        qDebug() << "正在保存" << n << "号敌人";

        //写入敌方单位eheader表
        writer.writeStartElement("eheader");

        QMap<QString,QVariant>::Iterator iteh;
        writer.writeAttribute("id",QString::number(n));
        for(iteh=enemyList[n].property.begin();iteh!=enemyList[n].property.end();iteh++){
            writer.writeTextElement("eh",iteh.key());
            qDebug() << "写入" << iteh.key();
        }

        writer.writeEndElement();

        //写入enemy表
        writer.writeStartElement("enemy");

        //写入属性
        QMap<QString,QVariant>::Iterator itec;
        writer.writeAttribute("id",QString::number(n));
        for(itec=enemyList[n].property.begin();itec!=enemyList[n].property.end();itec++){
            writer.writeTextElement("ec",itec.value().toString());
            qDebug() << "写入" << itec.value().toString();
        }


        //写入hints
        for(int ih=0;ih<enemyList[n].records.length();ih++){
            writer.writeTextElement("hint",enemyList[n].records.at(ih));
            qDebug() << "写入" << enemyList[n].records.at(ih);
        }
        writer.writeEndElement();//ec
        qDebug() << n <<"号敌人保存完毕";


    }

    //保存历史记录 每一次的游玩是一个长string
    for(int i =0 ; i< historyAll.length();i++){
        writer.writeStartElement("history");
        writer.writeAttribute("times",QString::number(i));
        writer.writeCharacters("这是第"+QString::number(i)+"次游玩记录|");
        writer.writeCharacters(historyAll.at(i));
        writer.writeCharacters("|");
        writer.writeEndElement(); //history
    }

    historyAll.clear();

    qDebug() << "保存成功！";
    writer.writeEndElement(); //root
    writer.writeEndDocument();
    playFile.close();

    ui->textBrowser_history->append("大概保存成功了！");

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

void MainWindow::updateLastPlayHistoryBoard(){

     //是一个list 先join 然后再|分割 然后再逐个输出

    historyInfoBuffer = historyInfoBuffer.join("|").split("|");
    for(int i=0;i<historyInfoBuffer.length();i++){
        ui->textBrowser_history->append(historyInfoBuffer.at(i));
    }

    //清空
    historyInfoBuffer.clear();

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
    saySomething(ui->label_charcterName->text()+"掷出了");

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












void MainWindow::update_changeTableItem(int row ,int col,QList<Character>& list,int index,QTableWidget* table){
    if(row > table->rowCount() || row >= list[index].property.size()){
        qDebug() <<"企图修改不存在的数据";
        return;
    }
    //根据table里的内容，反向传递至list中
    //获得内容
    QVariant newContent = table->item(row,col)->text();
    //找到表头
    QString headerName = table->verticalHeaderItem(row)->text();


    //查找list
    list[index].property.find(headerName).value() = newContent;

    qDebug() << "修改成功 新内容："<< newContent;
    loadBattleDetail();

}


void MainWindow::on_tableWidget_characterInfo_cellChanged(int row, int column)
{
    update_changeTableItem(row,column,characterList,currentActCharacter,ui->tableWidget_characterInfo);
}

void MainWindow::on_tableWidget_enemyInfo_cellChanged(int row, int column)
{
    qDebug() << row;
     update_changeTableItem(row,column,enemyList,currentActEnemy,ui->tableWidget_enemyInfo);
}
