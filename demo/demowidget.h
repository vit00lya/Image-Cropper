#ifndef DEMOWIDGET_H
#define DEMOWIDGET_H

#include <QWidget>
#include <QList>
#include <QPushButton>

class ImageCropper;
class QLabel;
class QMenuBar;
class QAction;
class QScrollArea;
class QHBoxLayout;

class DemoWidget : public QWidget
{
	Q_OBJECT
public:
	explicit DemoWidget(QWidget* _parent = 0);
	
protected:
	void closeEvent(QCloseEvent* event) override;
	
signals:
	void windowClosed(); // Сигнал для уведомления о закрытии окна
	
protected:
	bool eventFilter(QObject* obj, QEvent* event) override;
	private slots:
		void crop();
		void openImage();
		void deleteImage();
		void selectFolder();
		
	public:
		void setInitialParameters(const QString& imagePath, const QString& folderPath);
		

private:
	ImageCropper* m_imageCropper;
	QList<QLabel*> m_croppedImages; // Список для хранения всех обрезанных изображений
	QScrollArea* m_scrollArea;
	QWidget* m_scrollWidget;
	QHBoxLayout* m_scrollLayout;
	QMenuBar* m_menuBar;
	QAction* m_openAction;
	QPushButton* m_deleteButton;
	QPushButton* m_selectFolderButton;
	QLabel* m_folderPathLabel;
	QString m_currentFileName; // Имя текущего открытого файла
	int m_selectedImageIndex;
};

#endif // DEMOWIDGET_H
