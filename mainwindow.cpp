#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calculate.h"
#include <qinputdialog.h>
#include "QDebug"

// 检测值和阈值得出大失败 失败 成功 大成功的功能
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit_cunstomExprThreshold->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->lineEdit_timeSpeed->setValidator(new QRegExpValidator(QRegExp("[0-9]*[.][0-9]*")));
    ui->lineEdit_customExprInput->setValidator(new QRegExpValidator(QRegExp("([\\+\\-dt\\~]|[0-9])+$")));
    // connect(this,SIGNAL(historyUpdate(QString)),ui->textBrowser_history,SLOT(updateHistoryBoard()));

    //绑定：点击玩家信息表 -> 自定义公式区域中的信息更新
    connect(ui->tableWidget_characterInfo,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(updateLabelCustomExprInfo(QTableWidgetItem*)));
    //绑定：骰子数字更新后 -> 获得判定结果
    connect(ui->diceNumberDisplayer,SIGNAL(textChanged()),this,SLOT(updateLabelJuggInfo()));


    diceThrowTimes = 1;
    customThreshold = 50;
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


/////////////////////////Protect My Dog Eye///////////////////////////////

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
    needCheck = false;
    displayDiceNumber(damage);



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



/////////////////////////Protect My Dog Eye///////////////////////////////

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
    on_MainWindow_timerInitialize();

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

//跳转到上一个敌人
void MainWindow::on_pushButton_toPreEnemy_clicked()
{
    if(enemyList.length()==0) return;
    currentActEnemy = (++currentActEnemy) % enemyList.length();
    qDebug()<<currentActEnemy;

    updateInfoBoard(enemyList,currentActEnemy,ui->tableWidget_enemyInfo);
    updateLabelBoard();
    loadCombox();

}

//跳转到下一个敌人
void MainWindow::on_pushButton_toNextEnemy_clicked()
{
    if(enemyList.length()==0) return;
    currentActEnemy = (++currentActEnemy) % enemyList.length();
    qDebug()<<currentActEnemy;

    updateInfoBoard(enemyList,currentActEnemy,ui->tableWidget_enemyInfo);
    updateLabelBoard();
    loadCombox();
}


//显示角色的记录
void MainWindow::on_pushButton_showCharacterRecords_clicked()
{
    qDebug() <<characterList[currentActCharacter].records.length();
    for(int i=0;i<characterList[currentActCharacter].records.length();i++)
    {   qDebug() <<characterList[currentActCharacter].records[i];
        saySomething(characterList[currentActCharacter].records[i],true);
    }

}

//增加角色的一条记录
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

//按钮 增加新条目
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

//点击玩家信息表
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

        qDebug() << "单位信息列表写入失败";
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

//读取游戏
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

    QXmlStreamReader playFileReader;
    QXmlStreamAttributes streamAttr;
    playFileReader.setDevice(&playFile);
    qDebug() <<"读取的文件路径："<< playFilePath ;

    while(!playFileReader.atEnd()){

        playFileReader.readNext();

        streamAttr = playFileReader.attributes();



        ////////////////玩家table部分///////////////////////////////////////////////
        //当这一小节存在ID值时，可认为要写入的玩家的档案序号为该值
        if((playFileReader.name()=="character"||playFileReader.name()=="header") && streamAttr.hasAttribute("id")){
            characterIndex = streamAttr.value("id").toInt();
            qDebug() << "读取到id信息";
        }
        else if(playFileReader.isEndElement() && playFileReader.name()  == "character"){
            qDebug() << "尝试将读取内容写入到玩家table中";
            //根据位置配对插入
            for(int i =0;i<headerBuffer.length();i++){
                characterBuffer.property.insert(headerBuffer[i],valueBuffer[i]);
            }

            characterBuffer.index = characterIndex;
            characterList.append(characterBuffer);

            //把各个具体的人的buffer清空
            headerBuffer.clear();
            valueBuffer.clear();
            characterBuffer.property.clear();
            characterBuffer.records.clear();

            qDebug() << "已经成功读取了一名玩家的所有档案";
        }
        //读到档案记录
        else if(playFileReader.name() == "record"){
            characterBuffer.records.append( playFileReader.readElementText());
            qDebug() << "成功读取了玩家的记录";

        }
        //写入数据 header
        else if(playFileReader.name() == "h"){
            headerBuffer.append(playFileReader.readElementText());

        }
        //写入数据 character value
        else if(playFileReader.name() == "c"){
            valueBuffer.append(playFileReader.readElementText());

        }

        ////////////////////////////敌人table部分/////////////////////////////// else if 还没断
        //读到enemy的id
        else if((playFileReader.name()=="enemy"|| playFileReader.name()  == "eheader") && streamAttr.hasAttribute("id")){
            enemyIndex = streamAttr.value("id").toInt();

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
        }
        //写入数据  value
        else if(playFileReader.name() == "ec"){
            eValueBuffer.append(playFileReader.readElementText());
        }

        //写入历史记录
        else if(playFileReader.name()=="history"){
            totalPlayTimes = playFileReader.attributes().value("times").toString();
            historyInfoBuffer.append(playFileReader.readElementText());
            historyAll.append(historyInfoBuffer.join("|"));
        }

        //载入游戏设置 时间
        else if (playFileReader.name() == "timeInfo"){
            QStringList timeInfo = playFileReader.readElementText().split(",");

            if(timeInfo.length()<3){
                qDebug() << timeInfo.length();
                qDebug() << "剧本时间格式设置错误";return;
            }
            this->passedDays = timeInfo.at(0).toInt();
            this->passedSeconds=timeInfo.at(1).toDouble();
            this->timeSpeed = timeInfo.at(2).toInt();
        }

        //载入游戏设置 快捷检定公式
        else if(playFileReader.name() == "expression"){
            expressionQSList.append(playFileReader.readElementText());
            qDebug() << "读取到了" << QString::number( expressionQSList.length()) << "条公式";
        }
    }


    playFile.close();

    //发个更新信号呗
    emit updateInfoBoard(characterList,currentActCharacter,ui->tableWidget_characterInfo);
    emit updateInfoBoard(enemyList,currentActEnemy,ui->tableWidget_enemyInfo);

    emit updateLabelBoard();
    emit updateExprQSBoard();
    emit loadBattleDetail();
    emit setSwitchEnemySlider();

    emit updateTimeInfo();
    emit updateLastPlayHistoryBoard();


    //历史游戏记录次数+1
    totalPlayTimes = QString::number(totalPlayTimes.toInt()+1);

    //启用增加条目按钮
    ui->pushButton_addTableItem->setEnabled(true);
    ui->tableWidget_characterInfo->verticalHeader()->setVisible(true);

}

//保存游戏
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

    //保存游戏设置
    writer.writeStartElement("settings");
    //保存时间
    writer.writeTextElement("timeInfo", QString::number(passedDays)+","+QString::number(passedSeconds)+","+QString::number(timeSpeed));

    //保存公式快速选择列表
    for(int i=0;i<expressionQSList.length();i++){
        writer.writeTextElement("expression",expressionQSList.at(i));
    }

    writer.writeEndElement(); //settings


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

        writer.writeEndElement(); //header

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

    //保存敌方单位信息
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
        if(historyAll.at(i) != "\n"||historyAll.at(i) != "" ){
            writer.writeCharacters(historyAll.at(i));
            writer.writeCharacters("|");
        }
        writer.writeEndElement(); //history
    }

    historyAll.clear();

    qDebug() << "保存成功！";
    writer.writeEndElement(); //root
    writer.writeEndDocument();
    playFile.close();

    ui->textBrowser_history->append("大概保存成功了！");

}

//更新快速选择公式列表的信息
void MainWindow::updateExprQSBoard(){

    if(expressionQSList.length()==0){
        qDebug() << "快速公式选择列表加载失败";
    }

    QStringList buffer;
    //设置列表行数
    ui->tableWidget_checkTable->setRowCount(expressionQSList.length());

    //遍历整个list
    for(int i =0;i<expressionQSList.length();i++){
        //对于list中的单个元素，再将其拆分
        buffer =expressionQSList.at(i).split(";");

        ui->tableWidget_checkTable->setItem(i,0,new QTableWidgetItem(buffer.at(0))); //公式描述
        ui->tableWidget_checkTable->setItem(i,1,new QTableWidgetItem(buffer.at(1))); //目标属性
        ui->tableWidget_checkTable->setItem(i,2,new QTableWidgetItem(buffer.at(2))); //表达式
        ui->tableWidget_checkTable->setItem(i,3,new QTableWidgetItem(buffer.at(3))); //阈值
    }

}

//处理未翻译的含t公式的方法
QString MainWindow::exprCustomNormalizMethod(int row){

    //获得未翻译的含t表达式
    QString originExpr = ui->tableWidget_checkTable->item(row,2)->text();

    if(!originExpr.contains("t")){
        qDebug() << "这不含t";
        return originExpr;
    }

    //定义正则表达式
    QRegularExpression rx_tReplace;

    //获得目标属性的切分的列表，比如{力量,智力,敏捷}={力量},{智力},{敏捷}
    QStringList attrStringList = ui->tableWidget_checkTable->item(row,1)->text().split(",");

    //得到t值的个数，比如在上述举例中只可能有t1,t2,t3 而不可能有t4
    int variableNum =attrStringList.length();

    //tn代表的值
    QString value;

    //以不同的正则表达式多次遍历相同的string，分别找到原值并替换
    for(int i =0;i<variableNum;i++){
        if(variableNum == 1){
            rx_tReplace.setPattern("t");
        }
        else{
            rx_tReplace.setPattern("t"+QString::number(i+1));
        }

        //根据键名在当前玩家的characterlist中寻找相应的值替换，找不到就归0
        if( characterList[currentActCharacter].property.contains(attrStringList.at(i)) ){
            value =  characterList[currentActCharacter].property.find(attrStringList.at(i)).value().toString();
        }
        else{
            value = "0";
        }

        originExpr.replace(rx_tReplace,value);
    }
    return originExpr;
}

//更新历史记录面板
void MainWindow::saySomething(QString words,bool updateImmediately){

    historyLineBuffer += words;

    if(updateImmediately){
        emit updateHistoryBoard(historyLineBuffer);
    }
}

//更新历史记录面板
void MainWindow:: updateHistoryBoard(QString &infoBuffer){
    historyInfoBuffer.append(infoBuffer);
    ui->textBrowser_history->append(infoBuffer);

    //清空
    infoBuffer.clear();
}

//加载上次游玩的历史信息
void MainWindow::updateLastPlayHistoryBoard(){

    //是一个list 先join 然后再|分割 然后再逐个输出
    historyInfoBuffer = historyInfoBuffer.join("|").split("|");
    for(int i=0;i<historyInfoBuffer.length();i++){
        ui->textBrowser_history->append(historyInfoBuffer.at(i));
    }

    //清空
    historyInfoBuffer.clear();
}

//更新判定面板的信息
void MainWindow::updateLabelJuggInfo(){
    if(!needCheck) {
        needCheck = true;
        return;
    }
    int number = ui->diceNumberDisplayer->toPlainText().toInt();
    int result = Calculate().judgeDiceResult(number,customThreshold);
    switch (result) {
    case -1:{
        ui->label_judgeInfo->setText("大失败!");
        ui->label_judgeInfo->setStyleSheet("color:red;");
        break;
    }
    case 0:{
        ui->label_judgeInfo->setText("失败!");
        ui->label_judgeInfo->setStyleSheet("color:red;");
        break;}
    case 1:{
        ui->label_judgeInfo->setText("成功!");
        ui->label_judgeInfo->setStyleSheet("color:green;");
        break;}
    case 2:{
        ui->label_judgeInfo->setText("大成功!");
        ui->label_judgeInfo->setStyleSheet("color:green;");
        break;}
    }

}

//修改玩家角色信息
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

//当玩家信息被修改时
void MainWindow::on_tableWidget_characterInfo_cellChanged(int row, int column)
{
    update_changeTableItem(row,column,characterList,currentActCharacter,ui->tableWidget_characterInfo);
}

//修改敌人信息
void MainWindow::on_tableWidget_enemyInfo_cellChanged(int row, int column)
{
    update_changeTableItem(row,column,enemyList,currentActEnemy,ui->tableWidget_enemyInfo);
}

//Expression解读通用方法
int MainWindow::exprDecodingGeneralMethod(QString expression){
    /*  注意：
     *      如表达式 1d3-1d5+c 可以写为 1d3+1d5+(c-6) c为常数
     *      因此在设计公式时请不要在骰子间写“-”号
     *      即减去 减法得到的最小值 和 加法得到的最小值 的差值 (1-5与1+1的差值)
     *      一般写法： [x1]d[t1]+[x2]d[t2]+...+[xn]d[tn]+const，其中const为常数,常数项可以为负
     */

    //定义正则表达式
    QRegularExpression rx_getCoef("\\b\\d+d"); //从开头
    QRegularExpression rx_getDiceType("d\\d+"); // mid1
    QRegularExpression rx_getConst("(\\-|\\b)\\d+\\b"); //mid const的表现形式为 +或- 数字 边界
    QRegularExpressionMatch tmpResult;

    //定义伤害
    int damage=0;
    int noLimit=0;
    int coef,diceType,minDamage,diceTimes=0;

    //检查字符d的出现个数
    for(int i=0;i<expression.length();i++){
        if(expression.at(i)=="d"){
            diceTimes +=1;
        }
    }

    //检查下限是否为0
    if(expression.at(0)=="~"){
        expression.remove("~");
        noLimit = 1;
    }

    //切割字符串为QStringlist
    QStringList exprList = expression.split("+");
    QStringList::iterator it;

    //遍历整个list获得单个表达式的值
    for(it=exprList.begin();it!=exprList.end();it++){

        //每次循环都要清空
        coef=0;diceType=0;minDamage=0;

        tmpResult=rx_getCoef.match(*it);
        if(tmpResult.hasMatch()){
            coef = tmpResult.captured(0).remove("d").toInt();
            qDebug()<<"骰子系数"<<tmpResult.captured(0);
        }
        tmpResult=rx_getDiceType.match(*it);
        if(tmpResult.hasMatch()){
            diceType = tmpResult.captured(0).mid(1).toInt();
            qDebug()<<"骰子类型"<<diceType;
        }
        tmpResult=rx_getConst.match(*it);
        if(tmpResult.hasMatch()){
            minDamage = tmpResult.captured(0).remove("+").toInt();
            qDebug()<<"固定伤害"<<tmpResult.captured(0);
        }
        if(diceType != 0 ){
            damage +=Calculate().getRandomNumber(historyLineBuffer,diceType,coef,noLimit)+minDamage;
        }
        else{
            damage += minDamage;
        }
        qDebug() <<"伤害更新为" <<damage;
    }
    return damage;
}

//数值显示器根据值显示数值
void MainWindow::displayDiceNumber(int value){
    ui->diceNumberDisplayer->setText(QString::number(value));
    ui->diceNumberDisplayer->setAlignment(Qt::AlignHCenter); //对齐
}


//修改自定义公式
void MainWindow::on_lineEdit_customExprInput_textChanged(const QString &arg)
{
    customExpression = arg;
}

//根据选中的列表中的属性自动更新显示信息
void MainWindow::updateLabelCustomExprInfo(QTableWidgetItem *item){

    if(item == nullptr){
        qDebug() << "当前没有选中任何table中的物件";
        return;
    }

    //获得表的指向
    QTableWidget *table=item->tableWidget();

    //更新属性名信息
    ui->label_customTargetAttrName->setText(table->verticalHeaderItem(item->row())->text());

    //更新属性值信息
    customCheckAttr = 0;
    customCheckAttr = item->text().toInt();
    ui->label_customTargetAttrValue->setText(QString::number(customCheckAttr));

}

//自定义公式执行
void MainWindow::on_pushButton_executeCustomExpr_clicked()
{
    int value = exprDecodingGeneralMethod(customExpression);
    displayDiceNumber(value);
    saySomething("自定义表达式结果："+QString::number(value),true);
}

//手动修改阈值
void MainWindow::on_lineEdit_cunstomExprThreshold_textChanged(const QString &arg)
{
    customThreshold = arg.toInt();
}

//todo
void MainWindow::on_toolButton_custom_help_triggered(QAction *arg1)
{

}

//todo
void MainWindow::on_toolButton_custom_help_clicked()
{

}

//Init - 时间列表
void MainWindow::on_MainWindow_timerInitialize(){
    ui->lcd_timeHMS->display("00:00:00");
    ui->lcd_timeDays->display(passedDays);
    ui->lineEdit_timeSpeed->setText(QString::number(timeSpeed));
    this->timer = new QTimer(this);
    this->timer->start(updateFreq); //更新频率

    connect(this->timer,SIGNAL(timeout()),this,SLOT(updateTimeInfo()));

    //时间流速管理设置
    connect(ui->lineEdit_timeSpeed,SIGNAL(returnPressed()),this,SLOT(updateHSliderTimeQS()));
    updateHSliderTimeQS();

}
void MainWindow:: updateHSliderTimeQS(){
    double maxValueQS =floor(timeSpeed*20.0f);
    double minValueQS =floor(timeSpeed*5.0f);
    ui->hSlider_timeSpeedQS->setMaximum(maxValueQS);
    ui->hSlider_timeSpeedQS->setMinimum(minValueQS);
    ui->hSlider_timeSpeedQS->setSingleStep((minValueQS+maxValueQS)/5);

}

//更新并显示时间信息
void MainWindow::updateTimeInfo(){
    //从开始时间到现在的时间经过了多少秒
    passedSeconds += updateFreq/1000.0f*timeSpeed*pauseSpeed;
    if(passedSeconds>86400){
        passedSeconds -= 86400.0f;
        passedDays+=1;
        ui->lcd_timeDays->display(passedDays);
    }
    this->ui->lcd_timeHMS->display(totalTime.addSecs(floor(passedSeconds)).toString("hh:mm:ss"));

}

//计时器刷新频率
void MainWindow::on_dial_timerFreqAdjust_sliderReleased()
{
    updateFreq = ui->dial_timerFreqAdjust->value();
    this->timer->stop();
    this->timer->start(updateFreq);
}

//滑动调节时间流速
void MainWindow::on_hSlider_timeSpeedQS_sliderReleased()
{
    timeSpeed = ui->hSlider_timeSpeedQS->value()/10.0f;

    //更新linedit
    ui->lineEdit_timeSpeed->setText(QString::number(timeSpeed,'f',1));
}

//lineEdit手动输入时间流速
void MainWindow::on_lineEdit_timeSpeed_textEdited(const QString &arg)
{
    timeSpeed = arg.toDouble();

    //更新slider
    ui->hSlider_timeSpeedQS->setValue(timeSpeed*10);
}

//计时器暂停
void MainWindow::on_pushButton_timerPause_clicked()
{
    pauseSpeed = 0.0f;
}

//计时器继续
void MainWindow::on_pushButton_timerGo_clicked()
{
    pauseSpeed = 1.0f;
}

//输入发言信息
void MainWindow::on_pushButton_saySomething_clicked()
{
    //获得输入元素
    bool inputDone;
    QString words = QInputDialog::getText(this,"输入发言记录","",QLineEdit::Normal,"",&inputDone);
    if(!inputDone) return;

    saySomething(words,true);
}



//当玩家点击快速选择公式表
void MainWindow::on_tableWidget_checkTable_cellClicked(int row)
{
    if(ui->tableWidget_checkTable->item(row,0) == NULL){
        return ;
    }
    //调用处理方法
    customExpression =  exprCustomNormalizMethod(row);

    int tmp = ui->tableWidget_checkTable->item(row,3)->text().toInt();
    if(tmp != -1){
        customThreshold = tmp;

    }
    ui->lineEdit_cunstomExprThreshold->setText(QString::number(customThreshold));
    ui->lineEdit_customExprInput->setText(customExpression);


}

void MainWindow::on_lineEdit_customExprInput_textEdited(const QString &arg1)
{

}

