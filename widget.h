#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
const QString DATETIME_FMT = "yyyy-MM-dd hh:mm:ss";
const QString DATE_FMT = "yyyy-MM-dd";
const QString TIME_FMT = "hh:mm:ss";

QT_BEGIN_NAMESPACE
namespace Ui {class Widget;}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private slots:
    void do_valueChanged(int value);
    void on_checkBox_Visible_clicked(bool checked);

    void on_checkBox_Inverted_clicked(bool checked);

    void on_radio_Percent_clicked();

    void on_radio_Current_clicked();

    void on_btnDebugTime_clicked();

    void on_btnDebugDate_clicked();

    void on_btnDebugDateTime_clicked();

    void on_btnGetTime_clicked();

    void on_btnSetTime_clicked();

    void on_btnSetDate_clicked();

    void on_btnSetDateTime_clicked();

    void on_calendarWidget_selectionChanged();

private:
    void applyQSS();
    void initSignalSlots();
    void initUi();
    Ui::Widget *ui;
};
#endif // WIDGET_H
