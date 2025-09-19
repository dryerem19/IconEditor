#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QClipboard>
#include <QMouseEvent>

#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QListWidgetItem>

#include <QMessageBox>
#include <QFileDialog>
#include <QDialogButtonBox>

#include <QGraphicsView>

constexpr int FIXED_GRID_COLS = 512;
constexpr int FIXED_GRID_ROWS = 512;

constexpr int MAIN_SCENE_POS    = -1;
constexpr int PREV_SCENE_POS    = 5;
constexpr int SNAP_SCENE_POS    = 20;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qApp->installEventFilter(this);

    m_settingsDialog = new SettingsDialog(this);
    m_settingsDialog->addItem(new GridSettings(m_settingsDialog));

    // Включаем обработку горячих клавиш
    setFocusPolicy(Qt::StrongFocus);

    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);

    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewAtlas()
{
    int newCellSize, newColumns, newRows;
    QString path;

    if (!showNewAtlasDialog(newCellSize, newColumns, newRows, path)) {
        return;
    }

    // currentSavePath = path;
    // cellSize        = newCellSize;
    // gridWidthSize     = newColumns;
    // gridHeightSize        = newRows;

    // setWindowTitle("Icon Editor - " + QFileInfo(path).fileName());

    // ui->spinCellSize->setValue(cellSize);
    // // ui->spinGridColumns->setValue(gridColumns);
    // // ui->spinGridRows->setValue(gridRows);

    // createGrid();

    // int atlasWidth  = gridWidthSize * cellSize;
    // int atlasHeight = gridHeightSize * cellSize;
    // QImage atlas(atlasWidth, atlasHeight, QImage::Format_ARGB32);
    // atlas.fill(Qt::transparent);
}

void MainWindow::onLoadImage()
{
    QStringList paths = QFileDialog::getOpenFileNames(this, tr("Выберите PNG иконки"), "", tr("PNG Images (*.png)"));
    if (paths.isEmpty()) {
        return;
    }

    int loadedCount = 0;
    foreach (const QString &path, paths) {
        if (addIconToLibrary(path)) {
            loadedCount++;
        }
    }

    if (loadedCount > 0) {
        QMessageBox::information(this, tr("Успех"), QString("Загружено %1 иконок в библиотеку").arg(loadedCount));
    }
}

void MainWindow::onSaveAtlas()
{

}

void MainWindow::onActionSettings()
{
    if (m_settingsDialog->exec() == QDialog::Accepted)
    {
    }
}

void MainWindow::updateGrid()
{

}

void MainWindow::snapToGrid(QGraphicsPixmapItem *item)
{

}

void MainWindow::updateCellSizeWidth(int width)
{
    ImageTabStruct* tab = currentTab();
    if (!tab) {
        return;
    }

    tab->cellSizeWidth  = width;
    tab->gridColumns    = tab->imageWidth / tab->cellSizeWidth;

    createGrid(*tab);
}

void MainWindow::updateCellSizeHeight(int height)
{
    ImageTabStruct* tab = currentTab();
    if (!tab) {
        return;
    }

    tab->cellSizeHeight = height;
    tab->gridRows       = tab->imageHeight / tab->cellSizeHeight;

    createGrid(*tab);
}

void MainWindow::onOpenImage()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Открыть изображение"), "", tr("PNG Images (*.png)"));
    if (path.isEmpty()) return;

    QImage image(path);
    if (image.isNull()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось загрузить изображение!"));
        return;
    }

    // Создаем новую вкладку
    ImageTabStruct newTab;
    newTab.scene                    = new QGraphicsScene(this);
    newTab.cellSizeWidth            = ui->spinCellSizeWidth->value();
    newTab.cellSizeHeight           = ui->spinCellSizeHeight->value();
    newTab.gridColumns              = image.width() / newTab.cellSizeWidth;
    newTab.gridRows                 = image.height() / newTab.cellSizeHeight;
    newTab.filePath                 = path;
    newTab.background               = QPixmap::fromImage(image);
    newTab.imageWidth               = image.width();
    newTab.imageHeight              = image.height();

    // Загружаем изображение на сцену
    QGraphicsPixmapItem *background = newTab.scene->addPixmap(newTab.background);
    background->setZValue(MAIN_SCENE_POS);

    // Создаем QGraphicsView для вкладки
    QGraphicsView *graphicsView     = new QGraphicsView(newTab.scene);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsView->setBackgroundBrush(Qt::black);

    // Добавляем вкладку
    int tabIndex = ui->tabWidget->addTab(graphicsView, QFileInfo(path).fileName());
    imageTabs_.append(newTab);

    // Создаем сетку
    createGrid(imageTabs_.last());

    // Переключаемся на новую вкладку
    ui->tabWidget->setCurrentIndex(tabIndex);
}

void MainWindow::onIconSelectionChanged()
{
    QList<QListWidgetItem*> selectedItems = ui->iconLibraryListWidget->selectedItems();

    if (selectedItems.isEmpty()) {
        ui->iconLibraryPreviewLabel->setText("Выберите иконку");
        ui->iconLibraryPreviewLabel->setPixmap(QPixmap());
        return;
    }

    QListWidgetItem *selectedItem = selectedItems.first();
    QString path = selectedItem->data(Qt::UserRole).toString();

    QPixmap originalPixmap(path);
    if (originalPixmap.isNull()) {
        ui->iconLibraryPreviewLabel->setText("Ошибка загрузки\n" + path);
        return;
    }

    QPixmap previewPixmap = originalPixmap.scaled(
        ui->iconLibraryPreviewLabel->width(),
        ui->iconLibraryPreviewLabel->height(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );

    ui->iconLibraryPreviewLabel->setPixmap(previewPixmap);
    ui->iconLibraryPreviewLabel->setToolTip(QString("Размер: %1x%2 px\n%3")
                                     .arg(originalPixmap.width())
                                     .arg(originalPixmap.height())
                                     .arg(path));
}

QGraphicsView *MainWindow::currentGraphicsView() const
{
    int currentIndex = ui->tabWidget->currentIndex();
    if (currentIndex >= 0) {
        return qobject_cast<QGraphicsView*>(ui->tabWidget->widget(currentIndex));
    }
    return nullptr;
}

ImageTabStruct *MainWindow::currentTab()
{
    int currentIndex = ui->tabWidget->currentIndex();
    if (currentIndex >= 0 && currentIndex < imageTabs_.size()) {
        return &imageTabs_[currentIndex];
    }

    return nullptr;
}

bool MainWindow::addIconToLibrary(const QString &path)
{
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        QMessageBox::warning(this, tr("Ошибка"), QString("Не удалось загрузить изображение: %1").arg(path));
        return false;
    }

    // Проверяем, нет ли уже такой иконки в библиотеке
    for (int i = 0; i < ui->iconLibraryListWidget->count(); ++i) {
        if (ui->iconLibraryListWidget->item(i)->data(Qt::UserRole).toString() == path) {
            return false;
        }
    }

    // Создаем thumbnail
    QPixmap thumbnail = pixmap.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Создаем элемент списка
    QListWidgetItem *item = new QListWidgetItem(QIcon(thumbnail), QFileInfo(path).fileName());
    item->setData(Qt::UserRole, path);
    item->setToolTip(path);
    item->setStatusTip(path);

    ui->iconLibraryListWidget->addItem(item);
    return true;
}

bool MainWindow::showNewAtlasDialog(int &cellSize, int &columns, int &rows, QString &filePath)
{
    /*
    QDialog dialog(this);
    dialog.setWindowTitle("Создать новый атлас");

    dialog.setMinimumSize(400, 150);
    dialog.setMaximumSize(400, 150);

    QFormLayout form(&dialog);

    QSpinBox *widthSpin = new QSpinBox(&dialog);
    widthSpin->setRange(FIXED_CELL_SIZE, 4096);
    widthSpin->setSuffix(" px");
    widthSpin->setValue(512);

    QSpinBox *heightSpin = new QSpinBox(&dialog);
    heightSpin->setRange(FIXED_CELL_SIZE, 4096);
    heightSpin->setSuffix(" px");
    heightSpin->setValue(512);

    QLineEdit *pathEdit = new QLineEdit(&dialog);
    QPushButton *browseButton = new QPushButton(tr("Обзор..."), &dialog);

    connect(browseButton, &QPushButton::clicked, this, [pathEdit, &dialog]() {
        QString path = QFileDialog::getSaveFileName(&dialog, tr("Сохранить атлас как"), "", tr("PNG Images (*.png)"));
        if (!path.isEmpty()) {
            pathEdit->setText(path);
        }
    });

    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(browseButton);

    form.addRow(tr("Ширина атласа:"), widthSpin);
    form.addRow(tr("Высота атласа:"), heightSpin);
    form.addRow(tr("Путь сохранения:"), pathLayout);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    bool accepted = false;
    do {
        if (dialog.exec() == QDialog::Accepted) {
            if (pathEdit->text().isEmpty()) {
                QMessageBox::warning(&dialog, "Ошибка", "Не указан путь для сохранения!");
                continue;
            }

            int atlasWidth  = widthSpin->value();
            int atlasHeight = heightSpin->value();

            cellSize        = FIXED_CELL_SIZE;
            columns         = atlasWidth / cellSize;
            rows            = atlasHeight / cellSize;
            filePath        = pathEdit->text();

            accepted        = true;
        } else {
            break;
        };
    } while (!accepted);
    return accepted;
*/
}

void MainWindow::setupConnections()
{
    connect(ui->actionNovo, &QAction::triggered, this, &MainWindow::onNewAtlas);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::onLoadImage);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSaveAtlas);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenImage);

    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::onActionSettings);

    // Размеры сетки
    connect(ui->spinCellSizeWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateCellSizeWidth);
    connect(ui->spinCellSizeHeight, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateCellSizeHeight);

    // Контекстное меню
    ui->tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tabWidget, &QTabWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

    // Зум
    connect(ui->spinZoom, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onZoom);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    connect(ui->iconLibraryListWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::onIconSelectionChanged);
}

void MainWindow::createGrid(ImageTabStruct &tab)
{
    clearSelection(&tab);
    clearGrid(tab);

    int sceneWidth  = tab.imageWidth;
    int sceneHeight = tab.imageHeight;

    QPen gridPen(Qt::red);
    gridPen.setWidth(1);

    // Вертикальные линии (столбцы)
    for (int col = 0; col <= tab.gridColumns; ++col) {
        int x = col * tab.cellSizeWidth;
        if (x <= sceneWidth) { // Проверяем, чтобы не выходить за границы изображения
            QGraphicsLineItem *line = tab.scene->addLine(x, 0, x, sceneHeight, gridPen);
            tab.gridLines.append(line);
        }
    }

    // Горизонтальные линии (строки)
    for (int row = 0; row <= tab.gridRows; ++row) {
        int y = row * tab.cellSizeHeight;
        if (y <= sceneHeight) { // Проверяем, чтобы не выходить за границы изображения
            QGraphicsLineItem *line = tab.scene->addLine(0, y, sceneWidth, y, gridPen);
            tab.gridLines.append(line);
        }
    }

    // Установка размера сцены по размеру изображения
    tab.scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
}

void MainWindow::clearGrid(ImageTabStruct &tab)
{
    for (int i = 0; i < tab.gridLines.size(); i++) {
        QGraphicsLineItem *line = tab.gridLines[i];
        if (line && line->scene() == tab.scene)
        {
            tab.scene->removeItem(line);
            delete line;
        }
    }

    tab.gridLines.clear();
}

void MainWindow::onGraphicsViewMousePress(QMouseEvent *event, QGraphicsView *graphicsView)
{
    ImageTabStruct* tab = currentTab();
    if (!tab || !graphicsView) {
        return;
    }

    if (isPasting_ && event->button() == Qt::LeftButton)
    {
        QPointF scenePos = graphicsView->mapToScene(event->pos());
        commitPaste(scenePos);

        isPasting_ = false;

        // Восстанавливаем обычный курсор
        graphicsView->setCursor(Qt::ArrowCursor);
    }
    else
    {
        if (event->button() != Qt::LeftButton) {
            return;
        }

        QPointF sceneCursorPos  = graphicsView->mapToScene(event->pos());
        int cellX               = qFloor(sceneCursorPos.x() / tab->cellSizeWidth);
        int cellY               = qFloor(sceneCursorPos.y() / tab->cellSizeHeight);

        // Проверяем границы
        if (cellX >= 0 && cellX < tab->gridColumns &&
            cellY >= 0 && cellY < tab->gridRows)
        {
            tab->selection.startCell    = QPoint(cellX, cellY);
            tab->selection.endCell      = tab->selection.startCell;
            tab->selection.isSelecting  = true;

            updateSelectionRect(tab);
        }
    }
}

void MainWindow::onGraphicsViewMouseMove(QMouseEvent *event, QGraphicsView *graphicsView)
{
    ImageTabStruct* tab = currentTab();
    if (!tab || !graphicsView) {
        return;
    }



    if (isPasting_ && pastePreviewItem_)
    {
        // Позиционируем preview под курсором
        QPointF scenePos = graphicsView->mapToScene(event->pos());
        pastePreviewItem_->setPos(scenePos);
    }
    else
    {
        if (!tab->selection.isSelecting) {
            return;
        }

        QPointF sceneCursorPos  = graphicsView->mapToScene(event->pos());
        int cellX               = qFloor(sceneCursorPos.x() / tab->cellSizeWidth);
        int cellY               = qFloor(sceneCursorPos.y() / tab->cellSizeHeight);

        // Проверяем границы
        cellX = qBound(0, cellX, tab->gridColumns - 1);
        cellY = qBound(0, cellY, tab->gridRows - 1);

        if (tab->selection.endCell != QPoint(cellX, cellY)) {
            tab->selection.endCell = QPoint(cellX, cellY);
            updateSelectionRect(tab);
        }
    }

    // Обновляем прилипание
    QPointF sceneMousePos = graphicsView->mapToScene(event->pos());
    updateSnapIndicator(sceneMousePos, tab);
}

void MainWindow::onGraphicsViewMouseRelease(QMouseEvent *event)
{
    ImageTabStruct* tab = currentTab();
    if (!tab || event->button() != Qt::LeftButton) {
        return;
    }

    tab->selection.isSelecting = false;
}

void MainWindow::updateSelectionRect(ImageTabStruct* tab)
{
    if (!tab) {
        return;
    }

    // Удаляем старый прямоугольник выделения
    if (tab->selection.rect) {
        tab->scene->removeItem(tab->selection.rect);
        delete tab->selection.rect;
        tab->selection.rect = nullptr;
    }

    tab->selection.rect = tab->scene->addRect(tab->getSelectionRect(), QPen(Qt::blue, 2), QBrush(QColor(0, 0, 255, 50)));
    tab->selection.rect->setZValue(10); // Поверх всех элементов
}

void MainWindow::clearSelection(ImageTabStruct *tab)
{
    if (!tab) {
        return;
    }

    if (tab->selection.rect) {
        tab->scene->removeItem(tab->selection.rect);
        delete tab->selection.rect;
        tab->selection.rect = nullptr;
    }

    tab->selection.startCell    = QPoint(-1, -1);
    tab->selection.endCell      = QPoint(-1, -1);
    tab->selection.isSelecting  = false;
}

void MainWindow::clearSelection()
{
    ImageTabStruct* tab = currentTab();
    if (!tab) {
        return;
    }

    clearSelection(tab);
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    ImageTabStruct* tab = currentTab();
    if (!tab) {
        return;
    }

    // Проверяем, есть ли выделение
    if (tab->selection.startCell.x() == -1 || tab->selection.endCell.x() == -1) {
        return;
    }

    QMenu contextMenu(this);

    // Добавляем пункты меню
    QAction* copysAction = contextMenu.addAction(tr("Копировать"));
    QAction* pasteAction = contextMenu.addAction(tr("Вставить"));
    QAction* clearAction = contextMenu.addAction(tr("Очистить"));

    pasteAction->setEnabled(false);
    if (tab->selection.startCell.x() != -1) {
        pasteAction->setEnabled(true);
    }

    contextMenu.addSeparator();
    QAction* infoAction = contextMenu.addAction(tr("Информация о выделении"));

    connect(copysAction, &QAction::triggered, this, &MainWindow::onCopySelection);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::onPaste);
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClearSelection);
    connect(infoAction, &QAction::triggered, this, &MainWindow::onSelectionInfo);

    // Показываем меню в позиции курсора
    contextMenu.exec(ui->tabWidget->mapToGlobal(pos));
}

void MainWindow::onClearSelection()
{
    ImageTabStruct* tab = currentTab();
    if (!tab) {
        return;
    }

    // Вычисляем область выделения
    int startX          = qMin(tab->selection.startCell.x(), tab->selection.endCell.x());
    int startY          = qMin(tab->selection.startCell.y(), tab->selection.endCell.y());
    int endX            = qMax(tab->selection.startCell.x(), tab->selection.endCell.x());
    int endY            = qMax(tab->selection.startCell.y(), tab->selection.endCell.y());

    // Преобразуем ячейки в пиксельные координаты
    int pixelStartX     = startX * tab->cellSizeWidth;
    int pixelStartY     = startY * tab->cellSizeHeight;
    int pixelEndX       = (endX + 1) * tab->cellSizeWidth;
    int pixelEndY       = (endY + 1) * tab->cellSizeHeight;

    // Проверяем границы изображения
    pixelEndX           = qMin(pixelEndX, tab->imageWidth);
    pixelEndY           = qMin(pixelEndY, tab->imageHeight);

    int width           = pixelEndX - pixelStartX;
    int height          = pixelEndY - pixelStartY;

    if (width <= 0 || height <= 0) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Некорректная область выделения"));
        return;
    }

    // Создаем временное изображение для очистки области
    QImage updatedImage = tab->background.toImage();

    // Очищаем выделенную область
    QPainter painter(&updatedImage);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(pixelStartX, pixelStartY, width, height, Qt::transparent);
    painter.end();

    updateBackgroundImage(updatedImage);
}

void MainWindow::onCopySelection()
{
    ImageTabStruct* tab = currentTab();
    if (!tab) {
        return;
    }

    // Вычисляем область выделения
    int startX          = qMin(tab->selection.startCell.x(), tab->selection.endCell.x());
    int startY          = qMin(tab->selection.startCell.y(), tab->selection.endCell.y());
    int endX            = qMax(tab->selection.startCell.x(), tab->selection.endCell.x());
    int endY            = qMax(tab->selection.startCell.y(), tab->selection.endCell.y());

    // Преобразуем ячейки в пиксельные координаты
    int pixelStartX     = startX * tab->cellSizeWidth;
    int pixelStartY     = startY * tab->cellSizeHeight;
    int pixelEndX       = (endX + 1) * tab->cellSizeWidth;
    int pixelEndY       = (endY + 1) * tab->cellSizeHeight;

    // Проверяем границы изображения
    pixelEndX           = qMin(pixelEndX, tab->imageWidth);
    pixelEndY           = qMin(pixelEndY, tab->imageHeight);

    int width           = pixelEndX - pixelStartX;
    int height          = pixelEndY - pixelStartY;

    if (width <= 0 || height <= 0) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Некорректная область выделения"));
        return;
    }

    // Создаем изображение выделенной области из исходного изображения
    QImage selectionImage = tab->background.toImage().copy(pixelStartX, pixelStartY, width, height);

    // Копируем в буфер обмена
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setImage(selectionImage);
}

void MainWindow::onSelectionInfo()
{

}

void MainWindow::onPaste()
{
    ImageTabStruct* tab = currentTab();
    if (!tab) {
        return;
    }

    QClipboard* clipboard = QApplication::clipboard();
    QImage clipboardImage = clipboard->image();

    if (clipboardImage.isNull()) {
        return;
    }

    if (isPasting_) {
        cancelPaste();
    }

    // Создаем preview элемент
    QPixmap pastePixmap = QPixmap::fromImage(clipboardImage);
    pastePreviewItem_   = tab->scene->addPixmap(pastePixmap);
    pastePreviewItem_->setZValue(PREV_SCENE_POS);

    isPasting_ = true;

    // Включаем отслеживание мыши
    QGraphicsView* view = currentGraphicsView();
    if (view) {
        view->setMouseTracking(true);
        view->viewport()->setMouseTracking(true);
    }
}

void MainWindow::cancelPaste()
{
    if (!isPasting_) {
        return;
    }

    ImageTabStruct* tab = currentTab();
    if (tab && pastePreviewItem_) {
        tab->scene->removeItem(pastePreviewItem_);
        delete pastePreviewItem_;
        pastePreviewItem_ = nullptr;
    }

    isPasting_ = false;

    // Восстанавливаем обычный курсор
    QGraphicsView* view = currentGraphicsView();
    if (view) {
        view->setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::commitPaste(const QPointF &scenePos)
{
    ImageTabStruct* tab = currentTab();
    if (!tab || !pastePreviewItem_) return;

    // Получаем изображение для вставки
    QImage imageToPaste = pastePreviewItem_->pixmap().toImage();

    // Вычисляем координаты вставки
    int pasteX = qRound(scenePos.x());
    int pasteY = qRound(scenePos.y());

    // Проверяем границы изображения
    if (pasteX < 0) pasteX = 0;
    if (pasteY < 0) pasteY = 0;
    if (pasteX >= tab->imageWidth)  pasteX = tab->imageWidth  - 1;
    if (pasteY >= tab->imageHeight) pasteY = tab->imageHeight - 1;

    // Определяем область вставки
    int pasteWidth  = qMin(imageToPaste.width(), tab->imageWidth - pasteX);
    int pasteHeight = qMin(imageToPaste.height(), tab->imageHeight - pasteY);

    if (pasteWidth <= 0 || pasteHeight <= 0) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Нельзя вставить за границами изображения"));
        cancelPaste();
        return;
    }

    // Создаем копию основного изображения
    QImage updatedImage = tab->background.toImage();

    if (snappingEnabled_)
    {
        pasteX = neighboringPeak_.x();
        pasteY = neighboringPeak_.y();
    }

    // Вставляем пиксели в основное изображение
    QPainter painter(&updatedImage);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(pasteX, pasteY, imageToPaste, 0, 0, pasteWidth, pasteHeight);
    painter.end();

    updateBackgroundImage(updatedImage);
    cancelPaste();
}

void MainWindow::onZoom(int value)
{
    ImageTabStruct* tab = currentTab();
    if (!tab) return;

    QGraphicsView* view = currentGraphicsView();
    if (!view) return;

    float zoom = utils::mapToRange<float>(value, MIN_ZOOM_PERCENTAGE, MAX_ZOOM_PERCENTAGE, MIN_ZOOM_VALUE, MAX_ZOOM_VALUE);
    view->setTransform(QTransform::fromScale(zoom, zoom));

    tab->currentZoomPercentage = value;
    tab->currentZoomValue = zoom;
}

void MainWindow::updateBackgroundImage(const QImage &newImage)
{
    ImageTabStruct* tab = currentTab();
    if (!tab) {
        return;
    }

    if (newImage.isNull()) {
        return;
    }

    // Обновляем фоновое изображение
    tab->background = QPixmap::fromImage(newImage);

    auto items = tab->scene->items();
    foreach (QGraphicsItem* item, items) {
        if (item->zValue() == MAIN_SCENE_POS) {
            auto bgItem = static_cast<QGraphicsPixmapItem*>(item);
            bgItem->setPixmap(tab->background);
            break;
        }
    }

    this->update();
}

void MainWindow::updateSnapIndicator(const QPointF &mousePos, ImageTabStruct *tab)
{
    if (!tab) {
        return;
    }

    // Удаляем старый индикатор
    if (snapIndicator_) {
        tab->scene->removeItem(snapIndicator_);
        delete snapIndicator_;
        snapIndicator_ = nullptr;
    }

    // Находим ближайшую вершину сетки
    neighboringPeak_ = getNeighboringPeak(mousePos, tab);

    // Показываем индикатор на вершине сетки
    if (isPasting_)
    {
        float snapIndicatorRadius = snapIndicatorDiameter / 2.0f;
        snapIndicator_ = tab->scene->addEllipse(neighboringPeak_.x() - snapIndicatorRadius, neighboringPeak_.y() - snapIndicatorRadius,
                                                snapIndicatorDiameter, snapIndicatorDiameter, QPen(Qt::yellow, 2), QBrush(Qt::yellow));
        snapIndicator_->setZValue(SNAP_SCENE_POS);
    }
}

QPointF MainWindow::getNeighboringPeak(const QPointF &mousePos, ImageTabStruct *tab)
{
    if (!tab) {
        return mousePos;
    }

    // Вычисляем индексы ячейки
    int cellX   = qRound(mousePos.x() / tab->cellSizeWidth);
    int cellY   = qRound(mousePos.y() / tab->cellSizeHeight);

    // Ограничиваем границами сетки
    cellX       = qBound(0, cellX, tab->gridColumns);
    cellY       = qBound(0, cellY, tab->gridRows);

    // Возвращаем позицию вершины сетки
    return QPointF(cellX * tab->cellSizeWidth, cellY * tab->cellSizeHeight);
}

void MainWindow::onTabChanged(int index)
{
    ImageTabStruct* tab = currentTab();
    if (!tab) return;

    // Обновляем параметры
    ui->spinZoom->setValue(tab->currentZoomPercentage);

    // Важно! Фокус устанавливаем в последнюю очередь, чтобы его не забрали на себя другие виджеты
    auto view = qobject_cast<QGraphicsView*>(ui->tabWidget->widget(index));
    if (view) {
        view->setFocus();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    QGraphicsView* currentView = currentGraphicsView();
    if (!currentView || watched != currentView->viewport()) {
        return QMainWindow::eventFilter(watched, event);
    }

    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

    switch (event->type()) {
    case QEvent::MouseButtonPress:
        onGraphicsViewMousePress(mouseEvent, currentView);
        return true;
    case QEvent::MouseMove:
        onGraphicsViewMouseMove(mouseEvent, currentView);
        return true;
    case QEvent::MouseButtonRelease:
        onGraphicsViewMouseRelease(mouseEvent);
        return true;
    default:
        break;
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        if (isPasting_) {
            cancelPaste();
        } else {
            clearSelection();
        }
    }
    else if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_V) {
        onPaste();
    }

    QMainWindow::keyPressEvent(event);
}
