#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->label->installEventFilter(this);
    connect(this, this->finishcal, [=](){
        draw(ui->label);
    });
}

Widget::~Widget(){
    delete ui;
}

bool Widget::eventFilter(QObject *obj, QEvent *event){
    if(obj == ui->label){
        draw(ui->label);
        return true;
    }
    else return QWidget::eventFilter(obj, event);
}

void Widget::draw(QLabel *label){
    QPainter painter(label);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.drawRect(0,0,500,500);

    if(dots.empty()) return;

    painter.setPen(Qt::red);
    //x = 0.0, y = 0.0, r = 0.5;
    painter.drawEllipse(QPointF(250.00+x*150.00, 250.00+y*150.00), 150.00*r, 150.00*r);

    painter.setPen(QPen(Qt::black, 2));
    for(auto e:dots)
        painter.drawPoint(250.00 + e.x*150.00, 250.00 +e.y*150.00);
}

void Widget::on_pushButton_clicked(){
    QString filename = QFileDialog::getOpenFileName(this,
                                                "open a file",
                                                "C:",
                                                "txt(*.txt) ;; all (*.*)");
    if(filename.isEmpty())
        return ;
    qDebug() << filename;
    ui->pushButton->setText(filename + "\nis open");

    dots.clear();
    QFile DFile(filename);
    DFile.open(QIODevice::ReadOnly);

    QByteArray array;
    array = DFile.readLine();
    QString temp = array;
    temp.erase(temp.end()-1);
    int N = temp.toInt();

    for(int i = 0; i < N; i++){
        array = DFile.readLine();
        QString temp = array;
        temp.erase(temp.cend()-1);

        while(temp[0] == ' ') temp.erase(temp.begin());
        std::string str = temp.toStdString();
        double x1 = stod(str);

        while(temp[0] != ' ') temp.erase(temp.begin());
        temp.erase(temp.begin());
        str = temp.toStdString();
        double y1 = stod(str);


        dot t(x1, y1);
        dots.push_back(t);
    }

    if(dots.empty()) return ;

    clock_t start, end;
    start = clock();
    cal();
    end = clock();
    ui->lineEdit->setText(QString::number(end - start));
}

void Widget::cal(){
    int j = 0;
    for(int i = 1; i < dots.size(); i++){
        if(dots[i].y < dots[j].y
            || (dots[i].y == dots[j].y && dots[i].x < dots[j].x))
            j = i;
    }

    std::swap(dots[j], dots[0]);

    std::sort(dots.begin()+1,dots.end(), [=](const dot& a1, const dot& a2)->bool
    {
        if(atan2(a1.y - dots[0].y, a1.x- dots[0].x)
            != atan2(a2.y - dots[0].y, a2.x- dots[0].x))
            return atan2(a1.y - dots[0].y, a1.x- dots[0].x) < atan2(a2.y - dots[0].y, a2.x- dots[0].x);
        else return (((a1.x-dots[0].x) * (a1.x-dots[0].x) + (a1.y-dots[0].y)*(a1.y-dots[0].y))
                    < ((a2.x-dots[0].x) * (a2.x-dots[0].x) + (a2.y-dots[0].y)*(a2.y-dots[0].y)));
    });

    j = 4;
    std::vector<dot> temp = dots;
    while(j < temp.size()){
        if(atan2(temp[j].y - temp[0].y, temp[j].x- temp[0].x)
            == atan2(temp[j-1].y - temp[0].y, temp[j-1].x- temp[0].x))
        {
            temp.erase(temp.begin() + j-1);
        }
        else j++;
    }

    //Graham
    std::vector<dot> CH;

    for(auto e:temp){
        if(CH.size() <= 3){
            CH.push_back(e);
        }
        else{
            while((e.x - (CH.end()-2)->x) * (CH.back().y - (CH.end()-2)->y)
                   >= (CH.back().x - (CH.end()-2)->x) * (e.y - (CH.end()-2)->y))//right turn or collinear
            {
                CH.pop_back();
            }
            CH.push_back(e);
        }
    }

    for(auto e : CH){
        qDebug() << e.x << e.y;
    }

    //enum
    r = 0;
    x = CH[0].x;
    y = CH[0].y;

    for(int i = 1; i < CH.size(); i++){
        if(!incircle(CH[i])){
            r = 0;
            x = CH[i].x;
            y = CH[i].y;
            for(int j = 0; j < i; j++){
                if(!incircle(CH[j])){
                    x = abs((CH[i].x + CH[j].x)/2);
                    y = abs((CH[i].y + CH[j].y)/2);
                    r = sqrt((CH[i].x-CH[j].x) * (CH[i].x-CH[j].x) + (CH[i].y-CH[j].y) * (CH[i].y-CH[j].y)) / 2;
                    for(int k = 0; k < j; k++){
                        if(!incircle(CH[k])){
                            makecircle(CH[i], CH[j], CH[k]);
                        }
                    }
                }
            }
        }
    }

    ui->lineEdit_4->setText(QString::number(x));
    ui->lineEdit_3->setText(QString::number(y));
    ui->lineEdit_5->setText(QString::number(r));

    emit finishcal();
}

bool Widget::incircle(dot p){
    return (((p.x-x)*(p.x-x) + (p.y-y)*(p.y-y)) <= r*r);
}

void Widget::makecircle(dot a, dot b, dot c){
    x = ((b.y - a.y) * (c.y*c.y - a.y*a.y + c.x*c.x - a.x*a.x)
         - (c.y  -a.y) * (b.y*b.y - a.y*a.y + b.x*b.x - a.x*a.x))
        / (2*(c.x - a.x)*(b.y - a.y) - 2*(b.x - a.x)*(c.y - a.y));

    y = ((b.x - a.x) * (c.x*c.x - a.x*a.x + c.y*c.y - a.y*a.y)
         - (c.x - a.x) * (b.x*b.x - a.x*a.x + b.y*b.y - a.y*a.y))
        / (2*(c.y - a.y)*(b.x - a.x) - 2*(b.y - a.y)*(c.x - a.x));

    double l1 = sqrt((a.x-b.x) * (a.x-b.x) + (a.y-b.y) * (a.y-b.y));
    double l2 = sqrt((a.x-c.x) * (a.x-c.x) + (a.y-c.y) * (a.y-c.y));
    double l3 = sqrt((b.x-c.x) * (b.x-c.x) + (b.y-c.y) * (b.y-c.y));
    if(l1 > l2) std::swap(l1, l2);
    if(l2 > l3) std::swap(l2, l3);

    if(l1*l1 + l2*l2 < l3*l3) r = l3 / 2;
    else r = (l1*l2*l3)/(2* abs((b.x-a.x) * (c.y-a.y) - (b.y-a.y) * (c.x-a.x)));
}
