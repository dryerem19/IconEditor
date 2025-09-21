#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSettings>
#include <QMainWindow>

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>

#include "settingsdialog.h"
#include "gridsettings.h"
#include "utils.h"

// TODO LIST:
// 1. Во время вставки изображения при прокрутке остается остаточное изображению превью (необходимо обновлять состояние)
// 2. Дать возможность закрывать вкладки
// 3. Точка привязки отоброжается после завершения копирования
// 4. Функция "Вырезать"
// 5. Во время вставки сделать авто прокрутку когда мышь выходит за края
// 6. Проверка на степень двойки

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

const int   MIN_ZOOM_PERCENTAGE = 10;
const int   MAX_ZOOM_PERCENTAGE = 500;
const int   DEF_ZOOM_PERCENTAGE = 100;

const float MAX_ZOOM_VALUE      = 5.0f;
const float MIN_ZOOM_VALUE      = 0.1f;
const float DEF_ZOOM_VALUE      = 1.0f;

struct ImageCellSelection {
    QGraphicsRectItem*  rect;
    QPoint              startCell;
    QPoint              endCell;
    bool                isSelecting;

    ImageCellSelection() : rect(nullptr), isSelecting(false) {}
};

struct ImageTabStruct {
    QGraphicsScene*             scene;
    QVector<QGraphicsLineItem*> gridLines;

    int imageWidth;
    int imageHeight;

    int cellSizeWidth;
    int cellSizeHeight;
    int gridColumns;
    int gridRows;

    int     currentZoomPercentage   = DEF_ZOOM_PERCENTAGE;
    float   currentZoomValue        = DEF_ZOOM_VALUE;

    QString filePath;
    QPixmap background;
    ImageCellSelection selection; // Информация о выделенных ячейках для этой вкладки

    QRectF getSelectionRect() const
    {
        int startX  = qMin(selection.startCell.x(), selection.endCell.x());
        int startY  = qMin(selection.startCell.y(), selection.endCell.y());
        int endX    = qMax(selection.startCell.x(), selection.endCell.x());
        int endY    = qMax(selection.startCell.y(), selection.endCell.y());

        return QRectF(
            startX * cellSizeWidth,
            startY * cellSizeHeight,
            (endX - startX + 1) * cellSizeWidth,
            (endY - startY + 1) * cellSizeHeight
        );
    }

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNewAtlas();
    void onLoadImage();
    void onSaveAtlas();
    void onActionSettings();
    void updateGrid();
    void snapToGrid(QGraphicsPixmapItem* item);

    void updateCellSizeWidth(int width);
    void updateCellSizeHeight(int height);

    void onOpenImage();

    void onIconSelectionChanged();

private:
    Ui::MainWindow *ui;
    QVector<QGraphicsPixmapItem*> icons;
    QVector<ImageTabStruct> imageTabs_;
    SettingsDialog* m_settingsDialog{nullptr};

    // Превью при вставке
    QGraphicsPixmapItem* pastePreviewItem_{nullptr};
    bool isPasting_{false};

    int currentZoom_{100};

    int gridWidthSize;
    int gridHeightSize;
    QString currentSavePath;

    QGraphicsView* currentGraphicsView() const;
    ImageTabStruct* currentTab();

    bool addIconToLibrary(const QString& path);
    bool showNewAtlasDialog(int& cellSize, int& columns, int& rows, QString& filePath);

    void setupConnections();

    void createGrid(ImageTabStruct& tab);
    void clearGrid(ImageTabStruct& tab);

    void onGraphicsViewMousePress(QMouseEvent* event, QGraphicsView* graphicsView);
    void onGraphicsViewMouseMove(QMouseEvent* event, QGraphicsView* graphicsView);
    void onGraphicsViewMouseRelease(QMouseEvent* event);
    void updateSelectionRect(ImageTabStruct* tab);
    void clearSelection(ImageTabStruct* tab);
    void clearSelection();

    void showContextMenu(const QPoint& pos);
    void onClearSelection();
    void onCopySelection();
    void onSelectionInfo();

    void onPaste();
    void cancelPaste();
    void commitPaste(const QPointF& scenePos);

    // Функции зума
    void onZoom(int value);

    // Обновляет фоновое изображение на новое
    void updateBackgroundImage(const QImage& newImage);

    // Блок индикатора прилипания
private:
    QPointF                 neighboringPeak_{0.0f, 0.0f};
    int                     snapIndicatorDiameter{8};
    bool                    snappingEnabled_{true};
    QGraphicsEllipseItem*   snapIndicator_{nullptr};

    // Обновляет индикатор прилипания
    void updateSnapIndicator(const QPointF& mousePos, ImageTabStruct* tab);

    // Возвращает ближайшую вершину
    QPointF getNeighboringPeak(const QPointF& mousePos, ImageTabStruct* tab);

private:
    void onTabChanged(int index);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void keyPressEvent(QKeyEvent *event);
};
#endif // MAINWINDOW_H
