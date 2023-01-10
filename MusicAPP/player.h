#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QJsonArray>
#include <QTimer>
#include <QCloseEvent>

namespace Ui {
class Player;
}

#define START_PALY    1
#define SUSPEND_PLAY  2
#define STOP_PLAY     3

class Player : public QWidget
{
    Q_OBJECT

public:
    explicit Player(QTcpSocket *s, QWidget *parent = 0);
    ~Player();
    void closeEvent(QCloseEvent *event);

private slots:
    void server_reply_slot();

    void on_startButton_clicked();

    void on_priorButton_clicked();

    void on_nextButton_clicked();

//    void slider_changed_slot();

    void on_upButton_clicked();

    void on_downButton_clicked();

    void on_seqButton_cliecked();

    void on_circleButton_cliecked();

    void on_randomButton_cliecked();

    void timeout_slot();

private:
    Ui::Player *ui;
    QTcpSocket *socket;
    int play_flag;
    QTimer timer;
};

#endif // PLAYER_H
