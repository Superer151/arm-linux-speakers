#include "player.h"
#include "ui_player.h"

Player::Player(QTcpSocket *s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    socket = s;

    play_flag = STOP_PLAY;

    connect(socket, &QTcpSocket::readyRead, this, &Player::server_reply_slot);

    //radioButton信号与槽
    connect(ui->seqButton, &QRadioButton::clicked, this, &Player::on_seqButton_cliecked);
    connect(ui->randomButton, &QRadioButton::clicked, this, &Player::on_randomButton_cliecked);
    connect(ui->circleButton, &QRadioButton::clicked, this, &Player::on_circleButton_cliecked);

    //启动定时器，5秒后发送获取所有音乐的请求
    timer.start(5000);

    connect(&timer, &QTimer::timeout, this, &Player::timeout_slot);
}

Player::~Player()
{
    delete ui;
}

void Player::timeout_slot()
{
    //向服务器发送获取所有音乐的请求
    QJsonObject obj;
    obj.insert("cmd", "app_music");

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);

    timer.stop();
}

void Player::server_reply_slot()
{
    QByteArray ba = socket->readAll();

    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QString cmd = obj.value("cmd").toString();

    if (cmd == "app_reply")                    //播放操作的回复
    {
        QString result = obj.value("result").toString();
        if (result == "start_success")  //开始播放成功
        {
            ui->startButton->setText("||");
            play_flag = START_PALY;
        }
        else if (result == "suspend_success")   //暂停成功
        {
            ui->startButton->setText("|>");
            play_flag = SUSPEND_PLAY;
        }
        else if (result == "continue_success")
        {
            ui->startButton->setText("||");
            play_flag = START_PALY;
        }
        else if (result == "off_line")
        {
            QMessageBox::warning(this, "操作提示", "音箱下线");
        }
    }
    else if (cmd == "app_reply_status")        //播放器状态的回复
    {
        //播放状态、歌曲名、音量
        QString status = obj.value("status").toString();
        if (status == "start")                 //音箱是播放的状态
        {
            play_flag = START_PALY;
            ui->startButton->setText("||");
        }
        else if (status == "suspend")
        {
            play_flag = SUSPEND_PLAY;
            ui->startButton->setText("|>");
        }
        else if (status == "stop")
        {
            play_flag = STOP_PLAY;
            ui->startButton->setText("|>");
        }

        QString music_name = obj.value("music").toString();
        ui->curLabel->setText(music_name);

        int level = obj.value("voice").toInt();
        ui->voiceLabel->setText(QString::number(level));
    }
    else if (cmd == "app_reply_music")         //所有音乐的回复
    {
        QJsonArray arr = obj.value("music").toArray();  //转换成json数组
        QString result;
        for (int i = 0; i < arr.count(); i++)
        {
            result.append(arr.at(i).toString());
            result.append('\n');
        }
        ui->musicEdit->setText(result);
    }
}

void Player::on_startButton_clicked()
{
    if (play_flag == SUSPEND_PLAY)
    {
        QJsonObject obj;
        obj.insert("cmd", "app_continue");

        QByteArray ba = QJsonDocument(obj).toJson();
        socket->write(ba);
    }
    else if (play_flag == START_PALY)
    {
        QJsonObject obj;
        obj.insert("cmd", "app_suspend");

        QByteArray ba = QJsonDocument(obj).toJson();
        socket->write(ba);
    }
    else if (play_flag == STOP_PLAY)
    {
        QJsonObject obj;
        obj.insert("cmd", "app_start");

        QByteArray ba = QJsonDocument(obj).toJson();
        socket->write(ba);
    }
}

void Player::on_priorButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_prior");

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

void Player::on_nextButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_next");

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}


void Player::on_upButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_voice_up");

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

//void Player::slider_changed_slot()
//{
//    QJsonObject obj;
//    obj.insert("cmd", "app_get_stats");

//    QByteArray ba = QJsonDocument(obj).toJson();
//    socket->write(ba);
//}
void Player::on_downButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_voice_down");

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

//顺序播放
void Player::on_seqButton_cliecked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_sequence");

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

//随机播放
void Player::on_randomButton_cliecked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_random");

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

//单曲循环
void Player::on_circleButton_cliecked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_circle");

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

void Player::closeEvent(QCloseEvent *event)
{
    QJsonObject obj;
    obj.insert("cmd", "app_off_line");

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
    socket->waitForBytesWritten();

    event->accept();
}

