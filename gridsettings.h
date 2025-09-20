#ifndef GRIDSETTINGS_H
#define GRIDSETTINGS_H
#include "colorbutton.h"

namespace Ui {
class GridSettings;
}

class GridSettings : public QWidget
{
    Q_OBJECT

public:
    explicit GridSettings(QWidget *parent = nullptr);
    ~GridSettings();

private:
    Ui::GridSettings *ui;

    void createConnections();
};

#endif // GRIDSETTINGS_H
