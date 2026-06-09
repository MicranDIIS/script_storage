#include "se2.h"
#include "ui_se2.h"

SE2::SE2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SE2)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));

    ui->setupUi(this);

    bReloadScript = false;
    isRunFromConsole_ = false;
    isNewScr = false;
    isEditScr = false;

    pl = 0;
    w = 0;

    path = QApplication::applicationDirPath()+"/scripts";
    specific_ = "";
    category_ = "";
    scriptPath_ = "";

    ui->cbCategories->clear();
//    FindScripts(path);
     updateLists();

    QSettings loadState("Micran","SEv2");
    loadState.beginGroup("MainWinState");
    int curr_index = loadState.value("cbCurrInd",0).toInt();
    if(curr_index<0)
	curr_index = 0;
    on_cbCategories_currentIndexChanged(curr_index);
    ui->cbCategories->setCurrentIndex(curr_index);
    ui->lwOperations->setCurrentRow(loadState.value("lwCurrRow",0).toInt());
    loadState.endGroup();

    ui->lwOperations->setResizeMode(QListView::Adjust);
    ui->lwOperations->installEventFilter(this);
    ui->lwOperations->setFocus();

    connect(this,SIGNAL(reOpenScript(QListWidgetItem*)),SLOT(on_lwOperations_itemDoubleClicked(QListWidgetItem*)),Qt::QueuedConnection);

}

SE2::~SE2()
{
    QSettings saveState("Micran","SEv2");
    saveState.beginGroup("MainWinState");
    saveState.setValue("cbCurrInd",ui->cbCategories->currentIndex());
    saveState.setValue("lwCurrRow",ui->lwOperations->currentRow());
    saveState.endGroup();
    delete ui;
}

QObject* SE2::getObject(QString name_obj)
{
    QObjectList children = ui->centralWidget->children();
    foreach(QObject * child, children) {
        QWidget *childWidget = qobject_cast<QWidget*> (child);
        if (childWidget) {
            if(QString::compare(childWidget->objectName(),name_obj,Qt::CaseInsensitive))
                return childWidget;
        }
    }
    return 0;
}

void SE2::myshow()
{
    qDebug()<<"myshow";
    qApp->setQuitOnLastWindowClosed(true);
    qApp->processEvents();
    qDebug()<<"show";
//    pl->deleteLater();
    if(ui->cbCategories->currentText().isEmpty() && ui->cbCategories->count())
	ui->cbCategories->setCurrentIndex(0);
    if (isRunFromConsole_ && !bReloadScript) {
        this->close();
    }
    else {
        this->show();
        this->raise();
        this->activateWindow();
        if(bReloadScript){
            bReloadScript = false;
            isNewScr = false;
	   // on_lwOperations_itemDoubleClicked(ui->lwOperations->currentItem());
	    reOpenScript(ui->lwOperations->currentItem());
        }
        else{
            updateLists();
        }
    }
}

void SE2::updateLists()
{
    isNewScr = false;
    int cbInd = ui->cbCategories->currentIndex();
    if(cbInd<0) cbInd = 0;
    int lwRow = ui->lwOperations->currentRow();
    ui->cbCategories->clear();
    ScriptsList.clear();
    FindScripts(path);
    if(ui->cbCategories->count()>=cbInd)
    {
        on_cbCategories_currentIndexChanged(cbInd);
        ui->cbCategories->setCurrentIndex(cbInd);
        ui->lwOperations->setFocus();
    }
    if(ui->lwOperations->count()>=lwRow)
        ui->lwOperations->setCurrentRow(lwRow);

    ui->pbLaunch->setEnabled(ui->cbCategories->count());
    ui->tbEditScript->setEnabled(ui->lwOperations->count());
    ui->tbDelScript->setEnabled(ui->lwOperations->count());

#ifdef DEBUG
    qDebug()<<"SE2:UPDATE:CATEGORY CURRENT INDEX:"<<ui->cbCategories->currentIndex();
#endif
}

void SE2::FindScripts(QString path)
{
    QDir d(path);
    QStringList filter;
    QString SExt = ".lua";
    filter << "*.lua";

    d.setNameFilters(filter);
    QFileInfoList sList = d.entryInfoList(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);
    QStringList eList = d.entryList(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);

    for (int j = 0; j < sList.count(); j++){
        if(sList[j].isDir()){
            QString ap = sList.at(j).absoluteFilePath();
            FindScripts(ap);
        }
        QString specific;
        QString category;

        if (getScriptHeader(path + "/" + eList.at(j), specific, category)){
            bool Has = false;
            for(int i=0;i< ui->cbCategories->count();i++){
                if(ui->cbCategories->itemText(i) == category){
                    Has = true;
                    ScriptsList[i].insert(specific,path +"/"+ eList.at(j));
                    break;
                }
            }
            if(!Has){
                QMap<QString,QString> tempcell;
                tempcell[specific] = path + "/" +eList.at(j);
                ScriptsList.push_back(tempcell);
                ui->cbCategories->addItem(category);
            }
        }
    }
    return;
}

bool SE2::getScriptHeader(QString path, QString &specific, QString &category)
{
    bool result = true;
//    recodeFile(path);
    QFile f(path);

    if (f.open(QIODevice::ReadOnly)) // пробуем открыть файл
    {
        QTextStream in(&f); // добавляем файл в текстовый поток
        QString line = in.readLine(); // читаем построчно
        if (line.contains("--Specific:")){ // если есть строка --Specific: - то это наш клиент
            QString TempName, TempGroup;
            int start = line.indexOf("--Specific:")+11;
            TempName = line.mid(start,line.indexOf(";")-start).trimmed(); //Name

            if(line.contains("Categories:")){
                start = line.indexOf("Categories:")+11;
                TempGroup = line.mid(start,line.lastIndexOf(";")-start).trimmed(); //Categories
                if(TempGroup=="") TempGroup=tr("Other");

            }
            else
                TempGroup=tr("Other");
            specific = TempName;
            category = TempGroup;
        } else {
            result =  false;
        }
    } else {
        result =  false;
    }
    return result;
}

void SE2::runScript()
{
    if ((!specific_.isEmpty()) && (!category_.isEmpty()))
    {
        loadScript(scriptPath_,specific_);
    }
    isRunFromConsole_ = true;
}

void SE2::on_cbCategories_currentIndexChanged(int index)
{
    if(index>=0){
        ui->lwOperations->clear();
        if(ScriptsList.size()>0 && (( ScriptsList.size() - 1)>=index))
            ui->lwOperations->addItems(ScriptsList[index].keys());
        if(ui->lwOperations->sizeHintForColumn(0)>ui->lwOperations->size().width()){
            setMaximumWidth(ui->lwOperations->sizeHintForColumn(0)+55);
            resize(ui->lwOperations->sizeHintForColumn(0)+55,size().height());
        }
        ui->lwOperations->setCurrentRow(0);
    }
}

void SE2::on_lwOperations_itemDoubleClicked(QListWidgetItem *item)
{
    qDebug()<<"open script"<<item->text();
    QString path = ScriptsList[ui->cbCategories->currentIndex()].value(item->text());
    QString header = item->text();
    loadScript(path, header);
}

bool SE2::loadScript(QString path, QString header)
{
    bool result = true;
    QDir dir(qApp->applicationDirPath());
    dir.cd("plugins");
    QString baseWinPath = QApplication::applicationDirPath() + "/plugins/BaseWin.dll";
    QPluginLoader loader(baseWinPath);
    pl = loader.instance();
    PluginInterface *pi=qobject_cast<PluginInterface *>(pl);
    if (!pi){
	QMessageBox::critical(this,tr("Error BasePlugin.dll"),loader.errorString());
        result = false;
        return result;
    }
    pi->SetPath(path+";"+ui->cbCategories->currentText());
    pi->startThread();

    w = pi->GetPtrForm();

    if(w){
	connect(w,SIGNAL(ReloadScript()),this,SLOT(ReloadScript()),Qt::QueuedConnection);
	connect(w,SIGNAL(scriptStops()),this,SLOT(checkClosingBaseWin()),Qt::QueuedConnection);
	connect(w,SIGNAL(destroyed()),this,SLOT(myshow()),Qt::QueuedConnection);

        if(isNewScr || isEditScr)
            pi->setMode(true);
        else
            w->show();

        w->setWindowTitle(header);
        w->setAttribute(Qt::WA_DeleteOnClose);
        qApp->setQuitOnLastWindowClosed(false);
        this->hide();

    } else {
        result = false;
    }
    return result;
}

void SE2::checkClosingBaseWin()
{
//    if (isRunFromConsole_)
//	qDebug()<<"w closing !!";
//        w->close();
}

bool SE2::eventFilter(QObject *obj, QEvent *e)
{
    //Отлавливаем событие нажатия клавиши Esc
    if(e->type() == QEvent::KeyPress){
        QKeyEvent * ke = static_cast<QKeyEvent*>(e);
        if(ke->key()== Qt::Key_Enter || ke->key()== Qt::Key_Return){
            on_lwOperations_itemDoubleClicked(ui->lwOperations->currentItem());
        }
    }
    return  QWidget::eventFilter(obj, e);//Вызываем стандартный обработчик событий
}

void SE2::on_pbLaunch_clicked()
{
    on_lwOperations_itemDoubleClicked(ui->lwOperations->currentItem());
}

void SE2::ReloadScript()
{
    qDebug()<<"reload slot";
    bReloadScript = true;
    w->close();
}

void SE2::on_tbNewScript_clicked()
{
    isNewScr = true;
    loadScript(qApp->applicationDirPath()+"/scripts/_pattern.lu","");

#ifdef DEBUG
    qDebug()<<"SE2:NEW:CATEGORY CURRENT INDEX:"<<ui->cbCategories->currentIndex();
#endif
}

void SE2::on_tbEditScript_clicked()
{
    isNewScr = true;
    on_lwOperations_itemDoubleClicked(ui->lwOperations->currentItem());

#ifdef DEBUG
    qDebug()<<"SE2:EDIT:CATEGORY CURRENT INDEX:"<<ui->cbCategories->currentIndex();
#endif
}

void SE2::on_tbDelScript_clicked()
{
    int ret = QMessageBox::warning(this, tr("SE2"),
                                   tr("Do you really want to delete this script?"),
                                   QMessageBox::Yes | QMessageBox::Default,
                                   QMessageBox::No,
                                   QMessageBox::Cancel | QMessageBox::Escape);
    if (ret == QMessageBox::Yes){
        QFile file(ScriptsList[ui->cbCategories->currentIndex()].value(ui->lwOperations->currentItem()->text()));
        file.remove();
        updateLists();
    }
    else if (ret == QMessageBox::Cancel)
        return;
}
void SE2::recodeFile(QString path)
    {
	QFile file(path);
	if (!file.open(QFile::ReadOnly)) {
	    return;
	}
	QByteArray txt= file.readAll();
	file.close();
	QTextCodec::ConverterState state;
	QTextCodec *codec = QTextCodec::codecForName("utf-8");
	QString text = codec->toUnicode(txt.constData(), txt.size(), &state);
	if (state.invalidChars > 0)
	{
	    // Not a UTF-8 text - using system default locale
	    QTextCodec * codecW = QTextCodec::codecForName("windows-1251");//QTextCodec::codecForLocale();
	    if (!codecW)
		return;

	    text = codecW->toUnicode(txt);
	    if (!file.open(QFile::WriteOnly)) {
		return;
	    }
	    QTextStream out(&file);
	    out << text;
	    file.close();
	}
    }
