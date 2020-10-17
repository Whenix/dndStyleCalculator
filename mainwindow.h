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
#include <QTime>
#include <QTimer>

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
    QStringList historyAll;
    QStringList historyInfoBuffer;
    QString historyLineBuffer;
    QList<Character> characterList;
    QList<Character> enemyList;
    int currentActCharacter;
    int currentActEnemy;
    int currentActionTimes;
    QMap<QString,QString> skillDescMap;
    QMap<QString,QStringList> skillComboxInfo;
    QStringList expressionQSList;
    QString customExpression;
    QStringList customExprQS;
    int diceThrowTimes = 1;
    int diceSides;
    QString playFilePath;
    QStringList playFileDataTemp;
    QIntValidator *validatorInt;
    int currentSelectTableRow;
    QString totalPlayTimes;

    int customCheckAttr;
    int customThreshold;

    double timeSpeed=1.0f;
    bool needCheck;

    //首次加载初始化
    void on_MainWindow_timerInitialize();



    MainWindow(QWidget *parent = nullptr);


    ~MainWindow();


private slots:

    //时间类
    void updateTimeInfo();
    void updateHSliderTimeQS();

    //自定义检定
    int exprDecodingGeneralMethod(QString);
    QString exprCustomNormalizMethod(int);
    void displayDiceNumber(int);

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
    void updateLastPlayHistoryBoard();
    void updateInfoBoard(QList<Character>&,int,QTableWidget*);
    void updateLabelBoard();
    void updateLabelActionTimes(int);
    void updateLabelCustomExprInfo(QTableWidgetItem *);
    void updateLabelJuggInfo();
    void saySomething(QString words,bool update=false);
    void updateExprQSBoard();

    //面板反向更新至数组中
    void update_tableItemDelete(QString key);
    void update_enemyTableItemDelete(QString key);
    void update_changeTableItem(int,int,QList<Character>&,int,QTableWidget*);


    //菜单按钮动作区
    void on_action_loadFile_triggered();
    void on_action_saveFile_triggered();

    void on_pushButton_toNextCharacter_clicked();

    void on_pushButton_toPreCharacter_clicked();

    void on_pushButton_showCharacterRecords_clicked();

    void on_pushButton_addCharacterRecord_clicked();


    void on_pushButton_deleteTableHeader_clicked();

    void on_tableWidget_characterInfo_itemClicked(QTableWidgetItem *);



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

    void on_tableWidget_characterInfo_cellChanged(int row, int column);

    void on_tableWidget_enemyInfo_cellChanged(int row, int column);

    void on_lineEdit_customExprInput_textChanged(const QString &arg1);


    void on_pushButton_executeCustomExpr_clicked();


    void on_lineEdit_cunstomExprThreshold_textChanged(const QString &arg1);

    void on_toolButton_custom_help_triggered(QAction *arg1);

    void on_toolButton_custom_help_clicked();

    void on_dial_timerFreqAdjust_sliderReleased();

    void on_hSlider_timeSpeedQS_sliderReleased();

    void on_lineEdit_timeSpeed_textEdited(const QString &arg1);

    void on_pushButton_timerPause_clicked();

    void on_pushButton_timerGo_clicked();



    void on_pushButton_saySomething_clicked();

    void on_tableWidget_checkTable_cellClicked(int row);

    void on_lineEdit_customExprInput_textEdited(const QString &arg1);


private:
    Ui::MainWindow *ui;

    //时间类
    QTimer *timer;
    int updateFreq=1000;
    double passedSeconds=0.0f;
    int passedDays=0;
    double pauseSpeed=1.0f;
    QTime totalTime = QTime(0,0,0);

signals:
    void historyUpdate(QString &infoBuffer);

};
#endif // MAINWINDOW_H
