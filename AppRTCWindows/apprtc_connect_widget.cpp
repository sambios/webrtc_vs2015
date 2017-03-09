#include "apprtc_connect_widget.h"
#include "ui_apprtc_connect_widget.h"

apprtc_connect_widget::apprtc_connect_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::apprtc_connect_widget)
{
    ui->setupUi(this);
}

apprtc_connect_widget::~apprtc_connect_widget()
{
    delete ui;
}
