#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    socket = new QTcpSocket;
    socket->connectToHost(IP, PORT);

    connect(socket, &QTcpSocket::connected, [this]()
    {
        QMessageBox::information(this, "连接提示", "连接成功");
    });

    connect(socket, &QTcpSocket::readyRead, this, &Widget::server_reply_slot);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QString app_id = ui->appLineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "search_bind");
    obj.insert("appid", app_id);

    //向服务器发送数据，查询是否已经绑定
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

void Widget::server_reply_slot()
{
    QByteArray ba = socket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();

    QString cmd = obj.value("cmd").toString();
    if (cmd == "reply_bind")
    {
        QString result = obj.value("result").toString();
        if ("yes" == result)                 //已经绑定
        {
            qDebug() << "已经绑定";

            //断开信号与槽
            disconnect(socket, 0, this, 0);

            //跳转到新的界面
            Player *p = new Player(socket);
            p->show();
            this->close();

        }
        else if ("no" == result)
        {
            qDebug() << "未绑定";

            //绑定
            bind_operation();
        }
    }
    else if (cmd == "bind_success")
    {
        qDebug() << "绑定成功";

        //断开信号与槽
        disconnect(socket, 0, this, 0);

        //跳转到新的界面
        Player *p = new Player(socket);
        p->show();
        this->close();
    }
}

void Widget::bind_operation()
{
    QString app_id = ui->appLineEdit->text();
    QString dev_id = ui->devLineEdit->text();

    QJsonObject obj;
    obj.insert("cmd", "bind");
    obj.insert("appid", app_id);
    obj.insert("deviceid", dev_id);

    //QJsonOBject转换成QBYteArray
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

