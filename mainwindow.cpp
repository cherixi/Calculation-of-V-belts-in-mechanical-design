#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <cmath>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/lpgo-blue.png"));
    this->setWindowTitle("V带设计程序");
    ui->label_version->setText(version);
    ui->tabWidget->setTabEnabled(1,false);
    ui->tabWidget->setTabEnabled(2,false);
    ui->tabWidget->setTabEnabled(3,false);

    //点击tab1按钮，检查基本数据并计算
    connect(ui->pushButton_1, &QPushButton::clicked, this, [=](){
        P = ui->doubleSpinBox_P->value();
        n1 = ui->doubleSpinBox_n1->value();
        i = ui->doubleSpinBox_i->value();
        n2 = n1 / i;

        if (ui->radioButton_class1->isChecked())
            moterClass = 1;
        else
            moterClass = 2;

        if (ui->radioButton_hours1->isChecked())
            hoursPerDay = 1;
        else if (ui->radioButton_hours2->isChecked())
            hoursPerDay = 2;
        else
            hoursPerDay = 3;

        if (ui->radioButton_load1->isChecked())
            load = 1;
        else if (ui->radioButton_load2->isChecked())
            load = 2;
        else if (ui->radioButton_load3->isChecked())
            load = 3;
        else
            load = 4;

        KA = getKA();
        if (ui->checkBox_env->isChecked())
            KA *= 1.2;
        Pd = KA * P;


        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(0,false);
        ui->pushButton_2->setEnabled(false);
        ui->tabWidget->setCurrentIndex(1);

    });

    connect(ui->comboBox_Vtype, &QComboBox::currentIndexChanged, this, [=](){
        switch (ui->comboBox_Vtype->currentIndex()) {
        case 1: //A
            ui->comboBox_dd1->addItems({"75","80","90","100","112","125","140"});
            ui->comboBox_ld->addItems({"630","710","800","900","1000","1120","1250","1400","1600","1800",
                                      "2000","2240","2500","2800","3150","3550","4000"});
            vmax = 25;
            Kb = 0.0007725;
            m = 0.1;
            break;
        case 2: //B
            ui->comboBox_dd1->addItems({"125","140","150","160","180","200"});
            ui->comboBox_ld->addItems({"900","1000","1120","1250","1400","1600","1800","2000","2240","2500",
                                      "2800","3150","3550","4000","4500","5000"});
            vmax = 25;
            Kb = 0.0019875;
            m = 0.17;
            break;
        case 3: //C
            ui->comboBox_dd1->addItems({"200","224","250","280","315","355","400"});
            ui->comboBox_ld->addItems({"1600","1800","2000","2240","2500","2800","3150","3550","4000","4500",
                                       "5000","5600","6300","7100","8000","9000","10000"});
            vmax = 25;
            Kb = 0.005625;
            m = 0.3;
            break;
        case 4: //D
            ui->comboBox_dd1->addItems({"315","355","400","425","450","500"});
            ui->comboBox_ld->addItems({"2800","3150","3550","4000","4500","5000","5600","6300","7100","8000",
                                       "9000","10000","11200","12500","14000","16000"});
            vmax = 30;
            Kb = 0.01995;
            m = 0.6;
            break;
        case 5: //E
            ui->comboBox_dd1->addItems({"450","500","560","600","630","710","800"});
            ui->comboBox_ld->addItems({"4500","5000","5600","6300","7100","8000","9000","10000","11200","12500",
                                       "14000","16000"});
            vmax = 30;
            Kb = 0.3735;
            m = 0.8;
            break;
        case 0:
            ui->comboBox_dd1->addItems({"50","56","63","71","75","80","90","100"});
            ui->comboBox_ld->addItems({"630","710","800","900","1000","1120","1250","1400","1600","1800",
                                      "2000","2240","2500","2800","3150","3550","4000"});
            vmax = 25;
            Kb = 0.0007725;
            m = 0.1;
            break;
        default:
            break;
        }
        Vtype = getVtype();
        ui->label_vmax->setText(QString::number(vmax));
        qDebug() <<"Vtype:" << Vtype;
    });

    connect(ui->comboBox_dd1, &QComboBox::currentIndexChanged, this, [=](){
        dd1 = ui->comboBox_dd1->currentText().toInt();
        qDebug() << "dd1:" << dd1;
        dd2 = i * dd1;
        qDebug() << "dd2:" << dd2;
        v = 2*pi*n1*dd1/60/2/1000;
        qDebug() << "v:" << v << "m/s";
        ui->label_v->setText(QString::number(v));
        if(v < vmax)
            ui->pushButton_2->setEnabled(true);
        else
            ui->pushButton_2->setEnabled(false);
    });

    connect(ui->pushButton_2, &QPushButton::clicked, this, [=](){
        amin = 0.7 * (dd1 + dd2);
        amax = 2 * (dd1 + dd2);
        ui->doubleSpinBox_a0->setMinimum(amin);
        ui->doubleSpinBox_a0->setMaximum(amax);
        ui->label_amin->setText(QString::number(amin));
        ui->label_amax->setText(QString::number(amax));
        ui->tabWidget->setTabEnabled(2,true);
        ui->tabWidget->setTabEnabled(1,false);
        ui->tabWidget->setCurrentIndex(2);
    });

    connect(ui->doubleSpinBox_a0, &QDoubleSpinBox::valueChanged, this, [=](){
        a0 = ui->doubleSpinBox_a0->value();
        ld0 = 2*a0 + pi*(dd2+dd1)/2 + (dd2-dd1)*(dd2-dd1) / (4*a0);
        ui->label_ld0->setText(QString::number(ld0));
    });

    connect(ui->pushButton_3, &QPushButton::clicked, this, [=](){
        ld = ui->comboBox_ld->currentText().toInt();
        P0 = ui->doubleSpinBox_P0->value();
        a = a0 + (ld-ld0)/2;
        alpha1 = 180 - (dd2-dd1) / a * 57.3;
        qDebug() << "alpha1:" << alpha1;
        getK();
        z = Pd / ((P0 + Kb*n1*(1-1/Ki)) * Kalpha * KL) + 1;
        F0 = 500 * Pd/v/z * ((2.5-Kalpha)/Kalpha) + m * v * v;
        Q = 2 * F0 * z * sin(alpha1/2 * pi/180);

        ui->label_z->setText(QString::number(z));
        ui->label_alpha1->setText(QString::number(alpha1));
        ui->label_F0->setText(QString::number(F0));
        ui->label_Q->setText(QString::number(Q));

        ui->tabWidget->setTabEnabled(3,true);
        ui->tabWidget->setTabEnabled(2,false);
        ui->tabWidget->setCurrentIndex(3);
    });

    connect(ui->pushButton_init, &QPushButton::clicked, this, [=](){
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(3,false);
        ui->tabWidget->setCurrentIndex(0);
    });

    connect(ui->pushButton_4, &QPushButton::clicked, this, [=](){
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2,false);
        ui->tabWidget->setCurrentIndex(1);
    });

    connect(ui->pushButton_5, &QPushButton::clicked, this, [=](){
        ui->tabWidget->setTabEnabled(2,true);
        ui->tabWidget->setTabEnabled(3,false);
        ui->tabWidget->setCurrentIndex(2);
    });

    connect(ui->pushButton_6, &QPushButton::clicked, this, [=](){
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1,false);
        ui->tabWidget->setCurrentIndex(0);
    });

}

void MainWindow::getK()
{
    qDebug() << "alpha1:" << alpha1;
    float a[14] = {1.2, 1.15, 1.1, 1.05, 1, 0.98, 0.95, 0.92, 0.89, 0.86, 0.82, 0.78, 0.74, 0.69};
    Kalpha = a[13 - int((alpha1-90)/10)];
    qDebug() << "Kalpha:" << Kalpha;

    if (i < 1.01)
        Ki = 1;
    else if (i < 1.04)
        Ki = 1.0136;
    else if (i < 1.08)
        Ki = 1.0276;
    else if (i < 1.12)
        Ki = 1.0419;
    else if (i < 1.18)
        Ki = 1.0567;
    else if (i < 1.24)
        Ki = 1.0719;
    else if (i < 1.34)
        Ki = 1.0875;
    else if (i < 1.51)
        Ki = 1.1036;
    else if (i < 1.99)
        Ki = 1.1202;
    else
        Ki = 1.1373;

    qDebug() << "Ki:" << Ki;
    KL = ui->doubleSpinBox_KL->value();
}

int MainWindow::getVtype()
{
    return ui->comboBox_Vtype->currentIndex()+1;
}

float MainWindow::getKA()
{
    float a[2][3][4] =
    {
        {
            {1.0, 1.1, 1.2, 1.3},
            {1.1, 1.2, 1.3, 1.4},
            {1.2, 1.3, 1.4, 1.5}
        },
        {
            {1.1, 1.2, 1.4, 1.5},
            {1.2, 1.3, 1.5, 1.6},
            {1.3, 1.4, 1.6, 1.8}
        }
    };  //a[moterClass][hoursPerDay][load]
    return a[moterClass-1][hoursPerDay-1][load-1];
}

void MainWindow::inputBasicDatas()
{
}

MainWindow::~MainWindow()
{
    delete ui;
}
