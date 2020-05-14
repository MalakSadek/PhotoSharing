#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

    void on_pushButton_5_clicked();

    void askClient();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void DatabaseConnect();
    void DatabaseInit();

    void on_listView_2_doubleClicked(const QModelIndex &index);
    void request1View();
    void askForNewUsername();
    void updateLog();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
