#ifndef SHOWIMAGE_H
#define SHOWIMAGE_H

#include <QDialog>

namespace Ui {
class ShowImage;
}

class ShowImage : public QDialog
{
    Q_OBJECT
signals:
    void requestViews();

public:
    QString expiredv;
    explicit ShowImage(QWidget *parent = 0);
    void setData(const QString &pic);
    ~ShowImage();

private:
    Ui::ShowImage *ui;
};

#endif // SHOWIMAGE_H
