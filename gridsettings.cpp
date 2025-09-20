#include "gridsettings.h"
#include "ui_gridsettings.h"

GridSettings::GridSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GridSettings)
{
    ui->setupUi(this);
    createConnections();
}

GridSettings::~GridSettings()
{
    delete ui;
}

void GridSettings::createConnections()
{
    // auto colorButtons = findChildren<ColorButton*>();
    // foreach (auto button, colorButtons) {
    //     connect(button, &ColorButton::clicked, this, &GridSettings::onColorButtonClicked);
    // }
}
