/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief main cutecom-ng window
 *
 * \version $Id: $
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}


class SessionManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    SessionManager *session_mgr;

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

private:
    void openConnectionDialog();
};

#endif // MAINWINDOW_H
