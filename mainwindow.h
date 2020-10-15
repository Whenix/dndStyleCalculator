#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QTableWidgetItem>
#include <QMetaType>
#include <QRegExpValidator>
#include <QRegularExpression>
#include <QFileDialog>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

//角色构造
struct Character{
    int index;
    QMap<QString,QVariant> property;
    QStringList records;

};
struct Skill{
    int index;
    QString skillName;
    QVariant skillExpression;
};


Q_DECLARE_METATYPE(Character)
Q_DECLARE_METATYPE(Skill)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //历史消息
    QStringList historyInfoBuffer;
    QString historyLineBuffer;
    QList<Character> characterList;
    QList<Character> enemyList;
    int currentActCharacter;
    int currentActEnemy;
    int currentActionTimes;
    QMap<QString,QString> skillDescMap;
    QMap<QString,QStringList> skillComboxInfo;

    MainWindow(QWidget *parent = nullptr);


    ~MainWindow();


private slots:
    //战斗！
    int attackGeneralMethod(QString expression);
    void onDamageTaken_character(int,bool);
    void onDamageTaken_enemy(int,bool);
    void onDebuffTaken_character(){};
    void onDebuffTaken_enemy(){};

    //信息载入
    void setPlayFileXML();
    void savePlayFileXML();

    void loadBattleDetail();
    void loadCombox();
    void setSwitchEnemySlider();


    //更新记录面板
    void updateHistoryBoard(QString &infoBuffer);
   // void updateCharacterBoard(int index);
  //  void updateEnemyBoard(int index);
    void updateInfoBoard(QList<Character>&,int,QTableWidget*);
    void updateLabelBoard();
    void updateLabelActionTimes(int);


    //面板反向更新至数组中
    void update_tableItemDelete(QString key);
    void update_enemyTableItemDelete(QString key);

    //骰子投掷区
    void on_anyJudgePushButton_clicked(int threshold,bool needJudgement);
    void on_pushButton_easy_clicked();
    void on_pushButton_hard_clicked();
    void on_pushButton_impossible_clicked();
    void on_pushButton_zero_clicked();
    void on_pushButton_general_clicked();
    void on_pushButton_veryEasy_clicked();
    void on_pushButton_getNumber_clicked();

    void on_exactSlider_valueChanged();
    void on_pushButton_addDThrow_clicked();
    void on_pushButton_subtractThrow_clicked();
    void on_checkBox_exactSwitch_stateChanged(int arg1);
    void on_pushButton_timesReset_clicked();

    //骰子参数区
    void on_radioButton_d2_clicked();
    void on_radioButton_d6_clicked();
    void on_radioButton_d12_clicked();
    void on_radioButton_d16_clicked();
    void on_radioButton_d20_clicked();
    void on_radioButton_d28_clicked();
    void on_radioButton_d34_clicked();
    void on_radioButton_d100_clicked();
    void on_lineEdit_customDiceSides_textChanged(const QString &arg1);

    //菜单按钮动作区
    void on_action_loadFile_triggered();
    void on_action_saveFile_triggered();


    void on_checkBox_noRecordHistory_clicked();

    void on_pushButton_toNextCharacter_clicked();

    void on_pushButton_toPreCharacter_clicked();

    void on_pushButton_showCharacterRecords_clicked();

    void on_pushButton_addCharacterRecord_clicked();


    void on_pushButton_deleteTableHeader_clicked();

    void on_tableWidget_characterInfo_itemClicked(QTableWidgetItem *item);



    void on_pushButton_toRandomCharacter_clicked();

    void on_pushButton_addTableItem_clicked();

    //战斗区域
    void on_pushButton_attack_clicked();
    void on_comboBox_characterSpecial_activated(const QString &arg1);

    void on_pushButton_addTableItemEnemy_clicked();



    void on_tableWidget_enemyInfo_itemClicked(QTableWidgetItem *item);

    void on_pushButton_enemyAttack_clicked();


    void on_pushButton_toPreEnemy_clicked();

    void on_pushButton_toNextEnemy_clicked();


    void on_pushButton_cSpecialAttack_clicked();

    void on_pushButton_eSpecialAttack_clicked();


    //敌对单位面板信息区
    void on_pushButton_addEnemy_clicked();
    void on_horizontalSlider_quickSelect_valueChanged(int value);
    void on_pushButton_addEnemyItem_clicked();

    void on_pushButton_duplicateEnemy_clicked();

    void on_pushButton_product_clicked();
    void on_pushButton_destroyEnemy_clicked();
    void on_pushButton_deleteEnemyItem_clicked();

private:
    Ui::MainWindow *ui;
    int diceThrowTimes = 1;
    int successThreshold;
    int diceSides;
    QString playFilePath;
    QStringList playFileDataTemp;
    QIntValidator *validatorInt;
    int currentSelectTableRow;



    void saySomething(QString words,bool update=false);

signals:
    void historyUpdate(QString &infoBuffer);

};
#endif // MAINWINDOW_H
