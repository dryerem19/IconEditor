#include "gridsettings.h"
#include "ui_gridsettings.h"

GridSettings::GridSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GridSettings)
{
    ui->setupUi(this);
}

GridSettings::~GridSettings()
{
    delete ui;
}
