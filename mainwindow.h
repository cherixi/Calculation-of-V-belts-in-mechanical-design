#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString version = "v1.0.0.20231108_rc";

    const float pi = 3.14159;

    float P;    //名义功率，单位kW
    float Pd;   //设计功率，单位kW
    float n1;   //小带轮转速，单位RPM
    float i;    //传动比
    float n2;   //大带轮转速，单位RPM
    int moterClass;   //电动机类型
    float KA;   //工作情况系数
    int hoursPerDay;    //1~3
    int load;   //1~4

    int Vtype;  //Z~E -> 1~6
    float v;    //带速，单位m/s
    float dd1;  //小带轮基准直径，单位mm
    float dd2;  //大带轮基准直径，单位mm
    int vmax;   //最大带速，单位m/s
    float amin; //轴间距最小值，单位mm
    float amax; //轴间距最大值，单位mm
    float a0;    //初选中心距，mm

    float ld0;  //初选带基准长度，单位mm
    float ld;   //基准长度，单位mm

    float a;    //中心距，单位mm
    float alpha1;   //小轮包角，单位°
    float Kb;   //弯曲影响系数
    float Ki;   //传动比系数
    float Kalpha;   //包角修正系数
    float KL;   //长度系数
    int z;  //根数
    float P0;   //基本额定功率，单位kW
    float m;    //每米长度质量，单位kg/m
    float F0;   //初拉力，单位N
    float Q;    //压力，单位N

protected:
    void inputBasicDatas();
    float getKA();
    int getVtype();
    void getK();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
