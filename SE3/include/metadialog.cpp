#include "metadialog.h"
#include "ui_metadialog.h"

MetaDialog::MetaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MetaDialog)
{
    ui->setupUi(this);
    setStyleSheet("QGroupBox{font:13px; font-weight:bold;}");
    QPushButton *applB= ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(applB, SIGNAL(clicked(bool)), this, SLOT(on_buttonBox_accepted()));
}

void MetaDialog::init(stDecodeData &inp_data, QByteArray curr_state, QByteArray curr_geom)
{
    mbNewScriptMode = false;
    ui->lLanguage->hide();
    ui->cbLanguage->hide();
    curr_data = &inp_data;
    ui->leTitle->setText(curr_data->title);
    ui->leCategory->setText(curr_data->category);
    actCurrState = curr_state;
    actCurrGeom = curr_geom;
}

void MetaDialog::init(const QString& category)
{
    mbNewScriptMode = true;
    setWindowTitle(tr("Create new script"));
    ui->leTitle->setPlaceholderText(tr("Input New Script Name"));
    ui->leCategory->setPlaceholderText(tr("Input New Script Category"));
    ui->leCategory->setText(category);
    ui->lLanguage->show();
    ui->cbLanguage->show();
    ui->gbGeomOper->setVisible(false);
}

MetaDialog::~MetaDialog()
{
    delete ui;
}

void MetaDialog::on_buttonBox_accepted()
{
    emit done();
    if(mbNewScriptMode)
        emit updateParams(ui->leTitle->text(),ui->leCategory->text(),ui->cbLanguage->currentIndex());
    else
    {
        curr_data->title = ui->leTitle->text();
        curr_data->category = ui->leCategory->text();
        curr_data->curr_geom = newGeom;
        curr_data->curr_state = newState;
        curr_data->def_geom = newDefGeom;
        curr_data->def_state= newDefState;
        emit updateParams();
    }
    qDebug()<<"emit done";

}

void MetaDialog::on_pbToDefault_clicked()
{
    ui->lGeomState->setText(tr("Save by Default"));
    newDefState = actCurrState;
    newState = actCurrState;
    newDefGeom = actCurrGeom;
    newGeom = actCurrGeom;
}

void MetaDialog::on_pbFromDefault_clicked()
{
    ui->lGeomState->setText(tr("Restore from Default"));
    newState = curr_data->def_state;
    newDefState = curr_data->def_state;
    newGeom = curr_data->def_geom;
    newDefGeom = curr_data->def_geom;
}

void MetaDialog::on_buttonBox_rejected()
{
    emit done();
}

