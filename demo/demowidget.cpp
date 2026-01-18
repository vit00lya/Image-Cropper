#include "demowidget.h"
#include <imagecropper.h>

#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QPixmap>
#include <QScrollArea>
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileInfo>
#include <QDir>

DemoWidget::DemoWidget(QWidget* _parent) :
	QWidget(_parent)
{
	// Создаем меню
	m_menuBar = new QMenuBar(this);
	QMenu* fileMenu = m_menuBar->addMenu(tr("Файл"));
	m_openAction = fileMenu->addAction(tr("Открыть"));
	connect(m_openAction, SIGNAL(triggered()), this, SLOT(openImage()));
	
	// Настраиваем виджет обрезки изображений
	m_imageCropper = new ImageCropper(this);
	m_imageCropper->resize( 1024, 1024 );
	m_imageCropper->setProportion(QSize(4,3));
	m_imageCropper->setBackgroundColor( Qt::lightGray );
	m_imageCropper->setCroppingRectBorderColor( Qt::magenta);
	// Использовать фиксированные пропорции
	QCheckBox* fixedProportionsCombo = new QCheckBox("Фиксировать пропорции (4x3)", this);
	connect(fixedProportionsCombo, SIGNAL(toggled(bool)),
			m_imageCropper, SLOT(setProportionFixed(bool)));
	// Кадрировать выделенную область
	QPushButton* cropBtn = new QPushButton("Вырезать фрагмент", this);
	connect(cropBtn, SIGNAL(clicked()), this, SLOT(crop()));
	
	// Область для вставки обрезанного изображения
	m_scrollArea = new QScrollArea(this);
	m_scrollWidget = new QWidget(this);
	m_scrollLayout = new QHBoxLayout(m_scrollWidget); // Используем QHBoxLayout для горизонтального расположения
	m_scrollLayout->setAlignment(Qt::AlignLeft); // Выравнивание по левому краю
	m_scrollWidget->setMinimumSize(3000, 350); // Устанавливаем минимальный размер для виджета
	m_scrollWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Разрешаем растягивание по горизонтали
	m_scrollArea->setWidget(m_scrollWidget);
	m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_scrollArea->setWidgetResizable(false); // Запрещаем изменение размера виджета
	
	// Кнопка удаления изображения
	m_deleteButton = new QPushButton("Удалить изображение", this);
	m_deleteButton->setEnabled(false); // Отключаем кнопку по умолчанию
	connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteImage()));
	
	// Кнопка выбора папки
	m_selectFolderButton = new QPushButton("Выбрать папку для сохранения фрагментов", this);
	connect(m_selectFolderButton, SIGNAL(clicked()), this, SLOT(selectFolder()));
	
	// Метка для отображения выбранного пути
	m_folderPathLabel = new QLabel("Путь к папке не выбран", this);
	
	// Инициализируем индекс выбранного изображения
	m_selectedImageIndex = -1;
	
	// Создаем горизонтальный layout для кнопок crop и удаления
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(cropBtn);
	buttonLayout->addWidget(m_deleteButton);
	buttonLayout->addStretch(); // Добавляем растягивающийся элемент для выравнивания кнопок по левому краю
	
	// Создаем горизонтальный layout 
	QHBoxLayout* systemLayout = new QHBoxLayout();
	systemLayout->addWidget(fixedProportionsCombo);
	systemLayout->addWidget(m_selectFolderButton);
	systemLayout->addWidget(m_folderPathLabel);
	systemLayout->addStretch(); // Добавляем растягивающийся элемент для выравнивания по левому краю
	
	// Настраиваем отображение
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMenuBar(m_menuBar);
	layout->addWidget(m_imageCropper);
	layout->addLayout(buttonLayout); // Добавляем горизонтальный layout с кнопками crop и удаления
	layout->addLayout(systemLayout); // Добавляем горизонтальный layout для выбора папки
	layout->addWidget(m_scrollArea);
	layout->setStretchFactor(m_imageCropper, 1);
}

void DemoWidget::crop()
{
	// Проверяем, загружено ли изображение
	if (m_imageCropper->cropImage().isNull()) {
		return;
	}
	
	// Проверяем, не превышено ли максимальное количество изображений (7)
	if (m_croppedImages.size() >= 7) {
		// Показываем предупреждение
		QMessageBox::warning(this, tr("Предупреждение"), tr("Максимальное количество изображений - 7. Удалите одно изображение перед добавлением нового."));
		return;
	}
	
	// Создаем новый QLabel для обрезанного изображения
	QLabel* newCroppedImage = new QLabel(this);
	newCroppedImage->setFixedSize(400, 300); // Устанавливаем фиксированный размер 400x300
	newCroppedImage->setScaledContents(true); // Включаем масштабирование содержимого
	newCroppedImage->setPixmap(m_imageCropper->cropImage());
	
	// Делаем QLabel кликабельным
	newCroppedImage->setStyleSheet("QLabel { border: 1px solid gray; }");
	
	// Создаем фрейм для обрезанного изображения с возможностью выбора
	QFrame* imageFrame = new QFrame(this);
	imageFrame->setFixedSize(410, 310); // Немного больше QLabel для рамки
	imageFrame->setStyleSheet("QFrame { border: 1px solid gray; }");
	imageFrame->setCursor(Qt::PointingHandCursor); // Устанавливаем курсор указателя
	
	// Помещаем QLabel внутрь фрейма
	QVBoxLayout* frameLayout = new QVBoxLayout(imageFrame);
	frameLayout->addWidget(newCroppedImage);
	frameLayout->setContentsMargins(5, 5, 5, 5); // Отступы внутри фрейма
	
	// Подключаем сигнал клика к слоту выбора изображения
	imageFrame->installEventFilter(this); // Устанавливаем фильтр событий
	// Обрабатываем клик по фрейму
	QObject::connect(imageFrame, &QFrame::windowTitleChanged, [this, imageFrame, newCroppedImage]() {
		// Сбрасываем стиль всех фреймов
		for (int i = 0; i < m_scrollLayout->count(); ++i) {
			QLayoutItem* item = m_scrollLayout->itemAt(i);
			if (QWidget* widget = item->widget()) {
				if (QFrame* frame = qobject_cast<QFrame*>(widget)) {
					frame->setStyleSheet("QFrame { border: 1px solid gray; }");
				}
			}
		}
		// Устанавливаем стиль для выбранного фрейма
		imageFrame->setStyleSheet("QFrame { border: 2px solid red; }");
		// Сохраняем индекс выбранного изображения
		m_selectedImageIndex = m_croppedImages.indexOf(newCroppedImage);
		// Включаем кнопку удаления
		m_deleteButton->setEnabled(true);
	});
	
	// Добавляем фрейм в layout
	m_scrollLayout->addWidget(imageFrame); // Добавляем фрейм в конец layout
	
	// Добавляем новый QLabel в список
	m_croppedImages.append(newCroppedImage);
}

bool DemoWidget::eventFilter(QObject* obj, QEvent* event)
{
	// Проверяем, является ли объект фреймом изображения и произошло ли событие клика мыши
	if (event->type() == QEvent::MouseButtonPress) {
		// Проверяем, есть ли объект в списке фреймов
		for (int i = 0; i < m_scrollLayout->count(); ++i) {
			QLayoutItem* item = m_scrollLayout->itemAt(i);
			if (QWidget* widget = item->widget()) {
				if (widget == obj) {
					// Сбрасываем стиль всех фреймов
					for (int j = 0; j < m_scrollLayout->count(); ++j) {
						QLayoutItem* item2 = m_scrollLayout->itemAt(j);
						if (QWidget* widget2 = item2->widget()) {
							widget2->setStyleSheet("QFrame { border: 1px solid gray; }");
						}
					}
					// Устанавливаем стиль для выбранного фрейма
					widget->setStyleSheet("QFrame { border: 2px solid red; }");
					// Сохраняем индекс выбранного изображения
					m_selectedImageIndex = i;
					// Включаем кнопку удаления
					m_deleteButton->setEnabled(true);
					return true; // Событие обработано
				}
			}
		}
	}
	// Передаем событие дальше
	return QWidget::eventFilter(obj, event);
}

void DemoWidget::openImage()
{

	// Проверяем, выбрана ли папка для сохранения
	if (m_folderPathLabel->text() == "Путь к папке не выбран") {
		// Показываем предупреждение
		QMessageBox::warning(this, tr("Предупреждение"), tr("Пожалуйста, выберите папку для сохранения изображений перед открытием файла."));
		return;
	}

	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Открыть изображение"), "", tr("Изображения (*.png *.jpg *.bmp)"));
	
	if (!fileName.isEmpty()) {
		// Сохраняем имя файла без пути и расширения
		QFileInfo fileInfo(fileName);
		m_currentFileName = fileInfo.baseName();
		
		m_imageCropper->setImage(QPixmap(fileName));
	}
}

void DemoWidget::selectFolder()
{
	// Открываем диалог выбора папки
	QString folderPath = QFileDialog::getExistingDirectory(this, tr("Выберите папку"));
	
	// Если папка выбрана, обновляем метку с путем
	if (!folderPath.isEmpty()) {
		m_folderPathLabel->setText(folderPath);
	}
}

void DemoWidget::deleteImage()
{
	// Проверяем, выбрано ли изображение
	if (m_selectedImageIndex >= 0 && m_selectedImageIndex < m_croppedImages.size()) {
		// Получаем QLabel выбранного изображения
		QLabel* selectedImage = m_croppedImages.at(m_selectedImageIndex);
		
		// Удаляем QLabel из layout
		m_scrollLayout->removeWidget(selectedImage->parentWidget());
		
		// Удаляем родительский виджет (фрейм) из памяти
		selectedImage->parentWidget()->deleteLater();
		
		// Удаляем QLabel из списка
		m_croppedImages.removeAt(m_selectedImageIndex);
		
		// Сбрасываем индекс выбранного изображения
		m_selectedImageIndex = -1;
		
		// Отключаем кнопку удаления
		m_deleteButton->setEnabled(false);
	}
}

void DemoWidget::closeEvent(QCloseEvent* event)
{
	// Проверяем, есть ли изображения для сохранения
	if (!m_croppedImages.isEmpty()) {
		// Получаем путь к папке для сохранения
		QString folderPath = m_folderPathLabel->text();
		
		// Проверяем, выбрана ли папка для сохранения
		if (folderPath != "Путь к папке не выбран") {
			// Получаем имя исходного файла (если оно доступно)
			QString baseFileName =  m_currentFileName + "_fragment_";
			
			// Сохраняем все фрагменты
			for (int i = 0; i < m_croppedImages.size(); ++i) {
				// Формируем имя файла
				QString fileName = QString("%1/%2_%3.jpg").arg(folderPath).arg(baseFileName).arg(i + 1);
				
				// Получаем QPixmap из QLabel
				QPixmap pixmap = m_croppedImages.at(i)->pixmap(Qt::ReturnByValue);
				
				// Сохраняем изображение
				pixmap.save(fileName, "PNG");
			}
		}
	}
	
	// Вызываем реализацию базового класса
	QWidget::closeEvent(event);
}

void DemoWidget::setInitialParameters(const QString& imagePath, const QString& folderPath)
{
	// Проверяем, существует ли файл изображения
	if (QFileInfo::exists(imagePath)) {
		// Загружаем изображение
		m_imageCropper->setImage(QPixmap(imagePath));
		
		// Сохраняем имя файла без пути и расширения
		QFileInfo fileInfo(imagePath);
		m_currentFileName = fileInfo.baseName();
	}
	
	// Проверяем, существует ли папка
	if (QDir(folderPath).exists()) {
		// Устанавливаем путь к папке
		m_folderPathLabel->setText(folderPath);
	}
}
