#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QTextCodec>
#include <QDesktopServices>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->hide();
    Qt::WindowFlags flags = 0;
    setWindowFlags(flags);
    setFixedSize(500, 350);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openFileButton_clicked()
{
    QString selectedFilter;
    QFileDialog::Options options;
    fileNameList = QFileDialog::getOpenFileNames(
                                    this,                     // 设置当前对话框
                                    tr("打开vtt文件"),          // 设置对话框标题
                                    "",                       // 设置打开文件路径
                                    tr("Text files (*.vtt)"), // 设置打开文件类型
                                    &selectedFilter,
                                    options
                                );
    if (fileNameList.count()) {
        ui->fileListWidget->addItems(fileNameList); //返回文件列表的名称
    }
}

void MainWindow::on_convertButton_clicked()
{
    // 设置字符编码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QString currentFileName;
    ui->progressBar->setRange(0,fileNameList.size()-1);// 设置进度条范围
    int progressIndex = 0;
    for(QStringList::iterator it = fileNameList.begin();it!=fileNameList.end();++it){
        currentFileName = *it;
        // 读取文件内容
        QFile sourceFile(currentFileName);
        if(sourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream txtInput(&sourceFile);
            QStringList lineStrList; // 存储文件内容

            while(!txtInput.atEnd())
            {
               lineStrList.append(txtInput.readLine());
            }
            sourceFile.close(); // 关闭文件

            // 写入文件内容
            QString targetFileName = currentFileName.replace(currentFileName.lastIndexOf(".")+1, 3, "srt");
            QFile targetFile(targetFileName);
            if(targetFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream txtOutput(&targetFile);

                int count = 0;
                for(QStringList::iterator iter = lineStrList.begin();iter!=lineStrList.end();++iter){
                    if(count == 0 || count == 1){
                        count++;
                        continue;
                    }else{
                        txtOutput << *iter << endl;
                    }
                }
                targetFile.close();
            }
        }
        progressIndex++;
        ui->progressBar->setValue(progressIndex);
    }
    QMessageBox::about(NULL,"提示","转换完成!");
    // 打开转换目录
    QString convertDir = currentFileName.left(currentFileName.lastIndexOf("/")+1);
    QDesktopServices::openUrl(QUrl(convertDir, QUrl::TolerantMode));
}
