#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPaintDevice>
#include <QPainter>
#include <QFileDialog>
#include <bits/stdc++.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

struct dot{
    qreal x;
    qreal y;
    dot(qreal a = 0.0, qreal b = 0.0){
        x = a, y = b;
    }
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    qreal x, y, r;//size of circle
    std::vector<dot> dots;

    bool eventFilter(QObject *obj, QEvent *event);
    void draw(QLabel *label);
    void cal();
    bool incircle(dot p);
    void makecircle(dot a, dot b, dot c);
private slots:
    void on_pushButton_clicked();

private:
    Ui::Widget *ui;
signals:
    void finishcal();
};
#endif // WIDGET_H
