#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QTableWidgetItem>
#include <QMetaType>
#include <QRegExpValidator>
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
    int currentActCharacter;

    MainWindow(QWidget *parent = nullptr);


    ~MainWindow();


private slots:
    //信息载入
    void setPlayFileXML();
    void savePlayFileXML();

    //更新记录面板
    void updateHistoryBoard(QString &infoBuffer);
    void updateCharacterBoard(int index);
    void updateLabelBoard();

    //面板反向更新至数组中
    void update_tableItemDelete(QString key);

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


private:
    Ui::MainWindow *ui;
    int diceThrowTimes = 1;
    int successThreshold;
    int diceSides;
    QString playFilePath;
    QStringList playFileDataTemp;
    QIntValidator *validatorInt;  
    int currentSelectTableRow;
    QVector<QString> headerBuffer;
    QVector<QVariant> valueBuffer;

    void saySomething(QString words,bool update=false);

signals:
    void historyUpdate(QString &infoBuffer);

};
#endif // MAINWINDOW_H
