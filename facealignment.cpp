#include "facealignment.h"
#include "AffineAlign.h"
#include "openCVToQt.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtGui/QImageReader>

FaceAlignment::FaceAlignment(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	image = 0;
	
	pixmapItem = 0;

	scene = new myGraphicsScene(this);

	createConnections();
	createMenus();
	initializeView();

}

FaceAlignment::~FaceAlignment()
{

}

void FaceAlignment::createMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(ui.action_Load_Image);

	menuBar()->addMenu(fileMenu);
}

void FaceAlignment::createConnections()
{
	connect(ui.action_Load_Image, SIGNAL(triggered()), this, SLOT(loadFile()));
	connect(scene, SIGNAL(sceneMousePressed()), this, SLOT(sceneMousePressedHandler()));
	connect(ui.sliderOffsetPctX, SIGNAL(valueChanged(int)), this, SLOT(runAlgorithm()));
	connect(ui.sliderOffsetPctY, SIGNAL(valueChanged(int)), this, SLOT(runAlgorithm()));
	connect(ui.sliderOutputWidth, SIGNAL(valueChanged(int)), this, SLOT(runAlgorithm()));
	connect(ui.sliderOutputHeight, SIGNAL(valueChanged(int)), this, SLOT(runAlgorithm()));
}

void FaceAlignment::initializeView()
{
	graphicsView = new myGraphicsView(scene, ui.frame_3);
	graphicsView->setObjectName(QStringLiteral("graphicsView"));
	graphicsView->setCursor(Qt::CrossCursor);

	ui.gridLayout_4->addWidget(graphicsView, 0, 0, 1, 1);

	graphicsView->show();
}

void FaceAlignment::sceneMousePressedHandler()
{
	ui.label_LeftEyeXY->setText(QString("(%1,%2)").arg(scene->LeftEyeX()).arg(scene->LeftEyeY()));
	ui.label_RightEyeXY->setText(QString("(%1,%2)").arg(scene->RightEyeX()).arg(scene->RightEyeY()));
	//runAlgorithm();
}

bool FaceAlignment::openFile(const QString &fileName)
{
	if (image)
	{
		delete image;
	}

	image = new QImage(fileName);
	if (image->isNull()) {
		QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
			tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
		setWindowFilePath(QString());

		return false;
	}

	//scene->clear();

	if (!pixmapItem)
	{
		pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
		scene->addItem(pixmapItem);
		//scene->addPixmap(QPixmap::fromImage(*image));
	} 
	else
	{
		pixmapItem->setPixmap(QPixmap::fromImage(*image));
	}
	
	scene->setSceneRect(0, 0, image->width(), image->height());

	disconnect(ui.sliderOutputWidth, SIGNAL(valueChanged(int)), this, SLOT(runAlgorithm()));
	disconnect(ui.sliderOutputHeight, SIGNAL(valueChanged(int)), this, SLOT(runAlgorithm()));
	ui.sliderOutputWidth->setValue(image->width());
	ui.sliderOutputHeight->setValue(image->height());
	connect(ui.sliderOutputWidth, SIGNAL(valueChanged(int)), this, SLOT(runAlgorithm()));
	connect(ui.sliderOutputHeight, SIGNAL(valueChanged(int)), this, SLOT(runAlgorithm()));

	setWindowFilePath(fileName);

	return true;
}

void FaceAlignment::loadFile()
{
	QStringList mimeTypeFilters;

	QList<QByteArray> supportedMimeTypes = QImageReader::supportedMimeTypes();

	for (int i = 0; i < supportedMimeTypes.size(); i++)
	{
		mimeTypeFilters.append(supportedMimeTypes.at(i));
	}
		
	mimeTypeFilters.sort();
	const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
	QFileDialog dialog(this, tr("Open File"),
		picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
	dialog.setAcceptMode(QFileDialog::AcceptOpen);

	mimeTypeFilters.clear();
	mimeTypeFilters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";

	dialog.setNameFilters(mimeTypeFilters);

	//dialog.setMimeTypeFilters(mimeTypeFilters);
	//dialog.selectMimeTypeFilter("image/jpeg");
	//dialog.selectMimeTypeFilter("image/png");

	while (dialog.exec() == QDialog::Accepted && !openFile(dialog.selectedFiles().first())) {}
}


void FaceAlignment::runAlgorithm()
{
	//convert from QImage to Mat image

	if (scene->LeftEyeX()<0 | scene->LeftEyeY()<0 | scene->RightEyeX()<0 | scene->RightEyeY()<0 )
	{
		QMessageBox msg;
		msg.setText(QString("Eye locations are invalid. Please select the left eye location with mouse left button, right eye location with mouse right button!"));
		msg.exec();
		return;
	} 
	else
	{
		cv::Mat input = qimage_to_mat_ref(*image);

		Point leftEye(scene->LeftEyeX(), scene->LeftEyeY());
		Point rightEye(scene->RightEyeX(), scene->RightEyeY());
		vector<Point> eyeCoords;
		eyeCoords.push_back(leftEye);
		eyeCoords.push_back(rightEye);
		Size newSize(ui.sliderOutputWidth->value(), ui.sliderOutputHeight->value());

		float offsetPctX = (float) ui.sliderOffsetPctX->value()/100;
		float offsetPctY = (float) ui.sliderOffsetPctY->value()/100;

		Mat dst;
		AffineAlign alignerAlgorithm;
		alignerAlgorithm.alignFace(input, eyeCoords, newSize, dst, offsetPctX, offsetPctY);

		// convert back from Mat image to QImage and update scene
		QImage imageNew = mat_to_qimage_ref(dst);

		pixmapItem->setPixmap(QPixmap::fromImage(imageNew));

		scene->setSceneRect(0, 0, imageNew.width(), imageNew.height());

		if (image)
		{
			delete image;
		}
		image = new QImage(imageNew);
	}

}
