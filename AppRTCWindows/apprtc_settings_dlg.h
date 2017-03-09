#ifndef APPRTC_SETTINGS_DLG_H
#define APPRTC_SETTINGS_DLG_H

#include <QDialog>

namespace Ui {
class apprtc_settings_dlg;
}

class apprtc_settings_dlg : public QDialog
{
    Q_OBJECT

public:
    explicit apprtc_settings_dlg(QWidget *parent = 0);
    ~apprtc_settings_dlg();

private:
    Ui::apprtc_settings_dlg *ui;
};

#endif // APPRTC_SETTINGS_DLG_H
