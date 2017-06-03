#include "bigwindow.h"
#include "ui_bigwindow.h"

BigWindow::BigWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BigWindow)
{
    ui->setupUi(this);

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    ecc_big = new ECC_BIG();
    ecc_big->setA(ui->lineEdit_a->text().toInt());
    ecc_big->setB(stringToMPI("18958286285566608000408668544493926415504680968679321075787234672564"));
    ecc_big->setP(stringToMPI("26959946667150639794667015087019630673557916260026308143510066298881"));

    private_key_a = "20723429452102997097693055120908112174847588083791179561894667245437";
    private_key_b = "26783546327533480407843357618229179705380334259814175764895254907511";

    mpi numberB; mpi_init(&numberB);
    mpi numberP; mpi_init(&numberP);

    numberB = stringToMPI("18958286285566608000408668544493926415504680968679321075787234672564");
    numberP = stringToMPI("26959946667150639794667015087019630673557916260026308143510066298881");

    /*number = stringToMPI(private_key_a);

    text = mpiToString(number);

    qDebug() << text;*/

    // creating base point
    MyPoint *myPoint = new MyPoint();
    mpi base_x, base_y;
    mpi_init(&base_x);mpi_init(&base_y);

    base_x = stringToMPI(ui->lineEdit_base_x->text());
    base_y = stringToMPI(ui->lineEdit_base_y->text());

   // base_x = stringToMPI("15");
   // base_y = stringToMPI("1051");

    // 6455442420784385171892731890321939130302256445452508665571987526
    // 11713622093973467124672938316960118024378453519425273321965577354050
    myPoint->setX(base_x);
    myPoint->setY(base_y);
/*
    MyPoint *myPoint0 = new MyPoint();
    MyPoint *myPoint1 = new MyPoint();
    //ecc_big->setBasePoint(myPoint);
    myPoint0 = ecc_big->addDouble(myPoint);

    myPoint1 = ecc_big->addPoints(myPoint0, myPoint);
    myPoint1 = ecc_big->addPoints(myPoint1, myPoint);

    myPoint0 = ecc_big->addDouble(myPoint0);
*/

   /* qDebug() << "zzzz " << mpiToString(myPoint0->X());
    qDebug() << "zzzz " << mpiToString(myPoint0->Y());
    qDebug() << "zzzz " << mpiToString(myPoint1->X());
    qDebug() << "zzzz " << mpiToString(myPoint1->Y());*/


  /*  qDebug() << "numberB " << mpiToString(numberB);
    qDebug() << "numberP " << mpiToString(numberP);
    qDebug() << "base_x " << mpiToString(base_x);
    qDebug() << "base_y " << mpiToString(base_y);
*/
    mpi t0, t1, t3;
    mpi_init(&t0);mpi_init(&t1);mpi_init(&t3);
    //y^2 = x^3 -3X + B
    mpi_mul_mpi(&t3, &base_y, &base_y);
    mpi_mod_mpi(&t3, &t3, &numberP);

    mpi_mul_mpi(&t0, &base_x, &base_x);
    mpi_mul_mpi(&t0, &t0, &base_x);

    mpi_mul_int(&t1, &base_x, 3);//t1<- 3*x

    mpi_add_mpi(&t0, &t0, &numberB);
    mpi_sub_mpi(&t0, &t0, &t1);

    mpi_mod_mpi(&t0, &t0, &numberP);

    qDebug() << "zzzz " << mpiToString(t0);
    qDebug() << "zzzz " << mpiToString(t3);
}

BigWindow::~BigWindow()
{
    delete ui;
}

int BigWindow::generateRNG(void *, unsigned char *buffer, size_t numBytes)
{
    for (size_t i = 0; i < numBytes; i++) {
        buffer[i] = qrand() % 256;
    }
    return 0;
}

QString BigWindow::generatePrime()
{
    int k = 224;
    char buff[k];
    size_t nlen = k;
    QString text = "";
    mpi number; mpi_init(&number);

    mpi_gen_prime( &number, k, 0, generateRNG, NULL );
    mpi_write_string( &number, 10, buff, &nlen);

    for( int i = 0; i < 68; i++ ){
        text += buff[i];
    }
    return text;
}

mpi BigWindow::generatePrimeMPI()
{
    int k = 224;
    mpi number; mpi_init(&number);
    mpi_gen_prime( &number, k, 0, generateRNG, NULL );

    return number;
}

mpi BigWindow::stringToMPI(QString text)
{
    mpi number; mpi_init(&number);
    int k = 224;
    char buff[k];

    for (int i = 0; i < text.size(); i++){
        buff[i] = text.at(i).toLatin1();
    }
    mpi_read_string( &number, 10, buff);

    return number;
}

QString BigWindow::mpiToString(mpi number)
{
    int k = 512;
    char buff[k];
    size_t nlen = k;
    QString text = "";

    mpi_write_string( &number, 10, buff, &nlen);

    for( int i = 0; i < 69; i++ ){
        text += buff[i];
    }

    return text;
}
