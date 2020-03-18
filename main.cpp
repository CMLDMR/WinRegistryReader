#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include <QJsonValue>

#include <QProcess>
#include <intrin.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <windows.h>
#include <winioctl.h>


#include <iostream>


static void native_cpuid(unsigned int *eax, unsigned int *ebx,
                 unsigned int *ecx, unsigned int *edx)
{
    /* ecx is often an input as well as an output. */
    asm volatile("cpuid"
        : "=a" (*eax),
        "=b" (*ebx),
        "=c" (*ecx),
        "=d" (*edx)
        : "0" (*eax), "2" (*ecx)
        : "memory");
}



/**
 * @brief getHDDSerials
 * @param cmd
 * @return QJSONVALUE:
 *
 * {
 *  { "volumeName":"C" }
 *  { "size":240054693888 }
 *  { "free":356118077440 }
 *  { }
 * }
 */

std::string getHDDSize(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) throw std::runtime_error("_popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }

    auto list = QString::fromStdString (result).simplified ().split (" ");



    for( int i = 0 ; i < list.count () ; i += 3 )
    {
        if( i != 0 )
        {
            qDebug() << i << "Disk:"<<list[i] << "FreeSpace: "<<list[i+1].toDouble ()/(1024*1024*1024)<< "GB  Kapasite: "<<list[i+2].toDouble ()/(1024*1024*1024) << " GB";
        }
    }
    return result;
}


int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    QSettings m("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System",QSettings::NativeFormat);

    auto str = getHDDSize("wmic path win32_physicalmedia get SerialNumber");
    qDebug() << str.c_str () ;


    str = getHDDSize("wmic logicaldisk get size,freespace,caption");

    QStringList list = QString::fromStdString (str).simplified ().split (" ");

    for( int i = 0 ; i < list.count () ; i += 3 )
    {
        qDebug() << i << "Disk:"<<list[i] << "FreeSpace: "<<list[i+1].toDouble ()/(1024*1024*1024)<< "GB  Kapasite: "<<list[i+2].toDouble ()/(1024*1024*1024) << " GB";
    }

    qDebug() << str.c_str () ;


    auto val = m.value("BIOS/BIOSVendor").toString ();
    qDebug()<<val;


    // Reading Keys
    qDebug() << m.childKeys ();

    // Reading Groups
    qDebug() << m.childGroups ();

    // Reading Value
    val = m.value("BIOS/BiosMajorRelease").toString ();
    qDebug()<<val;


    unsigned int eax, ebx, ecx, edx;
    eax = 0;
    native_cpuid(&eax, &ebx, &ecx, &edx);
    printf("EAX: %08X EBX: %08X ECX: %08X EDX: %08X\n", eax, ebx, ecx, edx);
    char vendor[13];
    memcpy(vendor, &ebx, 4);
    memcpy(vendor+4, &edx, 4);
    memcpy(vendor+8, &ecx, 4);
    vendor[12] = '\0';

    qDebug() << vendor << edx << ecx ;
//    printf("%s\n", vendor);

    printf("serial number 0x%08x%08x\n", edx, ecx);





    return a.exec();

}



