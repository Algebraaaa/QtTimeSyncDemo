#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    initUi();
    initSignalSlots();
    applyQSS();
}

Widget::~Widget()
{
    delete ui;
}
void Widget::applyQSS()
{
    QString style = R"(
        QWidget {
            background-color: #f6f7fb;
            font-family: "Microsoft YaHei";
            font-size: 14px;
            color: #333;
        }

        QPushButton {
            background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,
                                              stop:0 #4FC3F7, stop:1 #0288D1);
            color: white;
            border-radius: 8px;
            padding: 5px 10px;
        }

        QPushButton:hover {
            background-color: #5EC8FF;
        }

        QProgressBar {
            border: 1px solid #bbb;
            border-radius: 8px;
            text-align: center;
            background: #eee;
            height: 16px;
        }

        QProgressBar::chunk {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,
                                        stop:0 #4FC3F7, stop:1 #0288D1);
            border-radius: 8px;
        }

        QSlider::groove:horizontal {
            border: 1px solid #bbb;
            height: 6px;
            background: #e0e0e0;
            border-radius: 3px;
        }

        QSlider::handle:horizontal {
            background: #0288D1;
            border: 1px solid #0277BD;
            width: 16px;
            margin: -5px 0;
            border-radius: 8px;
        }

        QSlider::handle:horizontal:hover {
            background: #4FC3F7;
        }

        QScrollBar:horizontal {
            background: #e0e0e0;
            height: 10px;
            margin: 0px 20px 0px 20px;
            border-radius: 5px;
        }

        QScrollBar::handle:horizontal {
            background: #0288D1;
            border-radius: 5px;
        }

        QDial {
            background: #f8f8f8;
            border: 2px solid #0288D1;
            border-radius: 50%;
        }

        QLineEdit, QDateEdit, QTimeEdit, QDateTimeEdit {
            border: 1px solid #ccc;
            border-radius: 6px;
            padding: 3px;
            background: #fff;
        }

        QLineEdit:focus, QDateEdit:focus, QTimeEdit:focus, QDateTimeEdit:focus {
            border: 1px solid #0288D1;
            box-shadow: 0 0 4px rgba(2,136,209,0.4);
        }

        QCalendarWidget QAbstractItemView {
            selection-background-color: #0288D1;
            selection-color: white;
            gridline-color: #ddd;
            background-color: white;
            border-radius: 4px;
        }

        QCalendarWidget QToolButton {
            color: white;
            background-color: #0288D1;
            border-radius: 4px;
        }

        QCalendarWidget QWidget#qt_calendar_navigationbar {
            background-color: #4FC3F7;
        }
    )";

    this->setStyleSheet(style);
}

void Widget::do_valueChanged(int value)
{
    // 更新进度条
    ui->progressBar->setValue(value);

    // 防止递归信号循环
    ui->horizontalSlider->blockSignals(true);
    ui->horizontalScrollBar->blockSignals(true);
    ui->dial->blockSignals(true);

    //使其他控件同步
    ui->horizontalSlider->setValue(value);
    ui->horizontalScrollBar->setValue(value);
    ui->dial->setValue(value);

    //恢复信号
    ui->horizontalSlider->blockSignals(false);
    ui->horizontalScrollBar->blockSignals(false);
    ui->dial->blockSignals(false);
}

void Widget::initSignalSlots()
{
    connect(ui->horizontalSlider,&QSlider::valueChanged,this,&Widget::do_valueChanged);
    connect(ui->dial,&QDial::valueChanged,this,&Widget::do_valueChanged);
    connect(ui->horizontalScrollBar,&QScrollBar::valueChanged,this,&Widget::do_valueChanged);
    //时间联动逻辑 --------------------------------------
    //使用lambda函数，不用再写多个小槽函数，逻辑集中、清晰
    //timeEdit → 更新 dateTimeEdit
    connect(ui->timeEdit,&QTimeEdit::timeChanged,this,[this](const QTime &time)
    {
        QDateTime dt=ui->dateTimeEdit->dateTime();
        dt.setTime(time);
        ui->dateTimeEdit->setDateTime(dt);
        ui->editTime->setText(time.toString(TIME_FMT));
        ui->editDateTime->setText(dt.toString(DATETIME_FMT));
    });
    //dateEdit → 更新 dateTimeEdit、calendarWidget
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, [this](const QDate &date){
        QDateTime dt = ui->dateTimeEdit->dateTime();
        dt.setDate(date);
        ui->dateTimeEdit->setDateTime(dt);
        ui->editDate->setText(date.toString(DATE_FMT));
        ui->editDateTime->setText(dt.toString(DATETIME_FMT));
        ui->calendarWidget->setSelectedDate(date);
    });
    // dateTimeEdit → 更新 dateEdit、timeEdit、calendarWidget
    connect(ui->dateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, [this](const QDateTime &dt){
        ui->timeEdit->setTime(dt.time());
        ui->dateEdit->setDate(dt.date());
        ui->editTime->setText(dt.time().toString(TIME_FMT));
        ui->editDate->setText(dt.date().toString(DATE_FMT));
        ui->editDateTime->setText(dt.toString(DATETIME_FMT));
        ui->calendarWidget->setSelectedDate(dt.date());
    });
    // calendarWidget → 更新 dateEdit、dateTimeEdit
    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged, this, [this](){
        QDate date = ui->calendarWidget->selectedDate();
        ui->dateEdit->setDate(date);
        QDateTime dt = ui->dateTimeEdit->dateTime();
        dt.setDate(date);
        ui->dateTimeEdit->setDateTime(dt);
        ui->editCalendar->setText(date.toString("yyyy年M月d日"));
        ui->editDate->setText(date.toString(DATE_FMT));
        ui->editDateTime->setText(dt.toString(DATETIME_FMT));
    });
}

void Widget::initUi()
{
    QDateTime now = QDateTime::currentDateTime();
    ui->timeEdit->setTime(now.time());
    ui->dateEdit->setDate(now.date());
    ui->dateTimeEdit->setDateTime(now);
    ui->calendarWidget->setSelectedDate(now.date());

    ui->horizontalSlider->setValue(30);
    ui->horizontalScrollBar->setValue(30);
    ui->dial->setValue(30);
}

void Widget::on_checkBox_Visible_clicked(bool checked)
{
    ui->progressBar->setTextVisible(checked);
}


void Widget::on_checkBox_Inverted_clicked(bool checked)
{
    ui->progressBar->setInvertedAppearance(checked);
}


void Widget::on_radio_Percent_clicked()
{
    ui->progressBar->setFormat("%p%");
}


void Widget::on_radio_Current_clicked()
{
    ui->progressBar->setFormat("%v");
}


void Widget::on_btnDebugTime_clicked()
{
    QTime TM1(13,24,5);
    QString str=TM1.toString(TIME_FMT);
    qDebug("Original time=%s",str.toLocal8Bit().data());
    QTime TM2=TM1.addSecs(150);
    str=TM2.toString(TIME_FMT);
    qDebug("150s later, time=%s",str.toLocal8Bit().data());

    TM2=QTime::currentTime();
    str=TM2.toString("HH:mm:ss zzz");
    qDebug("Current time=%s",str.toLocal8Bit().data());
    qDebug()<<"Hour="<<TM2.hour();
    qDebug()<<"Minute="<<TM2.minute();
    qDebug()<<"Second="<<TM2.second();
    qDebug()<<"MSecond="<<TM2.msec();

}


void Widget::on_btnDebugDate_clicked()
{
    QDate DT1(2021,7,6);
    QString str=DT1.toString(DATE_FMT);
    qDebug()<<"DT1="<<str.toLocal8Bit().data();
    QDate DT2;
    DT2.setDate(2021,8,25);
    str=DT2.toString(DATE_FMT);
    qDebug()<<"DT2="<<str.toLocal8Bit().data();
    qDebug()<<"Days between DT2 and DT1="<<DT2.daysTo(DT1);

    DT2=QDate::currentDate();
    str=DT2.toString(DATE_FMT);
    qDebug()<<"Current date="<<str.toLocal8Bit().data();
    qDebug()<<"Years="<<DT2.year();
    qDebug()<<"Month="<<DT2.month();
    qDebug()<<"Day="<<DT2.day();
    qDebug()<<"Day of Week="<<DT2.dayOfWeek();
}


void Widget::on_btnDebugDateTime_clicked()
{
    QDateTime DT1=QDateTime::currentDateTime();
    QString str=DT1.toString(DATETIME_FMT);
    qDebug()<<"DT1="<<str.toLocal8Bit().data();

    QDate dt=DT1.date();
    str=dt.toString(DATE_FMT);
    qDebug()<<"DT1.date()="<<str.toLocal8Bit().data();
    QTime tm=DT1.time();
    str=tm.toString(DATE_FMT);
    qDebug()<<"DT1.time()="<<str.toLocal8Bit().data();

    qint64 MS=DT1.toSecsSinceEpoch();
    qDebug()<<"DT1.toSecsSinceEpoch()="<<MS;
    MS+=120;
    DT1.setSecsSinceEpoch(MS);
    str=DT1.toString(DATETIME_FMT);
    qDebug()<<"DT1+120s="<<str.toLocal8Bit().data();
}


void Widget::on_btnGetTime_clicked()
{
    QDateTime curDateTime=QDateTime::currentDateTime();
    ui->timeEdit->setTime(curDateTime.time());
    ui->editTime->setText(curDateTime.toString(TIME_FMT));
    ui->dateEdit->setDate(curDateTime.date());
    ui->editDate->setText(curDateTime.toString(DATE_FMT));
    ui->dateTimeEdit->setDateTime(curDateTime);
    ui->editDateTime->setText(curDateTime.toString(DATETIME_FMT));
    QDate curDate=QDate::currentDate();
    ui->calendarWidget->setSelectedDate(curDate);
    QString str=curDate.toString("yyyy年M月d日");
    ui->editCalendar->setText(str);
}


void Widget::on_btnSetTime_clicked()
{
    QString str=ui->editTime->text();
    str=str.trimmed();
    if(!str.isEmpty())
    {
        QTime tm=QTime::fromString(str,TIME_FMT);
        ui->timeEdit->setTime(tm);
    }
}


void Widget::on_btnSetDate_clicked()
{
    QString str=ui->editDate->text();
    str=str.trimmed();
    if(!str.isEmpty())
    {
        QDate dt=QDate::fromString(str,DATE_FMT);
        ui->dateEdit->setDate(dt);
        ui->calendarWidget->setSelectedDate(dt);
    }
}


void Widget::on_btnSetDateTime_clicked()
{
    QString str=ui->editDateTime->text();
    str=str.trimmed();
    if(!str.isEmpty())
    {
        QDateTime dtm=QDateTime::fromString(str,DATETIME_FMT);
        ui->dateTimeEdit->setDateTime(dtm);
    }
}


void Widget::on_calendarWidget_selectionChanged()
{
    QDate dt=ui->calendarWidget->selectedDate();
    QString str=dt.toString("yyyy年M月d日");
    ui->editCalendar->setText(str);
}

