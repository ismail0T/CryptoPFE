#include "ecc_big.h"

ECC_BIG::ECC_BIG()
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    mpi_init(&r);
    koblitz = 30;
}

void ECC_BIG::setA(int a)
{
    this->a = a;
}

void ECC_BIG::setB(mpi b)
{
    this->b = b;
}

void ECC_BIG::setP(mpi p)
{
    this->p = p;
}

void ECC_BIG::setR(mpi r)
{
    this->r = r;
}

void ECC_BIG::setPrivateKey(mpi private_key)
{
    this->private_key = private_key;
}

void ECC_BIG::setBasePoint(MyPoint *p)
{
    this->base_point = p;
}

int ECC_BIG::getA()
{
    return a;
}

mpi ECC_BIG::getB()
{
    return b;
}

mpi ECC_BIG::getP()
{
    return p;
}

mpi ECC_BIG::getR()
{
    return this->r;
}

mpi ECC_BIG::getPrivateKey()
{
    return private_key;
}

MyPoint *ECC_BIG::getBasePoint()
{
    return base_point;
}

/*MyPoint *ECC_BIG::encryptPoint(MyPoint *p_new, int k)
{

    MyPoint *p ;

    if (k == 2) {
        p = addDouble(p_new);
    }
    else if (k > 2) {
        p = addDouble(p_new);
        for (int i = 0; i < k - 2; i++) {
            p = addPoints(p_new, p);
        }
    }

    return p;
}*/

MyPoint *ECC_BIG::encryptPointFast(MyPoint *p_new, mpi counter)
{
    MyPoint *q = new MyPoint(p_new->X(), p_new->Y());

    MyPoint *r = new MyPoint();
    int nb_double = 0;
    int nb_add = 0;
    t_uint temp_mod = 0;
    t_uint temp_1 = 1;
    mpi k; mpi_init(&k);
    mpi_copy(&k, &counter);

    while (mpi_cmp_int(&k, 0) == 1) {
        mpi_mod_int(&temp_mod, &k, 2);

        if (temp_mod == temp_1){
            r = addPoints(r, q);
            nb_add++;
        }

        mpi_div_int(&k, NULL, &k, 2);

        if (mpi_cmp_int(&k, 0) == 1){
            nb_double++;
            q = addDouble(q);

        }
    }
    return r;
}


/*if (k == 0){
    MyPoint *tt = addPoints(r, q);
    r = addPoints(r, q);
    return r;
}*/
MyPoint *ECC_BIG::decryptPoint(MyPoint *p)
{
    return NULL;
}

MyPoint *ECC_BIG::addDouble(MyPoint *point)
{
    mpi s, n, p_x, p_y, d;
    mpi temp_x_public, temp_y_public;

    mpi_init(&s);
    mpi_init(&n);
    mpi_init(&p_x);
    mpi_init(&p_y);
    mpi_init(&d);
    mpi_init(&temp_x_public);
    mpi_init(&temp_y_public);

    p_x = point->X();
    p_y = point->Y();

    mpi_mul_mpi(&n, &p_x, &p_x);
    mpi_mul_int(&n, &n, 3);

    mpi_add_int(&n, &n, a);
    mpi_mul_int(&d, &p_y, 2);

    if (mpi_cmp_int(&d, 0) == -1){
        mpi_mul_negative(&n, &n, -1);
        mpi_mul_negative(&d, &d, -1);
    }

    mpi x, temp0;
    mpi_init(&x);
    mpi_init(&temp0);

    mpi_inv_mod(&x, &d, &p);
    mpi_mul_mpi(&temp0, &x, &n);
    if (mpi_cmp_int(&temp0, 0) == 1){
        mpi_mod_mpi(&s, &temp0, &p);
    }
    else {
        s = NegModMPI(&temp0, &p);
    }

    mpi xr_, temp1, temp2;
    mpi_init(&xr_);
    mpi_init(&temp1);
    mpi_init(&temp2);

    mpi_mul_mpi(&temp1, &s, &s);
    mpi_mul_int(&temp2, &p_x, 2);
    mpi_sub_mpi(&xr_, &temp1, &temp2);

    if (mpi_cmp_int(&xr_, 0) == -1){ //xr_ < 0
        temp_x_public = NegModMPI(&xr_, &p);
    }
    else {
        mpi_mod_mpi(&temp_x_public, &xr_, &p);
    }

    mpi yr_, temp3;
    mpi_init(&yr_);
    mpi_init(&temp3);

    mpi_sub_mpi(&temp3, &p_x, &temp_x_public);
    mpi_mul_mpi(&temp3, &s, &temp3);
    mpi_sub_mpi(&yr_, &temp3, &p_y);

    if(mpi_cmp_int(&yr_, 0) == -1){ //yr_ < 0
        temp_y_public = NegModMPI(&yr_, &p);
    }
    else {
        mpi_mod_mpi(&temp_y_public, &yr_, &p);
    }

    return new MyPoint(temp_x_public, temp_y_public);
}


MyPoint *ECC_BIG::addPoints(MyPoint *point1, MyPoint *point2)
{
    mpi s, n, p1_x, p1_y, p2_x, p2_y, d;
    mpi temp_x_public, temp_y_public;

    mpi_init(&s);
    mpi_init(&n);
    mpi_init(&p1_x);
    mpi_init(&p1_y);
    mpi_init(&p2_x);
    mpi_init(&p2_y);
    mpi_init(&d);
    mpi_init(&temp_x_public);
    mpi_init(&temp_y_public);

    p1_x = point1->X();
    p1_y = point1->Y();
    p2_x = point2->X();
    p2_y = point2->Y();

    // test if no one of the points is an infinite number

    if ((mpi_cmp_int(&p1_x, 0) == 0) && (mpi_cmp_int(&p1_y, 0) == 0)) {
        //qDebug() << "p1 is infinite";
        MyPoint *pppp = new MyPoint(point2->X(), point2->Y());
        return pppp;
    }
    if ((mpi_cmp_int(&p2_x, 0) == 0) && (mpi_cmp_int(&p2_y, 0) == 0)) {
       // qDebug() << "p2 is infinite";
        return new MyPoint(point1->X(), point1->Y());
    }

    mpi_sub_mpi(&n, &p1_y, &p2_y);
    mpi_sub_mpi(&d, &p1_x, &p2_x);

    if (mpi_cmp_int(&d, 0) == -1){
        mpi_mul_negative(&n, &n, -1);
        mpi_mul_negative(&d, &d, -1);
    }

    mpi x, temp0;
    mpi_init(&x);
    mpi_init(&temp0);

    mpi_inv_mod(&x, &d, &p);
    mpi_mul_mpi(&temp0, &x, &n);

    if (mpi_cmp_int(&temp0, 0) == 1){ // temp0 > 0
        mpi_mod_mpi(&s, &temp0, &p);
    }
    else {
        s = NegModMPI(&temp0, &p);
    }

    mpi xr_, temp1, temp2;
    mpi_init(&xr_);
    mpi_init(&temp1);
    mpi_init(&temp2);

    mpi_mul_mpi(&temp1, &s, &s);
    mpi_add_mpi(&temp2, &p1_x, &p2_x);
    mpi_sub_mpi(&xr_, &temp1, &temp2);

    if (mpi_cmp_int(&xr_, 0) == -1){ //xr_ < 0
        temp_x_public = NegModMPI(&xr_, &p);
    }
    else {
        mpi_mod_mpi(&temp_x_public, &xr_, &p);
    }

    mpi yr_, temp3;
    mpi_init(&yr_);
    mpi_init(&temp3);

    mpi_sub_mpi(&temp3, &p1_x, &temp_x_public);
    mpi_mul_mpi(&temp3, &s, &temp3);
    mpi_sub_mpi(&yr_, &temp3, &p1_y);

    if(mpi_cmp_int(&yr_, 0) == -1){ //yr_ < 0
        temp_y_public = NegModMPI(&yr_, &p);
    }
    else {
        mpi_mod_mpi(&temp_y_public, &yr_, &p);
    }

    return new MyPoint(temp_x_public, temp_y_public);
}

MyPoint *ECC_BIG::generatePoint(int m)
{
    return encryptPointFast(base_point, stringToMPI(QString::number(m)));
}

QList<MyPoint *> *ECC_BIG::textToPoints(QString text)
{
    QList<MyPoint*> *list = new QList<MyPoint*>();

    for (int i = 0; i < text.size(); i++){
        QChar ch = text[i];
        int m = charToCode(ch);
        MyPoint *point = generatePoint(m);
        list->append(point);
    }
    return list;
}

int ECC_BIG::charToCode(QChar ch)
{
    if (ch == '0') return 36;
    else if (ch == '1') return 1;
    else if (ch == '2') return 2;
    else if (ch == '3') return 3;
    else if (ch == '4') return 4;
    else if (ch == '5') return 5;
    else if (ch == '6') return 6;
    else if (ch == '7') return 7;
    else if (ch == '8') return 8;
    else if (ch == '9') return 9;
    else if (ch == 'a') return 10;
    else if (ch == 'b') return 11;
    else if (ch == 'c') return 12;
    else if (ch == 'd') return 13;
    else if (ch == 'e') return 14;
    else if (ch == 'f') return 15;
    else if (ch == 'g') return 16;
    else if (ch == 'h') return 17;
    else if (ch == 'i') return 18;
    else if (ch == 'j') return 19;
    else if (ch == 'k') return 20;
    else if (ch == 'l') return 21;
    else if (ch == 'm') return 22;
    else if (ch == 'n') return 23;
    else if (ch == 'o') return 24;
    else if (ch == 'p') return 25;
    else if (ch == 'q') return 26;
    else if (ch == 'r') return 27;
    else if (ch == 's') return 28;
    else if (ch == 't') return 29;
    else if (ch == 'u') return 30;
    else if (ch == 'v') return 31;
    else if (ch == 'w') return 32;
    else if (ch == 'x') return 33;
    else if (ch == 'y') return 34;
    else if (ch == 'z') return 35;
    else if (ch == ' ') return 37;
}

QChar ECC_BIG::codeToChar(int code)
{
    switch (code){
    case 36:
        return '0';
    case 1:
        return '1';
    case 2:
        return '2';
    case 3:
        return '3';
    case 4:
        return '4';
    case 5:
        return '5';
    case 6:
        return '6';
    case 7:
        return '7';
    case 8:
        return '8';
    case 9:
        return '9';
    case 10:
        return 'a';
    case 11:
        return 'b';
    case 12:
        return 'c';
    case 13:
        return 'd';
    case 14:
        return 'e';
    case 15:
        return 'f';
    case 16:
        return 'g';
    case 17:
        return 'h';
    case 18:
        return 'i';
    case 19:
        return 'j';
    case 20:
        return 'k';
    case 21:
        return 'l';
    case 22:
        return 'm';
    case 23:
        return 'n';
    case 24:
        return 'o';
    case 25:
        return 'p';
    case 26:
        return 'q';
    case 27:
        return 'r';
    case 28:
        return 's';
    case 29:
        return 't';
    case 30:
        return 'u';
    case 31:
        return 'v';
    case 32:
        return 'w';
    case 33:
        return 'x';
    case 34:
        return 'y';
    case 35:
        return 'z';
    case 37:
        return ' ';
    default:
        return '!';
    }

}

mpi ECC_BIG::NegModMPI(mpi *a, mpi *p)
{
    /*
int b = a * -1;
int n = ceil((float)b / p);
return (n * p) - b;
*/
    mpi b, n, q, r;
    mpi_init(&b);
    mpi_init(&n);
    mpi_init(&q);
    mpi_init(&r);

    mpi_mul_negative(&b, a, -1);
    /* Division by mpi: A = Q * B + R
     \param Q        Destination MPI for the quotient
     * \param R        Destination MPI for the rest value
     * \param A        Left-hand MPI
     * \param B        Right-hand MPI

    */
    mpi_div_mpi(&q, &r, &b, p);
    int res = mpi_cmp_int(&r, 0);
    if (res == 0){ // r ==0
       n = q;
    }
    else if(res > 0){
        mpi_add_int(&n, &q, 1);
    }
    else{
        mpi_sub_int(&n, &q, 1);
    }

    mpi_mul_mpi(&n, &n, p);
    mpi_sub_mpi(&n, &n, &b);

    return n;
}

mpi ECC_BIG::stringToMPI(QString text)
{
    mpi number; mpi_init(&number);
    int k = 224;
    char buff[k];

    for (int i = 0; i < k; i++){
        buff[i] = '\n';
    }

    for (int i = 0; i < text.size(); i++){
        buff[i] = text.at(i).toLatin1();
    }
    mpi_read_string( &number, 10, buff);
    return number;
}

QString ECC_BIG::mpiToString(mpi number)
{
    int k = 2048;
    char buff[k];
    size_t nlen = k;
    QString text = "";

    int val = mpi_write_string( &number, 10, buff, &nlen);

    //qDebug() << "val " << val;
    for( int i = 0; i < k; i++ ){
        if (buff[i] == '0' || buff[i] == '1' || buff[i] == '2' || buff[i] == '3' || buff[i] == '4' || buff[i] == '5'
                || buff[i] == '6' || buff[i] == '7' || buff[i] == '8' || buff[i] == '9')  text += buff[i];
        else return text;
    }

    return text;
}

int ECC_BIG::generateRNG(void *, unsigned char *buffer, size_t numBytes)
{
    for (size_t i = 0; i < numBytes; i++) {
        buffer[i] = qrand() % 256;
    }
    return 0;
}

MyCM *ECC_BIG::generateCm(mpi counter, MyPoint *point, MyPoint *peer_public_key)
{
    MyPoint *p1 = encryptPointFast(base_point, counter);
    MyPoint *p2 = encryptPointFast(peer_public_key, counter);
    p2 = addPoints(point, p2);

    return new MyCM(p1, p2);
}

mpi ECC_BIG::generatePrivateKey()
{
    // r is the max value
    int r_size = mpi_size(&r);
    mpi key; mpi_init(&key);

    do{
        mpi_fill_random(&key, r_size, generateRNG, NULL);
    } while(mpi_cmp_mpi(&key, &r) == 1);

    return key;
}

bool ECC_BIG::isOnCurve(mpi x, mpi y)
{
    mpi t0, t1, t3;
    mpi_init(&t0);mpi_init(&t1);mpi_init(&t3);


    //y^2 = x^3 -3X + B
    mpi_mul_mpi(&t3, &y, &y);
    mpi_mod_mpi(&t3, &t3, &p);

    mpi_mul_mpi(&t0, &x, &x);
    mpi_mul_mpi(&t0, &t0, &x);

    mpi_mul_negative(&t1, &x, a);

    mpi_add_mpi(&t0, &t0, &b);
    mpi_add_mpi(&t0, &t0, &t1);

    mpi_mod_mpi(&t0, &t0, &p);

    // compare t0 and t3
    qDebug() << "t0 " << mpiToString(t0);
    qDebug() << "t3 " << mpiToString(t3);
    if (mpi_cmp_mpi(&t0, &t3) == 0) return true;
    else return false;
}

QString ECC_BIG::pointsToText(QList<MyPoint *> *list)
{
    QString temp = "";
    for(int i=0; i < list->size(); i++){
        temp += pointToString(list->at(i));
    }
    return temp;
}

QString ECC_BIG::pointToString(MyPoint *p)
{
    for (int i=1; i < 38; i++){
        mpi counter; mpi_init(&counter);
        counter = stringToMPI(QString::number(i));
        MyPoint *p2 = encryptPointFast(base_point, counter);
        if (p2->equal(p)) return codeToChar(i).toUpper();
    }
    return "";
}
