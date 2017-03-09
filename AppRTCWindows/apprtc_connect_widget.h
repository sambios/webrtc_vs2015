#ifndef APPRTC_CONNECT_WIDGET_H
#define APPRTC_CONNECT_WIDGET_H

#include <QWidget>

namespace Ui {
class apprtc_connect_widget;
}

class apprtc_connect_widget : public QWidget
{
    Q_OBJECT

public:
    explicit apprtc_connect_widget(QWidget *parent = 0);
    ~apprtc_connect_widget();

private:
    Ui::apprtc_connect_widget *ui;
};

#endif // APPRTC_CONNECT_WIDGET_H
