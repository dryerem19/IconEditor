#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QPushButton>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    createConnections();
    translateUi();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::addItem(QWidget *widget)
{
    if (!widget) {
        return;
    }

    auto layout = widget->layout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto item = new QListWidgetItem(widget->windowIcon(), widget->windowTitle());
    ui->itemList->addItem(item);
    ui->widgetsStack->addWidget(widget);

    if (ui->itemList->count() == 1) {
        ui->itemList->setCurrentRow(0);
    }
}

void SettingsDialog::onOkClicked()
{

}

void SettingsDialog::onApplyClicked()
{

}

void SettingsDialog::onCancelClicked()
{

}

void SettingsDialog::onGroupSelectionChanged(int currentRow)
{
    if (currentRow >= 0) {
        ui->widgetsStack->setCurrentIndex(currentRow);
        ui->currentSettingsNameLabel->setText(ui->widgetsStack->currentWidget()->windowTitle());
    }
}

void SettingsDialog::createConnections()
{
    connect(ui->itemList, &QListWidget::currentRowChanged, this, &SettingsDialog::onGroupSelectionChanged);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::pressed, this, &SettingsDialog::onOkClicked);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::pressed, this, &SettingsDialog::onApplyClicked);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::pressed, this, &SettingsDialog::onCancelClicked);
}

void SettingsDialog::translateUi()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    ui->buttonBox->button(QDialogButtonBox::Apply)->setText(tr("Применить"));
}
