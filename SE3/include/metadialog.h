#ifndef METADIALOG_H
#define METADIALOG_H

#include <QDialog>
#include "../include/sHead.h"

namespace Ui {
class MetaDialog;
}

class MetaDialog : public QDialog
{
    Q_OBJECT

signals:
    void updateParams();
    void updateParams(QString title, QString category, int lang);
//    void updateParams(stDecodeData);
    void done();

public:
    explicit MetaDialog(QWidget *parent = 0);
    void init(stDecodeData &inp_data, QByteArray curr_state, QByteArray curr_geom);
    void init(const QString &category);
    ~MetaDialog();

private slots:
    void on_buttonBox_accepted();
    void on_pbToDefault_clicked();
    void on_pbFromDefault_clicked();
    void on_buttonBox_rejected();

private:
    Ui::MetaDialog *ui;
    QString m_line;
    stDecodeData *curr_data;
    QByteArray actCurrState;
    QByteArray actCurrGeom;

    QByteArray newState;
    QByteArray newDefState;
    QByteArray newGeom;
    QByteArray newDefGeom;
    char newData[65535];
    bool mbNewScriptMode;
};

#endif // METADIALOG_H
