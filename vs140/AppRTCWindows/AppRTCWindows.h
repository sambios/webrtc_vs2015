#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AppRTCWindows.h"

class AppRTCWindows : public QMainWindow
{
	Q_OBJECT

public:
	AppRTCWindows(QWidget *parent = Q_NULLPTR);

private slots:
    void on_btnConnect_clicked();

    void on_btnSettings_clicked();

private:
	Ui::AppRTCWindowsClass ui;
};
