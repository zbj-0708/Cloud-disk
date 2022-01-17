#include <QApplication>
#include<ckernel.h>
#include<QTextCodec>

#undef main
int main(int argc, char *argv[])
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    QApplication a(argc, argv);
    CKernel kernel;
    //login.show();

    return a.exec();
}
