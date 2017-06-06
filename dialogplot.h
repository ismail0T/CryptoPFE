#ifndef DIALOGPLOT_H
#define DIALOGPLOT_H

#include <QDialog>

namespace Ui {
class DialogPlot;
}

class DialogPlot : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPlot(QWidget *parent = 0);
    ~DialogPlot();
    void setA(int a);
    void setB(int b);
    void setP(int p);
    void setR(int r);
    bool drawCurve();

private:
    Ui::DialogPlot *ui;
    int a, b, p, r;
};

#endif // DIALOGPLOT_H
