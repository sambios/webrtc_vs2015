#include "apprtc_settings_dlg.h"
#include "ui_apprtc_settings_dlg.h"

apprtc_settings_dlg::apprtc_settings_dlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::apprtc_settings_dlg)
{
    ui->setupUi(this);
}

apprtc_settings_dlg::~apprtc_settings_dlg()
{
    delete ui;
}
